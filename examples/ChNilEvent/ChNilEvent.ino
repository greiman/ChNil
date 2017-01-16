// Demonstration of events.
// AVR is configured for uint8_t event mask but could be changed to uint16_t.

#include "ChNil.h"

// Use tiny unbuffered ChNilSerial library.
// Macro to redefine Serial as ChNilSerial to save RAM.
// Remove definition to use standard Arduino Serial.
ChNilSerialClass ChNilSerial;
#define Serial ChNilSerial
//------------------------------------------------------------------------------
thread_t* thread1;
thread_t* thread2;
thread_t* listenThread = 0;
//------------------------------------------------------------------------------
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread1, 64);

// Declare thread function for thread 1.
THD_FUNCTION(Thread1, arg) {
  (void)arg;
  thread1 = chThdGetSelfX();
  while (!listenThread) {
    chThdSleep(10);
  }
  while (TRUE) {
    // Use event ID 0 for this thread.  AVR allows event ids 0 - 7.
    chEvtSignal(listenThread, EVENT_MASK(0));
    
    // Wait for signal from listner.    
    eventmask_t m = chEvtWaitAnyTimeout(ALL_EVENTS, 1000);
    if (m != EVENT_MASK(0)) {
      Serial.print("Thread1 err, m: ");
      Serial.println(m);
      break;
    }
    chThdSleepMilliseconds(1000);
  }
  chThdSleep(TIME_INFINITE);  
}
//------------------------------------------------------------------------------
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread2, 64);

// Declare thread function for thread 2.
THD_FUNCTION(Thread2, arg) {
  (void)arg;
  thread2 = chThdGetSelfX();    
  while (!listenThread) {
    chThdSleep(10);
  }
  while (TRUE) {
    // Use event ID 0 for this thread.  AVR allows event ids 0 - 7.    
    chEvtSignal(listenThread, EVENT_MASK(1));
    
    // Wait for signal from listner.
    eventmask_t m = chEvtWaitAnyTimeout(ALL_EVENTS, 1000);
    if (m != EVENT_MASK(1)) {
      Serial.print("Thread2 err, m: ");
      Serial.println(m);
      break;
    }
    chThdSleepMilliseconds(1100);
  }
  chThdSleep(TIME_INFINITE);
}
//------------------------------------------------------------------------------
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread3, 64);

// Declare thread function for thread 3.
THD_FUNCTION(Thread3, arg) {
  (void)arg;
  listenThread = chThdGetSelfX();
  while (TRUE) {
    eventmask_t m = chEvtWaitAnyTimeout(ALL_EVENTS, TIME_INFINITE);
    if (m & EVENT_MASK(0)) {
      chEvtSignal(thread1, EVENT_MASK(0));
      Serial.println("Signal from Thread1");
    }
    if (m & EVENT_MASK(1)) {
      chEvtSignal(thread2, EVENT_MASK(1));
      Serial.println("Signal from Thread2");
    }
  }
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
  THD_TABLE_ENTRY(waThread1, NULL, Thread1, NULL)
  THD_TABLE_ENTRY(waThread2, NULL, Thread2, NULL)
  THD_TABLE_ENTRY(waThread3, NULL, Thread3, NULL)
THD_TABLE_END
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  // start kernel
  chBegin();
}
//------------------------------------------------------------------------------
// Loop is the idle thread.  The idle thread must not invoke any
// kernel primitive able to change its state to not runnable.
void loop() {
}