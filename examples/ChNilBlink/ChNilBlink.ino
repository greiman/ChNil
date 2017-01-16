/*
 * Example to demonstrate thread definition, semaphores, and thread sleep.
 */
#include "ChNil.h"

// LED_BUILTIN pin on Arduino is usually pin 13.

// Declare a semaphore with an inital counter value of zero.
SEMAPHORE_DECL(sem, 0);
//------------------------------------------------------------------------------
/*
 * Thread 1, turn the LED off when signalled by thread 2.
 */
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread1, 64);

// Declare the thread function for thread 1.
THD_FUNCTION(Thread1, arg) {
  (void)arg;
  while (TRUE) {
    
    // Wait for signal from thread 2.
    chSemWait(&sem);
    
    // Turn LED off.
    digitalWrite(LED_BUILTIN, LOW);
  }
}
//------------------------------------------------------------------------------
/*
 * Thread 2, turn the LED on and signal thread 1 to turn the LED off.
 */
// Declare a stack with 64 bytes beyond context switch and interrupt needs. 
THD_WORKING_AREA(waThread2, 64);

// Declare the thread function for thread 2.
THD_FUNCTION(Thread2, arg) {
  (void)arg;
  pinMode(LED_BUILTIN, OUTPUT);
  
  while (TRUE) {
    // Turn LED on.
    digitalWrite(LED_BUILTIN, HIGH);
    
    // Sleep for 200 milliseconds.
    chThdSleepMilliseconds(200);
    
    // Signal thread 1 to turn LED off.
    chSemSignal(&sem);
    
    // Sleep for 200 milliseconds.   
    chThdSleepMilliseconds(200);
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
  chBegin();
}
//------------------------------------------------------------------------------
// Loop is the idle thread.  The idle thread must not invoke any 
// kernel primitive able to change its state to not runnable.
void loop() {
  // Not used.
}