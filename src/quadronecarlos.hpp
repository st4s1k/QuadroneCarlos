#ifndef _QUADRONE_CARLOS_HPP
#define _QUADRONE_CARLOS_HPP

#define Hz 1L
#define MHz 1000000 * Hz
#define F_CPU 16 * MHz

#include <avr/io.h>
#include <avr/iom328pb.h>

#include <util/delay.h>

#include "usart.hpp"

#define HIGH 1
#define LOW !HIGH
#define OUTPUT HIGH
#define INPUT !OUTPUT

#define bit(pin) (1 << (pin))

#define set_bits(port, pins) ((port) |= (pins))

#define clear_bits(port, pins) ((port) &= (~pins))

#define set_bit(port, pin) (set_bits((port), bit((pin))))

#define clear_bit(port, pin) (clear_bits((port), bit((pin))))

#define bits_are_set(port, pins) (((port) & (pins)) == (pins))

#define bits_are_clear(port, pin) (((port) & (pin) == LOW))

#define bit_is_val(port, pin) (((port) >> (pin)) & 1)

#define pinMode(port, pin, mode) ((mode) == OUTPUT ? set_bit((port), (pin)) : clear_bit((port), (pin)))

#define portMode(port, mode) ((mode) == OUTPUT ? set_bits((port), 0xFF) : clear_bits((port), 0xFF))

// Serial 0

RingBuff_t	Buffer0;

USART BT_Serial(&UCSR0A, &UCSR0B, &UCSR0C, &UBRR0L, &UDR0,
                U2X0, UCSZ00, UCSZ01, RXEN0, TXEN0, RXCIE0, RXC0, UDRE0);

// Serial 1

RingBuff_t	Buffer1;

USART Serial1(&UCSR1A, &UCSR1B, &UCSR1C, &UBRR1L, &UDR1,
                 U2X1, UCSZ10, UCSZ11, RXEN1, TXEN1, RXCIE1, RXC1, UDRE1);

#endif