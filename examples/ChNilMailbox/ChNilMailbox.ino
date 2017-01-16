
#include "ChNil.h"

// Use tiny unbuffered ChNilSerial library.
// Macro to redefine Serial as ChNilSerial.
// Remove definition to use standard Arduino Serial.
ChNilSerialClass ChNilSerial;
#define Serial ChNilSerial

#define NUM_BUFFERS 4
#define BUFFERS_SIZE 32
 
static char buffers[NUM_BUFFERS][BUFFERS_SIZE];
 
static msg_t free_buffers_queue[NUM_BUFFERS];
static mailbox_t free_buffers;
 
static msg_t filled_buffers_queue[NUM_BUFFERS];
static mailbox_t filled_buffers;
//-----------------------------------------------------------------------------
// Declare a stack with 128 bytes beyond context switch and interrupt needs.
THD_WORKING_AREA(waThread1, 128);

// Declare the thread function for thread 1.
THD_FUNCTION(Thread1, arg) {
  (void)arg;
  
  while (true) {
    chThdSleepMilliseconds(2000);
    void *pbuf;
 
    /* Waiting for a filled buffer.*/
    msg_t msg = chMBFetch(&free_buffers, (msg_t *)&pbuf, TIME_INFINITE);
    if (msg != MSG_OK) {
      Serial.println("fetch free failed");
      chThdSleep(TIME_INFINITE);
    }      
    sprintf((char*)pbuf, "%ld", micros());
    (void)chMBPost(&filled_buffers, (msg_t)pbuf, TIME_INFINITE);    
  }
}
//-----------------------------------------------------------------------------
// Declare a stack with 128 bytes beyond context switch and interrupt needs. 
THD_WORKING_AREA(waThread2, 128);

// Declare the thread function for thread 2.
THD_FUNCTION(Thread2, arg) {
  (void)arg;
  while (true) {
    void *pbuf;
 
    /* Waiting for a filled buffer.*/
    msg_t msg = chMBFetch(&filled_buffers, (msg_t *)&pbuf, TIME_INFINITE);
    if (msg != MSG_OK) {
      Serial.println("fetch filled failed");
      chThdSleep(TIME_INFINITE);
    }  
    Serial.println((char*)pbuf);    
    
    (void)chMBPost(&free_buffers, (msg_t)pbuf, TIME_INFINITE);
  }  
}
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  chMBObjectInit(&filled_buffers, filled_buffers_queue, NUM_BUFFERS);
  chMBObjectInit(&free_buffers, free_buffers_queue, NUM_BUFFERS);
  
  for (int i = 0; i < NUM_BUFFERS; i++) {
   (void)chMBPost(&free_buffers, (msg_t)&buffers[i], TIME_INFINITE);
  }
  chBegin();
}

void loop() {
}