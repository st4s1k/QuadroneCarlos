#ifndef _USART_HPP
#define _USART_HPP

#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "LightweightRingBuff.h"
#include "quadronecarlos.hpp"

class USART
{
  private:
	RingBuff_t *const _rx_buffer;
	RingBuff_Data_t line[BUFFER_SIZE];

	volatile uint8_t *const _ucsra;
	volatile uint8_t *const _ucsrb;
	volatile uint8_t *const _ucsrc;
	volatile uint8_t *const _ubrrl;
	volatile uint8_t *const _udr;

	uint8_t const _u2x;
	uint8_t const _ucsz0;
	uint8_t const _ucsz1;
	uint8_t const _rxen;
	uint8_t const _txen;
	uint8_t const _rxcie;
	uint8_t const _rxc;
	uint8_t const _udre;

  public:
	USART(RingBuff_t *rx_buffer,
		  volatile uint8_t *ucsra,
		  volatile uint8_t *ucsrb,
		  volatile uint8_t *ucsrc,
		  volatile uint8_t *ubrrl,
		  volatile uint8_t *udr,
		  uint8_t u2x,
		  uint8_t ucsz0,
		  uint8_t ucsz1,
		  uint8_t rxen,
		  uint8_t txen,
		  uint8_t rxcie,
		  uint8_t rxc,
		  uint8_t udre);

	void begin(long baud);
	char read(void);
	char *readln(void);
	void write(char data);
	void print(const char *str);
	void print(int num);
	void println(const char *str);
	void println(int num);
	bool available(void);
	bool availableForWrite(void);
};

extern RingBuff_t Rx_Buffer0;

extern RingBuff_t Rx_Buffer1;

extern USART BT_Serial;

extern USART Serial1;

#endif
