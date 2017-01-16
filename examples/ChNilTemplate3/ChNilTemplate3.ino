// Template for idle thread plus three schedulable threads

#include "ChNil.h"

// Use tiny unbuffered ChNilSerial library.
// Macro to redefine Serial as ChNilSerial to save RAM.
// Remove definition to use standard Arduino Serial.
ChNilSerialClass ChNilSerial;
#define Serial ChNilSerial
//------------------------------------------------------------------------------
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread1, 64);

// Declare thread function for thread 1.
THD_FUNCTION(Thread1, arg) {
  (void)arg;
  while (TRUE) {
  
    // Sleep so lower priority threads can execute.
    chThdSleep(100);
  }
}
//------------------------------------------------------------------------------
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread2, 64);

// Declare thread function for thread 2.
THD_FUNCTION(Thread2, arg) {
  (void)arg;
  while (TRUE) {

    // Sleep so lower priority threads can execute.
    chThdSleep(100);
  }
}
//------------------------------------------------------------------------------
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread3, 64);

// Declare thread function for thread 3.
THD_FUNCTION(Thread3, arg) {
  (void)arg;
  while (TRUE) {

    // Sleep so lower priority threads can execute.
    chThdSleep(100);
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

  chFillStacks();
  // start kernel
  chBegin();
}
//------------------------------------------------------------------------------
// Loop is the idle thread.  The idle thread must not invoke any
// kernel primitive able to change its state to not runnable.
uint32_t lastPrint = 0;
void loop() {
  if ((millis() - lastPrint) > 1000) {
    lastPrint = millis();
    chPrintStackSizes(&Serial);
    chPrintUnusedStack(&Serial);
    Serial.println();
  }
}
