#include "quadronecarlos.hpp"

#include <ctype.h>
#include <util/delay.h>
#include "mpu6050.h"
#include "simple_timer.hpp"
#include "usart.hpp"

/* User defined variables BEGIN */

/* User defined variables END */

/* User defined function prototypes BEGIN */

bool isnum(char *str);
bool isprefix(const char *pre, const char *str);

/* User defined function prototypes END */

inline void setup() {
    cli();

    /* User code BEGIN */
    TIMER0_Init_PWM();
    TIMER1_Init_PWM();

    BT_Serial.begin(9600);
    // setupMPU();

    /* User code END */

    sei();
}

inline void loop() {
    /* User code BEGIN */

    char *ln;

    if (BT_Serial.available()) {
        ln = BT_Serial.readln();
    }

    if (strlen(ln)) {
        if (isprefix("PROP", ln)) {
            if (isnum(ln + 6)) {
                int val = atoi(ln + 6);  // cmd: PROPn_val
                if (val >= 0) {
                    BT_Serial.print("Value: ");
                    BT_Serial.println(val);
                    if (val <= 0xFF) {
                        switch (*(ln + 4)) {
                            case '1':
                                OCR0A = val;
                                BT_Serial.print("OCR0A: ");
                                BT_Serial.println(OCR0A);
                                break;
                            case '2':
                                OCR0B = val;
                                BT_Serial.print("OCR0B: ");
                                BT_Serial.println(OCR0B);
                                break;
                            case '3':
                                OCR1A = val;
                                BT_Serial.print("OCR1A: ");
                                BT_Serial.println(OCR1A);
                                break;
                            case '4':
                                OCR1B = val;
                                BT_Serial.print("OCR1B: ");
                                BT_Serial.println(OCR1B);
                                break;
                            default:
                                BT_Serial.print("Bad syntax!");
                        }
                    } else {
                        BT_Serial.print("Value not in range 0-255");
                    }
                } else {
                    BT_Serial.print("Negative values are forbidden");
                }
            } else {
                BT_Serial.print("Value not a number");
            }
        } else {
            BT_Serial.println(ln);
        }
    }

    // if (curTime - prevTime[1] >= 100) {
    //     recordAccelRegisters();
    //     recordGyroRegisters();

    //     BT_Serial.print("Gyro (deg)");
    //     BT_Serial.print(" X=");
    //     BT_Serial.print(get_rotX());
    //     BT_Serial.print(" Y=");
    //     BT_Serial.print(get_rotY());
    //     BT_Serial.print(" Z=");
    //     BT_Serial.print(get_rotZ());
    //     BT_Serial.print(" Accel (g)");
    //     BT_Serial.print(" X=");
    //     BT_Serial.print(get_gForceX());
    //     BT_Serial.print(" Y=");
    //     BT_Serial.print(get_gForceY());
    //     BT_Serial.print(" Z=");
    //     BT_Serial.println(get_gForceZ());
    // }

    /* User code END */
}

/* User defined function implementations BEGIN */

inline bool isnum(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

inline bool isprefix(const char *pre, const char *str) {
    size_t lenpre = strlen(pre);
    size_t lenstr = strlen(str);

    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

/* User defined function implementations END */

int main(void) {
    // int blink_period = 1 * SEC;
    // unsigned long prevTime[2];

    setup();

    /* RED LED SETTINGS */
    // set_bit(DDRD, DDD4);
    // set PD4 as ouptut (RED LED)
    // clear_bit(PORTD, PIND4);
    // initialize PD4 with LOW

    while (true) {
        loop();

        // int curTime = millis();

        // if (curTime - prevTime[0] >= blink_period / 2) {
        //     toggle_bit(PORTD, PIND4);
        //     prevTime[0] = curTime;
        // } else if (curTime == 0) {
        //     BT_Serial.println("Timer ZERO.");
        // }
    }

    return 0;
}
