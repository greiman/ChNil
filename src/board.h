#ifndef board_h
#define board_h
#ifdef __AVR__
#include "AVR/avr_board.h"
#else  // __AVR__
#error unknown cpu
#endif  // _AVR__
#endif  // board_h