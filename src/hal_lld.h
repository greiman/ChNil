#ifndef hal_lld_h
#define hal_lld_h
#ifdef __AVR__
#include "AVR/avr_hal_lld.h"
#else  // __AVR__
#error unknown cpu
#endif  // _AVR__
#endif  // hal_lld_h