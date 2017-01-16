// Simple ADC logger that demonstrates mailboxes, NilAnalog, and ChNilTimer1.
#include "SdFat.h"
#include "ChNil.h"


// Use tiny unbuffered ChNilSerial library.
// Macro to redefine Serial as ChNilSerial.
// Remove definition to use standard Arduino Serial.
ChNilSerialClass ChNilSerial;
#define Serial ChNilSerial
//------------------------------------------------------------------------------
// Time between points in microseconds.
// Maximum value is 4,194,304 (2^22) usec.
const uint32_t PERIOD_USEC = 1000;

// Number of ADC channels to log
const uint8_t NADC = 2;

// Number of mailbox slots.
#define NUM_BUFFERS 200
 
static msg_t adc_data_queue[NUM_BUFFERS];
static mailbox_t adc_data;
//------------------------------------------------------------------------------
// SD card chip select pin.
const uint8_t sdChipSelect = SS;

// SD file system.
SdFat sd;

// Log file.
SdFile file;
//------------------------------------------------------------------------------
// Declare ADC semaphore and IRQ.
CH_NIL_ADC_IRQ
// Declare timer1 semaphore and IRQ.
CH_NIL_TIMER1_IRQ
//------------------------------------------------------------------------------
// Declare a stack with 16 bytes beyond context switch and interrupt needs.
// The highest priority thread requires less stack than other threads.
THD_WORKING_AREA(waThread1, 16);

// Declare thread function for thread 1.
THD_FUNCTION(Thread1, arg) {
  uint16_t overrun = 0;
  (void)arg;

  // Check for very fast reads.
  if (NADC*160 > PERIOD_USEC) {
    // Set ADC prescaler to 64 for very fast reads.
    chAnalogPrescalar(ADC_PS_64);
  }
  // Initialize ADC by doing first read.
  chAnalogRead(0);

  // Start timer 1 with a period of PERIOD_USEC.
  chTimer1Start(PERIOD_USEC);

  // Record data until serial input is available.
  while (!Serial.available()) { 
    // Sleep until it's time for next data point.
    if (!chTimer1Wait()) {
      Serial.println("time1 err");
      break;
    }
    chSysLock();
    cnt_t nfree = chMBGetFreeCountI(&adc_data);
    chSysUnlock();
    if (nfree < NADC) {
      overrun++;
      continue;
    }
    // Read ADCs
    for (int i = 0; i < NADC; i++) {
      msg_t data = chAnalogRead(i);
      if (overrun && i == 0) data |= 0X8000;
      if (i == (NADC-1)) data |= 0X4000;
      if (MSG_OK != chMBPost(&adc_data, data, TIME_IMMEDIATE)) {
        Serial.println("mbox err");
        break;        
      }
    }
    overrun = 0;
  }
  // Done, sleep forever.
  chThdSleep(TIME_INFINITE);
}
//------------------------------------------------------------------------------
/*
 * Threads static table, one entry per thread.  A thread's priority is
 * determined by its position in the table with highest priority first.
 *
 * These threads start with a null argument.  A thread's name is also
 * null to save RAM since the name is currently not used.
 */
THD_TABLE_BEGIN
  THD_TABLE_ENTRY(waThread1, "thread1", Thread1, NULL)
THD_TABLE_END
//------------------------------------------------------------------------------
void setup() {

  Serial.begin(9600);
  Serial.println(F("type any character to begin"));
  while (Serial.read() < 0);

  // Initialize SD and create or open and truncate the data file.
  if (!sd.begin(sdChipSelect)
    || !file.open("DATA.CSV", O_CREAT | O_WRITE | O_TRUNC)) {
    Serial.println(F("SD problem"));
    sd.errorHalt();
  }
  // Write the file header.
  file.print(F("PERIOD_USEC,"));
  file.println(PERIOD_USEC);
  for (int i = 0; i < NADC; i++) {
    file.print(F("ADC"));
    file.printField(i, ',');
  }
  file.println(F("Overruns"));

  // Throw away serial input.
  while (Serial.read() >= 0) {
    delay(10);
  }
  Serial.println(F("type any character to end"));
  chMBObjectInit(&adc_data, adc_data_queue, NUM_BUFFERS);
  // Start kernel.
  chFillStacks();
  chBegin();
}
//------------------------------------------------------------------------------
// Write data to the SD in the idle loop.
//
// Loop is the idle thread.  The idle thread must not invoke any
// kernel primitive able to change its state to not runnable.
void loop() {
  // Maximum SD write latency.
  uint32_t maxLatency = 0;
  uint32_t dataCount = 0;
  uint32_t overruns = 0;
  uint32_t timeMillis = millis();
  cnt_t maxUsed = 0;
  
  // Record data until serial data is available()
  while (!Serial.available()) {
    uint16_t data;
    chSysLock();
    cnt_t nused = chMBGetUsedCountI(&adc_data);
    chSysUnlock();
    if (nused > maxUsed) maxUsed = nused;      
    
    // Check for an available data record in the FIFO.
    msg_t msg = chMBFetch(&adc_data, (msg_t *)&data, TIME_IMMEDIATE);
     // Continue if no available data records in the FIFO.   
    if (msg != MSG_OK) {
      continue;
    }
    // Write start time.
    uint32_t u = micros();

    if (data & 0X8000) {
      file.println(F("overruns"));
      overruns++;
    }
    file.printField(data & 0X1FFF, data & 0X4000 ? '\n' : ',');
    u = micros() - u;
    if (u > maxLatency) maxLatency = u;
    dataCount++;
  }
  timeMillis = millis() - timeMillis;
  // Done, close the file and print stats.
  file.close();
  Serial.println(F("Done!"));
  Serial.print(F("Max Used Slots: "));
  Serial.println(maxUsed);
  Serial.print(F("Max Write Latency: "));
  Serial.print(maxLatency);
  Serial.println(F(" usec"));
  Serial.print(F("overruns: "));
  Serial.println(overruns);  
  Serial.print(F("dataCount: "));
  Serial.println(dataCount);
  Serial.print(F("timeMillis: "));
  Serial.println(timeMillis);  
  chPrintUnusedStack(&Serial);
  while (1) {}
}