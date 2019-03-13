#include "quadronecarlos.hpp"

/* User variables BEGIN */

unsigned long prevTime = 0;

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

    unsigned long curTime = millis();

    if (BT_Serial.available())
    {
        BT_Serial.write(BT_Serial.read());
	
        blink_delay = 200;
    }
    else
    {
        BT_Serial.print(prevTime);
        BT_Serial.println(curTime);

        // BT_Serial.print("i'm here! rand = ");
        // BT_Serial.println(strlen("i'm here!"));
        // BT_Serial.println((int) rand() % 10);

        blink_delay = 1000;
    }

    if (curTime - prevTime >= blink_delay) {
        toggle_bit(PORTD, PIND4);
        prevTime = curTime;
    }    

    /* User code END */
}

int main(void)
{
    // this combination is for the standard 168/328/1280/2560
    set_bit(TCCR0B, CS01);
    // enable timer 0 overflow interrupt
    set_bit(TCCR0B, CS00);
    set_bit(TIMSK0, TOIE0);

    setup();

    while (true)
    {
        loop();
    }

    return 0;
}
