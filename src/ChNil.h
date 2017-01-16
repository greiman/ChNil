/**
 * @file    ChNil.h
 * @brief   ChNil Arduino port.
 *
 * @defgroup ChNil Arduino port
 * @details ChNil Added functions for Arduino port.
 * @{
 */
#ifndef ChNil_h
#define ChNil_h
#include "Arduino.h"
#include "nil/ch.h"
#include "hal/hal.h"
#include "ChNilAdc.h"
#include "ChNilSerial.h"
#include "ChNilTimer1.h"
/**
 * @brief Returns true if current thread is the idle thread.
 */
#define chIsIdleThread() (nil.current == &nil.threads[chCfgNumThreads])
/** start the chNil. */
void chBegin();
/** Fill stacks with 0X55.*/
void chFillStacks();
/** Print size of all all stacks.
 * @param[in] pr Print stream for output.
 */
void chPrintStackSizes(Print* pr);
/** Print unused byte count for all stacks.
 * @param[in] pr Print stream for output.
 *
 * Stacks must be filled by chFillStacks().
 */
void chPrintUnusedStack(Print* pr);
size_t chUnusedStack(void* base, size_t size);
void dmpRam(Print* pr);
#endif  // ChNil_h
/** @} */
