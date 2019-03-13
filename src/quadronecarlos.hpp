#ifndef _QUADRONE_CARLOS_HPP
#define _QUADRONE_CARLOS_HPP

#define Hz 1L
#define MHz 1000000 * Hz
#define F_CPU 16 * MHz

#include <avr/io.h>
#include <avr/iom328pb.h>

#include <util/delay.h>

#include "timer.hpp"
#include "usart.hpp"

#define HIGH 1
#define LOW !HIGH
#define OUTPUT HIGH
#define INPUT !OUTPUT

#define set_bits(port, pins) ((port) |= (pins))

#define clear_bits(port, pins) ((port) &= (~pins))

#define set_bit(port, pin) (set_bits((port), _BV((pin))))

#define clear_bit(port, pin) (clear_bits((port), _BV((pin))))

#define toggle_bit(port, pin) ((port) ^= _BV((pin)))

#define bits_are_set(port, pins) (((port) & (pins)) == (pins))

#define bits_are_clear(port, pin) (((port) & (pin) == LOW))

#define bit_is_val(port, pin) (((port) >> (pin)) & 1)

#define pinMode(port, pin, mode) ((mode) == OUTPUT ? set_bit((port), (pin)) : clear_bit((port), (pin)))

#define portMode(port, mode) ((mode) == OUTPUT ? set_bits((port), 0xFF) : clear_bits((port), 0xFF))

#endif