/* Example of a handler thread triggered from an ISR by using a semaphore.
 * The handler message with response time should occur between the
 * "High" and "Low" messages from thread 2.
 */
#include "ChNil.h"

// Uncomment the next two line to save RAM and flash
// with the smaller unbuffered ChNilSerial library.
//ChNilSerialClass ChNilSerial;
//#define Serial ChNilSerial

// Pin used to trigger interrupt.
const uint8_t INTERRUPT_PIN = 2;

// Declare and initialize the semaphore.
SEMAPHORE_DECL(isrSem, 0);

// Low bits of ISR entry time in micros.
volatile uint16_t tIsr = 0;
//------------------------------------------------------------------------------
/* Fake ISR, normally void isrFcn()
 * would be replaced by something like
 * NIL_IRQ_HANDLER(INT0_vect).
 */
void isrFcn() {

  /* On AVR this forces compiler to save registers r18-r31.*/
  CH_IRQ_PROLOGUE();
  
  /* IRQ handling code, preemptable if the architecture supports it.*/
  
  /* Save low bits of micros(). */
  tIsr = micros();
  
  /* Nothing on AVR.*/
  chSysLockFromISR();
  
  /* Invocation of some I-Class system APIs, never preemptable.*/
  
  /* Signal handler thread. */
  chSemSignalI(&isrSem);
  
  /* Nothing on AVR.*/
  chSysUnlockFromISR();
 
  /* More IRQ handling code, again preemptable.*/
 
  /* Epilogue performs rescheduling if required.*/
  CH_IRQ_EPILOGUE();
}
//------------------------------------------------------------------------------
// Handler thread for interrupt.

// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread1, 64);

// Declare thread function for thread 1.
THD_FUNCTION(Thread1, arg) {
  (void)arg;
  while (1) {
    // wait for event
    chSemWait(&isrSem);
    
    // Save time.
    uint16_t t = micros();    
    
    // Print message with elapsed time.

    Serial.print(F("Handler: "));
    Serial.print(t - tIsr);
    Serial.println(F(" usec"));
  }
}
//------------------------------------------------------------------------------
// Task that generates an interrupt by toggling INTERRUPT_PIN.

// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread2, 64);

// Declare thread function for thread 2.
THD_FUNCTION(Thread2, arg) {
  (void)arg;  
  Serial.begin(9600);
  
  pinMode(INTERRUPT_PIN, OUTPUT);
  
  // attach interrupt function
  attachInterrupt(0, isrFcn, RISING);
  
  while (1) {
    // Cause an interrupt.  This is normally done by external event.
    Serial.println(F("High"));
    digitalWrite(INTERRUPT_PIN, HIGH);
    
    // The interrupt should cause thread 1 to run here.
    
    // Set pin LOW.
    Serial.println(F("Low"));
    digitalWrite(INTERRUPT_PIN, LOW);
    
    // Print Stack stats.
   chPrintUnusedStack(&Serial);
    Serial.println();
    
    // Sleep for a second.
    chThdSleepMilliseconds(1000);
  }
}
//------------------------------------------------------------------------------
/*
 * Threads static table, one entry per thread.  A thread's priority is
 * determined by its position in the table with highest priority first.
 *
 * These threads start with a null argument.  A thread's name may also
 * be null to save RAM since the name is currently not used.
 */
THD_TABLE_BEGIN
  THD_TABLE_ENTRY(waThread1, "Thread1", Thread1, NULL)
  THD_TABLE_ENTRY(waThread2, "Thread2", Thread2, NULL)
THD_TABLE_END
//------------------------------------------------------------------------------
void setup() {
  chFillStacks();
  // Start Nil RTOS. 
  chBegin();
}
//------------------------------------------------------------------------------
// Loop is the idle thread.  The idle thread must not invoke any
// kernel primitive able to change its state to not runnable.
void loop() {
  // Not used.
}
