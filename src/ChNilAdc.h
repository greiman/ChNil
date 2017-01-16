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
 * @file    ChNilAdc.h
 * @brief   ChNilAdc read ADC
 *
 * @defgroup ChNilAdc Read AVR ADC
 * @details ChNilAdc functions.
 * @{
 */
#ifndef ChNilAdc_h
#define ChNilAdc_h
//-----------------------------------------------------------------------------
/** ADC prescaler 2 */
const uint8_t ADC_PS_2   = (1 << ADPS0);
/** ADC prescaler 4 */
const uint8_t ADC_PS_4   = (1 << ADPS1);
/** ADC prescaler 8 */
const uint8_t ADC_PS_8   = (1 << ADPS1) | (1 << ADPS0);
/** ADC prescaler 16 */
const uint8_t ADC_PS_16  = (1 << ADPS2);
/** ADC prescaler 32 */
const uint8_t ADC_PS_32  = (1 << ADPS2) | (1 << ADPS0);
/** ADC prescaler 64 */
const uint8_t ADC_PS_64  = (1 << ADPS2) | (1 << ADPS1);
/** ADC prescaler 128 */
const uint8_t ADC_PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
/** Prescaler bit field */
const uint8_t ADC_PS_BITS = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
//-----------------------------------------------------------------------------
#define CH_NIL_ADC_IRQ        \
SEMAPHORE_DECL(adcSem, 0);    \
CH_IRQ_HANDLER(ADC_vect) {    \
  CH_IRQ_PROLOGUE();          \
  chSysLockFromISR();         \
  chSemSignalI(&adcSem);      \
  chSysUnlockFromISR();       \
  CH_IRQ_EPILOGUE();          \
}
extern semaphore_t adcSem;

void chAnalogPrescalar(uint8_t ps);
int chAnalogRead(uint8_t pin);
void chAnalogReference(uint8_t mode);
#endif  // ChNilAdc_h
/** @} */