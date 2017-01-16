#ifndef chcore_timer_h
#define chcore_timer_h
#ifdef __AVR__
#include "AVR/avr_chcore_timer.h"
#else  // __AVR__
#error unknown cpu
#endif  // _AVR__
#endif  // chcore_timer_h