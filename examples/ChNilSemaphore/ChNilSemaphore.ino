/* Example of a counting semaphore with three threads. */

#include "ChNil.h"

// Declare and initialize a semaphore for limiting access to a region.
SEMAPHORE_DECL(twoSlots, 2);
//------------------------------------------------------------------------------
// Declare the thread function for all threads.
THD_FUNCTION(thdFcn, name) {
 while (true) {

  // wait for slot
  chSemWait(&twoSlots);

    // Only two threads can be in this region at a time.
    Serial.println((char*)name);

    chThdSleep(1000);

    // Exit region.
    chSemSignal(&twoSlots);
    
    // Slight delay to allow a lower priority thread to enter above region.
    // Comment out the next line to see the priority effect.
    chThdSleep(10);
  }
}
//------------------------------------------------------------------------------
/* Declare a stack with 100 bytes beyond context switch and interrupt
 * needs for each thread.
 */
THD_WORKING_AREA(waTh1, 100);
THD_WORKING_AREA(waTh2, 100);
THD_WORKING_AREA(waTh3, 100);
/*
 * Threads static table, one entry per thread.  A thread's priority is
 * determined by its position in the table with highest priority first.
 *
 * These threads start with their name as their argument.  A thread's name
 * is null to save RAM since the name is currently not used.
 */
THD_TABLE_BEGIN
  THD_TABLE_ENTRY(waTh1,NULL, thdFcn, (void*)"Th 1")
  THD_TABLE_ENTRY(waTh2,NULL, thdFcn, (void*)"Th 2")
  THD_TABLE_ENTRY(waTh3,NULL, thdFcn, (void*)"Th 3")
THD_TABLE_END
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);


  chFillStacks();
  // Start Nil RTOS
  chBegin();
}
//------------------------------------------------------------------------------
// Loop is the idle thread.  The idle thread must not invoke any
// kernel primitive able to change its state to not runnable.
void loop() {
  /* not used */
}
