/*
 * Example to demonstrate thread definition, thread sleep, the idle
 * thread, concurrent access to a variable, and ChNilSerial.
 */
#include "ChNil.h"

// Use tiny unbuffered ChNilSerial library.
// Macro to redefine Serial as ChNilSerial.
// Remove definition to use standard Arduino Serial.
ChNilSerialClass ChNilSerial;
#define Serial ChNilSerial

// LED_BUILTIN pin on Arduino is usually pin 13.

// Shared idle thread counter.
volatile uint32_t loopCount = 0;
//------------------------------------------------------------------------------
/*
 * Thread 1, blink the LED.
 */
// Declare a stack with 16 bytes beyond context switch and interrupt needs. 
THD_WORKING_AREA(waThread1, 32);

// Declare thread function for thread 1.
THD_FUNCTION(Thread1, arg) {
  (void)arg;
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Flash led every 200 ms.
  while (TRUE) {
  
    // Turn LED on.
    digitalWrite(LED_BUILTIN, HIGH);
    
    // Sleep for 50 milliseconds.
    chThdSleepMilliseconds(50);
    
    // Turn LED off.
    digitalWrite(LED_BUILTIN, LOW);
    
    // Sleep for 150 milliseconds.
    chThdSleepMilliseconds(150);
  }
}
//------------------------------------------------------------------------------
/*
 * Thread 2, print thread.
 */
// Declare a stack with 100 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread2, 100);

// Declare thread function for thread 2.
THD_FUNCTION(Thread2, arg) {
  (void)arg;  
  // Wake time for thread sleep.
  systime_t wakeTime = chVTGetSystemTimeX();
  
  Serial.begin(9600);
  
  while (TRUE) {   
    
    // Add ticks for one second.  The MS2ST macro converts ms to system ticks.
    wakeTime += MS2ST(1000);
    
    // Sleep until next second.
    chThdSleepUntil(wakeTime);
    
    // Print loopCount for previous second.
    Serial.print(F("Count: "));
    Serial.print(loopCount);
    Serial.print(", ");
    
    // Print unused stack for thread 1, thread 2, and idle thread.
    chPrintUnusedStack(&Serial);
    
    // Zero loopCount at start of each second.
    loopCount = 0;
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
  THD_TABLE_ENTRY(waThread1, "thread1", Thread1, NULL)
  THD_TABLE_ENTRY(waThread2, "thread2", Thread2, NULL)
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
  // Enter kernel lock mode to prevent access to loopCount by thread 2.
  chSysLock();
    
  // Increment of loopCount is atomic in kernel lock mode.
  loopCount++;
    
  // Leave kernel lock mode.
  chSysUnlock();
}