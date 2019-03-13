#include "quadronecarlos.hpp"

/* User variables BEGIN */

unsigned long prevTime = 0;

/* User variables END */

void setup()
{
    cli();

    /* User code BEGIN */

    BT_Serial.begin(9600);

    pinMode(PORTD, PIND4, OUTPUT);

    clear_bit(PORTD, PIND4);

    /* User code END */

    sei();
}

void loop()
{
    /* User code BEGIN */

    int blink_delay;

    unsigned long curTime = millis();

    char *ln = BT_Serial.readln();

    if (strlen(ln))
    {
        BT_Serial.println(ln);
    }

    // if (RingBuffer_IsEmpty(&Rx_Buffer0))
    // {
    //     BT_Serial.println("Rx_Buffer0 is empty.");
    // }
    // else
    // {
    //     for (int i = 0; i < BUFFER_SIZE; i++)
    //     {
    //         BT_Serial.write((char) Rx_Buffer0.Buffer[i]);
    //     }
    //     BT_Serial.write('\n');
    // }

    if (BT_Serial.available())
    {
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            BT_Serial.write(Rx_Buffer0.Buffer[i]);
        }

        blink_delay = 200;
    }
    else
    {
        blink_delay = 1000;
        // BT_Serial.print("i'm here! rand = ");
        // BT_Serial.println(strlen("i'm here!"));
        // BT_Serial.println((int) rand() % 10);
    }

    BT_Serial.print(prevTime);
    BT_Serial.write('\t');
    BT_Serial.println(curTime);

    if (curTime - prevTime >= blink_delay)
    {
        toggle_bit(PORTD, PIND4);
        prevTime = curTime;
    }

    /* User code END */
}

int main(void)
{
    TIMER0_Init();

    setup();

    while (true)
    {
        loop();
    }

    return 0;
}
