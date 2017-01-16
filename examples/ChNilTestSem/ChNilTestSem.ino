// Test semaphores.

#include "ChNil.h"

// Use tiny unbuffered ChNilSerial library.
// Macro to redefine Serial as ChNilSerial to save RAM.
// Remove definition to use standard Arduino Serial.
ChNilSerialClass ChNilSerial;
#define Serial ChNilSerial

SEMAPHORE_DECL(sem, 0);

#define chAssert1(c) chAssert2(c, #c)

#define chAssert2(c, m) {          \
  if (!(c)) {                      \
    Serial.print(F(m));            \
    Serial.print(", at line ");    \
    Serial.println(__LINE__);      \
    while(1);                      \
  }                                \
}
//------------------------------------------------------------------------------
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread1, 64);

// Declare thread function for thread 1.
THD_FUNCTION(Thread1, arg) {
  (void)arg;
  chAssert1(MSG_OK == chSemWait(&sem));
  chAssert1(MSG_RESET == chSemWait(&sem));
  chAssert1(MSG_OK == chSemWaitTimeout(&sem, 100));
  Serial.println("Thread1 Done");
  while (TRUE) {
   // Allow lower priority threads to execute.
   chThdSleep(1);
  }
}
//------------------------------------------------------------------------------
// Declare a stack with 64 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread2, 64);

// Declare thread function for thread 2.
THD_FUNCTION(Thread2, arg) {

  (void)arg;   uint32_t m = micros();
   m = micros() - m;
   Serial.print("micros overhead = ");
   Serial.println(m);

    chAssert1(sem.cnt == -1);
    m = micros();
    chSemSignal(&sem);
    m = micros() - m;
    Serial.print("sem switch tasks micros = ");
    Serial.println(m);
    chAssert1(sem.cnt == -1);
    chSemReset(&sem, 0);
    chAssert1(sem.cnt == -1);
    chSemSignal(&sem);
    chAssert1(sem.cnt == 0);

    m = micros();
    chAssert1(MSG_TIMEOUT == chSemWaitTimeout(&sem, TIME_IMMEDIATE));
    m = micros() - m;
    Serial.print("sem immediate timeout = ");
    Serial.println(m);
    chAssert1(sem.cnt == 0);
    chSemSignal(&sem);
    chAssert1(sem.cnt == 1);
    m = micros();
    chAssert1(MSG_OK == chSemWaitTimeout(&sem, 100));
    m = micros() - m;
    Serial.print("sem immediate OK = ");
    Serial.println(m);
    chAssert1(sem.cnt == 0);
    chAssert1(MSG_TIMEOUT == chSemWaitTimeout(&sem, 100));
    chAssert1(sem.cnt == 0);

    Serial.println("Thd2 Done");
    while (1);
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
THD_TABLE_END
//------------------------------------------------------------------------------
void setup() {

  Serial.begin(9600);

  chAssert1(sem.cnt == 0);

  // start kernel
  chBegin();
}
//------------------------------------------------------------------------------
// Loop is the idle thread.  The idle thread must not invoke any
// kernel primitive able to change its state to not runnable.
void loop() {
  // not used
}
