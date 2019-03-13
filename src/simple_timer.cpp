#include "simple_timer.hpp"

volatile unsigned long ms = 0;

ISR(TIMER0_COMPA_vect)
{
	cli();
	++ms;
	sei();
}

void TIMER0_Init(void)
{
    cli();

	/*
		In Clear Timer on Compare or CTC mode (WGM0[2:0]=0x2), the OCR0A Register
		is used to manipulatethe counter resolution: the counter is cleared to ZERO
		when the counter value (TCNT0) matches theOCR0A.
		
		The OCR0A defines the top value for the counter, hence also its resolution.
		This mode allowsgreater control of the compare match output frequency.
		It also simplifies the counting of external events.

		The TOP value can be assigned to be the fixed value MAX or the value stored in the OCR0A Register.
	*/

	OCR0A = 250;

	/*
		COM0A1	COM0A0	Description
		0		0		Normal port operation, OC0A disconnected.
		0		1		Toggle OC0A on Compare Match.
		1		0		Clear OC0A on Compare Match.
		1		1		Set OC0A on Compare Match.
	*/

	set_bit(TCCR0A, COM0A1);
	clear_bit(TCCR0A, COM0A0);

	/*
		+-----------+-----------+-----------+-----------+-----------------------------------+-----------+-----------------------+-------------------+
		|	Mode	|	WGM02	|	WGM01	|	WGM00	|	Timer/Counter Mode of Operation	|	TOP		|	Update of OCR0x at	|	TOV Flag Set on	|
		+-----------+-----------+-----------+-----------+-----------------------------------+-----------+-----------------------+-------------------+
		|	2		|	0		|	1		|	0		|	CTC								|	OCRA	|	Immediate			|	MAX				|
		+-----------+-----------+-----------+-----------+-----------------------------------+-----------+-----------------------+-------------------+
	*/

	clear_bit(TCCR0A, WGM02);
	set_bit(TCCR0A, WGM01);
	clear_bit(TCCR0A, WGM00);

	/*
		Timer/Counter Clock Sources
		
		The TC can be clocked by an internal or an external clock source.
		The clock source is selected by writing to the Clock Select (CS0[2:0])
		bits in the Timer/Counter Control Register (TCCR0B).

		Bits 2:0 – CS0[2:0]: Clock Select 0 [n = 0..2]
		The three Clock Select bits select the clock source to be used by the Timer/Counter.
		
		Clock Select Bit Description
		
		CS02	CS01	CS00	Description
		0		0		0		No clock source (Timer/Counter stopped).
		0		0		1		clkI/O/1 (No prescaling)
		0		1		0		clkI/O/8 (From prescaler)

		0		1		1		clkI/O/64 (From prescaler)	<--- THIS	presc. = 64, 64 / 16 000 000 = 0.004 ms * OCR0A (250) = 1 ms !!!

		1		0		0		clkI/O/256 (From prescaler)
		1		0		1		clkI/O/1024 (From prescaler)
		1		1		0		External clock source on T0 pin. Clock on falling edge.
		1		1		1		External clock source on T0 pin. Clock on rising edge.
		
		If external pin modes are used for the Timer/Counter0, transitions on
		the T0 pin will clock the countereven if the pin is configured as an output.
		This feature allows software control of the counting.
	*/

	clear_bit(TCCR0B, CS02);
	set_bit(TCCR0B, CS01);
	set_bit(TCCR0B, CS00);

	/*
		TC0 Interrupt Mask Register (TIMSK0)

		Bit 2 – OCIE0B: Timer/Counter0, Output Compare B Match Interrupt Enable
		When the OCIE0B bit is written to one, and the I-bit in the Status Register is set, the Timer/Counter
		Compare Match B interrupt is enabled. The corresponding interrupt is executed if a Compare Match in
		Timer/Counter occurs, i.e., when the OCF0B bit is set in TIFR0.
		
		Bit 1 – OCIE0A: Timer/Counter0, Output Compare A Match Interrupt Enable
		When the OCIE0A bit is written to one, and the I-bit in the Status Register is set, the Timer/Counter0
		Compare Match A interrupt is enabled. The corresponding interrupt is executed if a Compare Match in
		Timer/Counter0 occurs, i.e., when the OCF0A bit is set in TIFR0.
		
		Bit 0 – TOIE0: Timer/Counter0, Overflow Interrupt Enable
		When the TOIE0 bit is written to one, and the I-bit in the Status Register is set, the Timer/Counter0
		Overflow interrupt is enabled. The corresponding interrupt is executed if an overflow in Timer/Counter0
		occurs, i.e., when the TOV0 bit is set in TIFR0.
	*/

	// set_bit(TIMSK0, OCIE0B);
	set_bit(TIMSK0, OCIE0A);
	// set_bit(TIMSK0, TOIE0);

	sei();
}

unsigned long millis()
{
	return ms;
}