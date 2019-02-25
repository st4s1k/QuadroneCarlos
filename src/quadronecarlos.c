#include "quadronecarlos.h"

int main(void)
{
    float freq = 0.5;
    int some_pin = 2;

    set_bits(DDRC, 0xFF);

    clear_bits(PORTC, PINC0 | PINC1 | PINC2);

    while (1)
    {
        clear_bit(PORTC, some_pin);
        _delay_ms((int)(1000.0/freq));
        set_bit(PORTC, some_pin);
        _delay_ms((int)(1000.0/freq));
    }

    return 0;
}