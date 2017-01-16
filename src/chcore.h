#ifndef chcore_h
#define chcore_h
#ifdef __AVR__
#include "AVR/avr_chcore.h"
#else  // __AVR__
#error unknown cpu
#endif  // _AVR__
#endif  // chcore_h