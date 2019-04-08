#ifndef _QUADRONE_CARLOS_HPP
#define _QUADRONE_CARLOS_HPP

#define Hz 1UL
#define MHz 1000000 * Hz
#define F_CPU 8 * MHz

#include <avr/io.h>
#include <avr/iom328pb.h>

#define HIGH 1
#define LOW !HIGH
#define OUTPUT HIGH
#define INPUT !OUTPUT

#define MS 1UL
#define SEC 1000 * MS
#define MIN 60 * SEC
#define HR 60 * MIN

#define set_bits(port, pins) ((port) |= (pins))

#define clear_bits(port, pins) ((port) &= (~pins))

#define set_bit(port, pin) (set_bits((port), _BV((pin))))

#define clear_bit(port, pin) (clear_bits((port), _BV((pin))))

#define toggle_bit(port, pin) ((port) ^= _BV((pin)))

#define bits_are_set(port, pins) (((port) & (pins)) == (pins))

#define bits_are_clear(port, pin) (((port) & (pin) == LOW))

#define bit_is_val(port, pin) (((port) >> (pin)) & 1)

#endif