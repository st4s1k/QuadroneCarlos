#ifndef _TIMER_HPP
#define _TIMER_HPP

#include "quadronecarlos.hpp"
#include <avr/interrupt.h>

void TIMER0_Init(void);

unsigned long millis(void);

#endif