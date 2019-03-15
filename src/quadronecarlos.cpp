#include "quadronecarlos.hpp"
#include <ctype.h>

/* User defined variables BEGIN */

int blink_delay = 1 * SEC;
unsigned long prevTime[2];

/* User defined variables END */

/* User defined function prototypes BEGIN */

bool isnum(char *str);

/* User defined function prototypes END */

void setup() {
    cli();

    /* User code BEGIN */

    BT_Serial.begin(9600);

    pinMode(PORTD, PIND4, OUTPUT);

    clear_bit(PORTD, PIND4);

    /* User code END */

    sei();
}

void loop() {
    /* User code BEGIN */

    unsigned long curTime = millis();

    char *ln = BT_Serial.readln();

    if (strlen(ln)) {
        BT_Serial.println(ln);
        if (isnum(ln)) {
            int val = atoi(ln);
            if (val >= 0) {
                blink_delay = val;
            }
        }
    }

    if (curTime - prevTime[0] >= blink_delay) {
        toggle_bit(PORTD, PIND4);
        prevTime[0] = curTime;
    }

    /* User code END */
}

/* User defined function implementations BEGIN */

bool isnum(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

/* User defined function implementations END */

int main(void) {
    TIMER0_Init();

    setup();

    while (true) {
        loop();
    }

    return 0;
}
