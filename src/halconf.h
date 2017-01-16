#ifndef halconf_h
#define halconf_h
#ifdef __AVR__
#include "AVR/avr_halconf.h"
#else  // __AVR__
#error unknown cpu
#endif  // _AVR__
#endif  // halconf_h