#include "usart.hpp"

RingBuff_t Rx_Buffer0;
RingBuff_t Rx_Buffer1;

USART BT_Serial(&Rx_Buffer0, &UCSR0A, &UCSR0B, &UCSR0C, &UBRR0L, &UDR0, U2X0,
                UCSZ00, UCSZ01, RXEN0, TXEN0, RXCIE0, RXC0, UDRE0);

/*
USART Serial1(&Rx_Buffer1, &UCSR1A, &UCSR1B, &UCSR1C, &UBRR1L, &UDR1,
              U2X1, UCSZ10, UCSZ11, RXEN1, TXEN1, RXCIE1, RXC1, UDRE1);
*/

ISR(USART0_RX_vect) {
    cli();
    if (!RingBuffer_IsFull(&Rx_Buffer0)) {
        RingBuffer_Insert(&Rx_Buffer0, UDR0);
    } else {
        uint8_t dummy_data = UDR0;
    }
    sei();
}

USART::USART(RingBuff_t *rx_buffer, volatile uint8_t *ucsra,
             volatile uint8_t *ucsrb, volatile uint8_t *ucsrc,
             volatile uint8_t *ubrrl, volatile uint8_t *udr, uint8_t u2x,
             uint8_t ucsz0, uint8_t ucsz1, uint8_t rxen, uint8_t txen,
             uint8_t rxcie, uint8_t rxc, uint8_t udre)
    : _rx_buffer(rx_buffer),
      _ucsra(ucsra),
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

void USART::begin(long baud) {
    cli();

    *_ucsra |= (1 << _u2x);  // dublam viteza de transmitere USART

    *_ubrrl = (F_CPU / (8L * baud)) - 1;  // Asynchronous x2 speed mode

    *_ucsrc |= (1 << _ucsz1 | 1 << _ucsz0);  // cofig URSEL, ASINCORN, PARITY
                                             // DISABLE, STOPBIT 1, 8 bit data,
    *_ucsrb |= (1 << _rxen) | (1 << _txen) | (1 << _rxcie);

    RingBuffer_InitBuffer(_rx_buffer);

    sei();
}

char USART::read(void) {
    if (RingBuffer_IsEmpty(_rx_buffer)) {
        println("Buffer is empty!");
        return -1;
    }
    // println("Buffer is not empty!");
    return RingBuffer_Remove(_rx_buffer);
}

char *USART::readln(void) {
    RingBuff_Count_t itr = 0;

    while (itr < BUFFER_SIZE && !RingBuffer_IsEmpty(_rx_buffer)) {
        line[itr] = read();
        if ('\n' == (char)line[itr] || '\r' == (char)line[itr]) {
            if (itr == 0) {
                continue;
            } else {
                break;
            }
        }
        ++itr;
    }

    line[itr] = '\0';

    return (char *)line;
}

void USART::write(char data) {
    cli();
    while (!availableForWrite())
        ;

    *_udr = (uint8_t)data;

    if (data == '\n') {
        write('\r');
    }
    sei();
}

void USART::print(const char *str) {
    for (int i = 0; i < strlen(str); i++) {
        write(str[i]);
    }
}

void USART::print(int num) {
    char buffer[256];
    itoa(num, buffer, 10);
    print(buffer);
}

void USART::println(const char *str) {
    print(str);
    write('\n');
}

void USART::println(int num) {
    print(num);
    write('\n');
}

bool USART::available(void) { return *_ucsra & (1 << _rxc); }

bool USART::availableForWrite(void) { return *_ucsra & (1 << _udre); }
