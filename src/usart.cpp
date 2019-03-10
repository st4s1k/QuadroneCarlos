#include "usart.hpp"

USART::USART(volatile uint8_t *ucsra,
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
			 uint8_t udre) : _ucsra(ucsra),
							 _ucsrb(ucsrb),
							 _ucsrc(ucsrc),
							 _ubrrl(ubrrl),
							 _udr(udr),
							 _u2x(u2x),
							 _ucsz0(ucsz0),
							 _ucsz1(ucsz1),
							 _rxen(rxen),
							 _txen(txen),
							 _rxcie(rxcie),
							 _rxc(rxc),
							 _udre(udre) {}

void USART::begin(int baud)
{
	*_ucsra |= (1 << _u2x); // dublam viteza de transmitere USART

	*_ubrrl = (F_CPU / (8L * baud)) - 1; // Asynchronous x2 speed mode

	*_ucsrc |= (1 << _ucsz1 | 1 << _ucsz0); // cofig URSEL, ASINCORN, PARITY DISABLE, STOPBIT 1, 8 bit data,
	*_ucsrb |= (1 << _rxen) | (1 << _txen) | (1 << _rxcie);
}

char USART::read(void)
{
	return (char)*_udr;
}

void USART::write(char data)
{
	*_udr = (uint8_t)data;

	if (data == '\n')
	{
		write('\r');
	}
}

void USART::print(const char str[])
{
	for (int i = 0; i < strlen(str); i++)
	{
		while (!availableForWrite()) ;
		write(str[i]);
	}
}

void USART::print(int num)
{
	char buffer[256];
	itoa(num, buffer, 10);
	print(buffer);
}

void USART::println(const char str[])
{
	print(str);
	while(!availableForWrite()) ;
	write('\n');
}

void USART::println(int num)
{
	print(num);
	while(!availableForWrite()) ;
	write('\n');
}

bool USART::available(void)
{
	return *_ucsra & (1 << _rxc);
}

bool USART::availableForWrite(void)
{
	return *_ucsra & (1 << _udre);
}
