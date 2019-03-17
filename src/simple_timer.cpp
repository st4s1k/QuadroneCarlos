#include "simple_timer.hpp"

#include <avr/interrupt.h>

volatile unsigned long ms = 0;

ISR(TIMER0_COMPA_vect) {
    ++ms;
}

void TIMER0_Init(void) {
    cli();

    OCR0A = 250;

    set_bit(TCCR0A, COM0A1);
    clear_bit(TCCR0A, COM0A0);

    clear_bit(TCCR0A, WGM02);
    set_bit(TCCR0A, WGM01);
    clear_bit(TCCR0A, WGM00);

    clear_bit(TCCR0B, CS02);
    set_bit(TCCR0B, CS01);
    set_bit(TCCR0B, CS00);

    set_bit(TIMSK0, OCIE0A);

    sei();
}

unsigned long millis() { return ms; }