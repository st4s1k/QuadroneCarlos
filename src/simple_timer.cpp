#include "simple_timer.hpp"

#include <avr/interrupt.h>

volatile unsigned long ms = 0;

// ISR(TIMER0_COMPA_vect) { ++ms; }

void TIMER0_Init(void) {
    /* SET INITIAL VALUES */
    OCR0A = F_CPU / 64000;

    /* SET WAVEGENERATOR MODE */
    clear_bit(TCCR0A, WGM02);
    set_bit(TCCR0A, WGM01);
    clear_bit(TCCR0A, WGM00);

    /* SET PRESCALER */
    clear_bit(TCCR0B, CS02);
    set_bit(TCCR0B, CS01);
    set_bit(TCCR0B, CS00);

    /* SET INTERRUPT MODE */
    set_bit(TCCR0A, COM0A1);
    clear_bit(TCCR0A, COM0A0);

    /* ENABLE INTERRUPT */
    set_bit(TIMSK0, OCIE0A);
}

void TIMER0_Init_PWM(void) {
    /* SET INITIAL VALUES */
    OCR0A = 0;
    OCR0B = 0;

    /* SET PINS */
    set_bit(DDRD, DDD6);
    // set PD6 as output (OC0A) [M1]
    set_bit(DDRD, DDD5);
    // set PD5 as output (OC0B) [M2]

    /* SET WAVEGENERATOR MODE */
    clear_bit(TCCR0A, WGM02);
    set_bit(TCCR0A, WGM01);
    set_bit(TCCR0A, WGM00);
    // set fast PWM mode (TOP = 0xFF)

    /* SET INTERRUPT MODE */
    set_bit(TCCR0A, COM0A1);
    clear_bit(TCCR0A, COM0A0);
    // enable OC0A none-inverting mode
    set_bit(TCCR0A, COM0B1);
    clear_bit(TCCR0A, COM0B0);
    // enable OC0B none-inverting mode

    /* SET PRESCALER */
    clear_bit(TCCR0B, CS02);
    set_bit(TCCR0B, CS01);
    clear_bit(TCCR0B, CS00);
    // set prescaler to 8 and starts PWM
}

void TIMER1_Init_PWM(void) {
    /* SET INITIAL VALUES */
    OCR1A = 0;
    OCR1B = 0;

    /* SET PINS */
    set_bit(DDRB, DDB1);
    // set PB1 as output (OC1A) [M3]
    set_bit(DDRB, DDB2);
    // set PB2 as output (OC1B) [M4]

    /* SET WAVEGENERATOR MODE */
    clear_bit(TCCR1B, WGM13);
    set_bit(TCCR1B, WGM12);
    clear_bit(TCCR1A, WGM11);
    set_bit(TCCR1A, WGM10);
    // set fast PWM 8bit mode (TOP = 0xFF)

    /* SET INTERRUPT MODE */
    set_bit(TCCR1A, COM1A1);
    clear_bit(TCCR1A, COM1A0);
    // enable OC1A none-inverting mode
    set_bit(TCCR1A, COM1B1);
    clear_bit(TCCR1A, COM1B0);
    // enable OC1B none-inverting mode

    /* SET PRESCALER */
    clear_bit(TCCR1B, CS12);
    set_bit(TCCR1B, CS11);
    clear_bit(TCCR1B, CS10);
    // set prescaler to 8 and starts PWM
}

unsigned long millis() { return ms; }