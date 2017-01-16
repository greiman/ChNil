/* Example to demonstrate the time require for a semaphore signal plus
 * a thread context switch.
 *
 * Connect a scope to LED_BUILTIN, usually pin 13.
 * Measure the difference in time between the first pulse with no context
 * switch and the second pulse started in thread 2 and ended in thread 1.
 * The difference should be about 12 usec on a 16 MHz 328 Arduino.
 */
#include "ChNil.h"

// Declare the semaphore used to trigger a context switch.
SEMAPHORE_DECL(sem, 0);
//------------------------------------------------------------------------------
/*
 * Thread 1 - high priority thread to set pin low.
 */
// Declare a stack with 128 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread1, 128);

// Declare thread function for thread 1.
THD_FUNCTION(Thread1, arg) {
  (void)arg;
  while (TRUE) {
    // Wait for semaphore signal from thread 2.
    chSemWait(&sem);
    
    // Set LED pin low to end second pulse.
    digitalWrite(LED_BUILTIN, LOW);
  }
}
//------------------------------------------------------------------------------
/*
 * Thread 2 - lower priority thread to toggle LED and trigger thread 1.
 */
// Declare a stack with 128 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread2, 128);

// Declare thread function for thread 2.
THD_FUNCTION(Thread2, arg) {
  (void)arg;
  pinMode(LED_BUILTIN, OUTPUT);
  
  while (TRUE) {
  
    // Generate first pulse to get time with no context switch.
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
    
    // Start second pulse.
    digitalWrite(LED_BUILTIN, HIGH);
    
    // Trigger a context switch to thread 1.
    chSemSignal(&sem);
    
    // Sleep until next tick, 1024 microseconds tick on Arduino.
    chThdSleep(1);
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
  // Start Nil RTOS.
  chBegin();
}
//------------------------------------------------------------------------------
void loop() {
  // Not used.
}
