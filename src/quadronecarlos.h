#define MHz 1000000L
#define F_CPU 16*MHz

#include <avr/io.h>
#include <util/delay.h>

#define bit(pin)                 (1 << (pin)) 
#define set_bits(port,pins)      ((port) |= (pins)) 
#define clear_bits(port,pins)    ((port) &= (~(pins))) 
#define set_bit(port,pin)        set_bits((port), (bit((pin))))
#define clear_bit(port,pin)      clear_bits((port), (bit((pin)))) 
// #define bit_is_set(port,pin)        ((port) & (bit(pin))) 
// #define bit_is_clear(port,pin)      !bit_is_set((port), (pin)) 
#define bits_are_set(port,pins)      (((port) & (pins)) == (pins)) 
#define bits_are_clear(port,pin)     (((port) & (pin)) == 0) 
#define bit_is_val(port,pin)        (((port)>>(pin)) & 1) 
