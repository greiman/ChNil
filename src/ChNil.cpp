/**
 * @file    ChNil.cpp
 * @brief   ChNil Arduino port.
 *
 * @defgroup ChNil Arduino port
 * @details ChNil Added functions for Arduino port.
 * @{
 */
#include "Arduino.h"
#include "ChNil.h"

/** boundary between stack and heap. */
extern char* __brkval;
/** End of bss section.*/
extern char __bss_end;
//-----------------------------------------------------------------------------
/** @return Address of first byte after the heap. */
static inline
char* heapEnd() {
 return __brkval ? __brkval : &__bss_end;
}
//-----------------------------------------------------------------------------
/** @return Size of heap and idle thread stack in bytes. */
static inline 
size_t heapSize() {
  return (char*)RAMEND - __malloc_heap_start + 1;
}
//-----------------------------------------------------------------------------
static __attribute__ ((noinline)) 
void fill8(uint8_t* bgn, uint8_t* end) {
  while (bgn < end) *bgn++ = 0X55;
}
//-----------------------------------------------------------------------------
static __attribute__ ((noinline)) 
size_t unusedStack(uint8_t* bgn, uint8_t* end) {
  uint8_t* p = bgn;
  while (p < end && *p == 0X55) p++;
  return p - bgn;
}
//----------------------------------------------------------------------------
void chBegin() {
  chSysLock();
  halInit();
  chSysInit();  
}
//-----------------------------------------------------------------------------
void chFillStacks() {
  uint8_t* sp;
  chSysLock();
  const thread_config_t *tcp = nil_thd_configs;

  for (size_t i = 0; i < chCfgNumThreads; i++) {
    fill8(tcp[i].wbase, tcp[i].wend);
  }
  sp = (uint8_t*)SP;
  fill8((uint8_t*)heapEnd(), sp - 32);
  chSysUnlock();
}
//-----------------------------------------------------------------------------
void chPrintStackSizes(Print* pr) {
  const thread_config_t *tcp = nil_thd_configs;
  pr->print(F("Stack Sizes:  "));
  for (size_t i = 0; i < chCfgNumThreads; i++) {
    size_t size = (tcp[i].wend - tcp[i].wbase);
    pr->print(size);
    pr->print(' ');
  }
  pr->println(heapSize());
}
//-----------------------------------------------------------------------------
void chPrintUnusedStack(Print* pr) {
  const thread_config_t *tcp = nil_thd_configs;
  pr->print(F("Unused Stack: "));
  for (size_t i = 0; i < chCfgNumThreads; i++) {
    pr->print(unusedStack((uint8_t*)tcp[i].wbase, (uint8_t*)tcp[i].wend));
    pr->print(' ');
  }
  pr->println(unusedStack((uint8_t*)heapEnd(), (uint8_t*)RAMEND));
}
//-----------------------------------------------------------------------------
void dmpRam(Print* pr) {
  for (uint8_t* ptr = (uint8_t*)256; ptr <= (uint8_t*)RAMEND; ptr++) {
    if (((int)ptr)%16 == 0) {
      pr->println();
      pr->print((int)ptr, HEX);
      pr->write(' ');
    }
    pr->write(' ');
    if (*ptr < 16) pr->write('0');
    pr->print(*ptr, HEX);
  }
  pr->println();
}
//------------------------------------------------------------------------------
size_t chUnusedStack(void* base, size_t size) {
  return unusedStack((uint8_t*)base, (uint8_t*)base + size);
}
/** @} */