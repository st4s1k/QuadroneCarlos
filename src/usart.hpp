#ifndef _USART_HPP
#define _USART_HPP

#include "quadronecarlos.hpp"
#include <string.h>
#include <stdlib.h>

class USART
{
  private:
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
	USART(volatile uint8_t *ucsra,
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

	void begin(int baud);
	char read(void);
	void write(char data);
	void print(const char str[]);
	void print(int num);
	void println(const char str[]);
	void println(int num);
	bool available(void);
	bool availableForWrite(void);
};

#endif
