#include "quadronecarlos.hpp"

/* User variables BEGIN */

USART BT_Serial(&UCSR0A, &UCSR0B, &UCSR0C, &UBRR0L, &UDR0,
                U2X0, UCSZ00, UCSZ01, RXEN0, TXEN0, RXCIE0, RXC0, UDRE0);

// USART Serial1(&UCSR1A, &UCSR1B, &UCSR1C, &UBRR1L, &UDR1,
//                  U2X1, UCSZ10, UCSZ11, RXEN1, TXEN1, RXCIE1, RXC1, UDRE1);

bool blink = true;

/* User variables END */

void setup()
{
    /* User code BEGIN */

    BT_Serial.begin(9600);

    pinMode(PORTD, PIND4, OUTPUT);

    clear_bit(PORTD, PIND4);

    /* User code END */
}

void loop()
{
    /* User code BEGIN */

    int blink_delay = 5000;

    blink = true;

    if (BT_Serial.available() && BT_Serial.availableForWrite())
    {
        BT_Serial.write(BT_Serial.read());
        blink_delay = 200;
    }
    else if (BT_Serial.available())
    {
        blink_delay = 500;
    }
    else if (BT_Serial.availableForWrite())
    {
	//BT_Serial.print("i'm here!\n");
	BT_Serial.write('i');
	BT_Serial.write('\'');
	BT_Serial.write('m');
	BT_Serial.write(' ');
	BT_Serial.write('h');
	BT_Serial.write('e');
	BT_Serial.write('r');
	BT_Serial.write('e');
	BT_Serial.write('!');
	BT_Serial.write('\n');
	blink_delay = 1000;
    }
    else
    {
        blink = false;
    }

    if (blink)
    {
        set_bit(PORTD, PIND4);
        switch (blink_delay)
        {
        case 200:
            _delay_ms(200);
            break;
        case 500:
            _delay_ms(500);
        case 1000:
            _delay_ms(1000);
            break;
        }
        clear_bit(PORTD, PIND4);
        switch (blink_delay)
        {
        case 200:
            _delay_ms(200);
            break;
        case 500:
            _delay_ms(500);
        case 1000:
            _delay_ms(1000);
            break;
        }
    }

    /* User code END */
}

int main(void)
{
    setup();

    while (true)
    {
        loop();
    }

    return 0;
}
