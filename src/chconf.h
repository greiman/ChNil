#ifndef chconf_h
#define chconf_h
#ifdef __AVR__
#include "AVR/avr_chconf.h"
#else  // __AVR__
#error unknown cpu
#endif  // _AVR__
#endif  // chconf_h


