/* Arduino NilRTOS Library
 * Copyright (C) 2013 by William Greiman
 *
 * This file is part of the Arduino NilRTOS Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino NilRTOS Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
/**
 * @file    ChNilTimer1.h
 * @brief   Nil RTOS timer 1 source
 *
 * @defgroup Timer1 ChNilTimer1
 * @details Nil RTOS timer 1 functions.
 * @{
 */
#ifndef ChNilTimer1_h
#define ChNilTimer1_h
#include "ChNil.h"

#define CH_NIL_TIMER1_IRQ            \
SEMAPHORE_DECL(timerSem, 0);         \
CH_IRQ_HANDLER(TIMER1_COMPB_vect) {  \
  CH_IRQ_PROLOGUE();                 \
  chSysLockFromISR();                \
  chSemSignalI(&timerSem);           \
  chSysUnlockFromISR();              \
  CH_IRQ_EPILOGUE();                 \
}
extern semaphore_t timerSem;
void chTimer1Start(uint32_t microseconds);
void chTimer1Stop();
bool chTimer1Wait();
#endif // ChNilTimer1
/** @} */