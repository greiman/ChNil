// This example tests ChNilTimer1.
// Timer1 allows finer time units than sys tick.

#include "ChNil.h"

// Use tiny unbuffered ChNilSerial library.
// Macro to redefine Serial as ChNilSerial to save RAM.
// Remove definition to use standard Arduino Serial.
ChNilSerialClass ChNilSerial;
#define Serial ChNilSerial
//------------------------------------------------------------------------------
// Declare timer1 semaphore and IRQ.
CH_NIL_TIMER1_IRQ
//------------------------------------------------------------------------------
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread1, 64);

// Declare thread function for thread 1.
THD_FUNCTION(Thread1, arg) {
  (void)arg;
  uint16_t n = 0;
  Serial.println("Start");
  chTimer1Start(400000);  
  uint32_t last = micros();
  
  // Execute while loop every 0.4 seconds.
  while (TRUE) {
    chTimer1Wait();
    uint32_t t = micros();
    Serial.print(t - last);
    Serial.print(' ');
    Serial.println(n++);    
    last = t;
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
THD_TABLE_END
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  chBegin();
}
//------------------------------------------------------------------------------
// Loop is the idle thread.  The idle thread must not invoke any
// kernel primitive able to change its state to not runnable.
void loop() {
  // Not used
}