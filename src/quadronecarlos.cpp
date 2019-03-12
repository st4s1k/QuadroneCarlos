#include "quadronecarlos.hpp"
#include "usart.hpp"

/* User variables BEGIN */

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

    int blink_delay;

    if (BT_Serial.available())
    {
        BT_Serial.write(BT_Serial.read());
	
        blink_delay = 200;
    }
    else
    {
        BT_Serial.print("i'm here! rand = ");
        // BT_Serial.println(strlen("i'm here!"));
        BT_Serial.println((int) rand() % 10);

        blink_delay = 1000;
    }

    set_bit(PORTD, PIND4);
    switch (blink_delay)
    {
    case 200:
        _delay_ms(200);
        break;
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
    case 1000:
        _delay_ms(1000);
        break;
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
