// compare sizes - MinBlinkArduino vs MinBlinkChNil

#include "ChNil.h"
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread1, 64);

// Declare the thread function for thread 1.
THD_FUNCTION(Thread1, arg) {
  (void)arg;
  pinMode(LED_BUILTIN, OUTPUT);  
  bool level = true;
  while (TRUE) {
    digitalWrite(LED_BUILTIN, level = !level);
    chThdSleep(500);    
  }
}
// Threads static table, one entry per thread.  A thread's priority is
// determined by its position in the table with highest priority first.
THD_TABLE_BEGIN
  THD_TABLE_ENTRY(waThread1, "thread1", Thread1, NULL)
THD_TABLE_END

void setup() {
  chBegin();
}
void loop() {
}
