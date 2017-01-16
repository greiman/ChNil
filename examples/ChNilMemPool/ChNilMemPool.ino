// Demo of memory pool and mailbox
#include "ChNil.h"

// Use tiny unbuffered ChNilSerial library.
// Macro to redefine Serial as ChNilSerial.
// Remove definition to use standard Arduino Serial.
ChNilSerialClass ChNilSerial;
#define Serial ChNilSerial

#define NUM_BUFFERS 4
#define BUFFERS_SIZE 32
 
static char buffers[NUM_BUFFERS][BUFFERS_SIZE];
static MEMORYPOOL_DECL(mem_pool, sizeof (buffers[0]), NULL);

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
 
    /* Get a free buffer */
    pbuf = chPoolAlloc(&mem_pool);
    if (!pbuf) {
       Serial.println("chPoolAlloc failed");
       chThdSleep(TIME_INFINITE);
    }
    sprintf((char*)pbuf, "%ld", micros());

    // Send buffer to print thread.
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
    void *pbuf = 0;
 
    /* Waiting for a filled buffer.*/
    msg_t msg = chMBFetch(&filled_buffers, (msg_t *)&pbuf, TIME_INFINITE);
    if (msg != MSG_OK) {
      Serial.println("chMBFetch failed");
      chThdSleep(TIME_INFINITE);      
    }
    Serial.println((char*)pbuf);    
    
    chPoolFree(&mem_pool, pbuf);
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
  chPoolObjectInit(&mem_pool, sizeof(buffers[0]), NULL);
  chPoolLoadArray(&mem_pool, buffers, NUM_BUFFERS);  

  chBegin();
}

void loop() {
}