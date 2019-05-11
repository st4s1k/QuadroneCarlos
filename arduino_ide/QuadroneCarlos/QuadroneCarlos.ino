#if defined(ARDUINO_AVR_UNO)
#   define M1 6   // Arduino UNO pin PD6
#   define M2 5   // Arduino UNO pin PD5
#   define M3 9   // Arduino UNO pin PB1
#   define M4 10  // Arduino UNO pin PB2
#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
#   define M1 PD6
#   define M2 PD5
#   define M3 PB1
#   define M4 PB2
#else
#   error "MCU not supported"
#endif

#if defined(M1) && defined(M2) && defined(M3) && defined(M4)

/* PROGRAM CODE BEGIN */




/////////////////////////////////////////////////////////////////////////////
// INCLUDES
/////////////////////////////////////////////////////////////////////////////

#include "MPU6050_tockn.h" // Modified

#include "PID.hpp"




/////////////////////////////////////////////////////////////////////////////
// DEFINES
/////////////////////////////////////////////////////////////////////////////

// Custom propeller configuration
#define P1 M3
#define P2 M1
#define P3 M2
#define P4 M4

#define PROP_CMD_THRESHOLD  5

#define ROLL_CMD_MIN       -255
#define ROLL_CMD_MAX        255
#define ROLL_MIN           -900 //  -90 deg
#define ROLL_MAX            900 //   90 deg

#define PITCH_CMD_MIN      -255
#define PITCH_CMD_MAX       255
#define PITCH_MIN          -900 //  -90 deg
#define PITCH_MAX           900 //   90 deg

#define YAW_CMD_STEP        15
#define YAW_CMD_MIN        -255
#define YAW_CMD_MAX         255
#define YAW_MIN            -3600 // -360 deg
#define YAW_MAX             3600 //  360 deg

#define THRUST_CMD_STEP     15
#define THRUST_CMD_MIN      0
#define THRUST_CMD_MAX      255
#define THRUST_MIN          0
#define THRUST_MAX          255

#define BUFF_SIZE 64
#define END_OF_LINE         '*'

#define RP_KP_MAX           5.0f
#define RP_KI_MAX           0.05f
#define RP_KD_MAX           10.0f
#define Y_KP_MAX            1.0f




/////////////////////////////////////////////////////////////////////////////
// DECLARATIONS
/////////////////////////////////////////////////////////////////////////////

MPU6050 mpu(Wire);

long    timer[3] = {0, 0, 0};

char    serial_buff[BUFF_SIZE];

float   roll_offset = 0.0f;
float   pitch_offset = 0.0f;
float   yaw_offset = 0.0f;

float   roll = 0.0f;
float   pitch = 0.0f;
float   yaw = 0.0f;
int     thrust = 0;

bool    MANUAL = false;
bool    PROPS_ON = false;

int     cmdThrust = 0;
int     cmdRoll = 0;
int     cmdPitch = 0;
int     cmdYaw = 0;

PID     roll_PID  (0.0f,  0.0f,   0.0f, ROLL_MIN,   ROLL_MAX,   -THRUST_MAX,  THRUST_MAX);
PID     pitch_PID (0.0f,  0.0f,   0.0f, PITCH_MIN,  PITCH_MAX,  -THRUST_MAX,  THRUST_MAX);
PID     yaw_PID   (0.0f,  0.0f,   0.0f, YAW_MIN,    YAW_MAX,    -THRUST_MAX,  THRUST_MAX);

bool    closed_system = true;




/////////////////////////////////////////////////////////////////////////////
// SETUP
/////////////////////////////////////////////////////////////////////////////

void setup(void) {
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(400000);

  mpu.begin();

  Serial.print("*T");
  mpu.calcOffsets();
  Serial.println("*");

  Serial.println("*GC*");

  playReadySignal();
}




/////////////////////////////////////////////////////////////////////////////
// MAIN LOOP
/////////////////////////////////////////////////////////////////////////////

void loop(void) {
  //  timer[1] = micros();
  mpu.update();

  roll  = mpu.getAngleX();
  pitch = mpu.getAngleY();
  yaw   = mpu.getAngleZ();

  if (millis() - timer[0] > 100) {

    //    Serial.print("\t");
    //    Serial.print(thrust);
    Serial.print(roll);
    Serial.print("\t");
    Serial.print(pitch);
    Serial.print("\t");
    Serial.println(yaw);

    timer[2] = millis();
  }

  remoteControl(false);

  stabilization();

  //  Serial.print(micros() - timer[1]);
  //  Serial.print("\t");
  //  Serial.println(1000000 / (micros() - timer[1]));
  //  delay(10);
}




/////////////////////////////////////////////////////////////////////////////
// STABILIZATION
/////////////////////////////////////////////////////////////////////////////

void stabilization(void) {

  // stabilization set-points (in degrees)
  int roll_sp     = map(cmdRoll,    ROLL_CMD_MIN,   ROLL_CMD_MAX,   ROLL_MIN / 18,   ROLL_MAX / 18);
  int pitch_sp    = map(cmdPitch,   PITCH_CMD_MIN,  PITCH_CMD_MAX,  PITCH_MIN / 18,  PITCH_MAX / 18);
  int yaw_sp      = map(cmdYaw,     YAW_CMD_MIN,    YAW_CMD_MAX,    YAW_MIN,    YAW_MAX);

  // Thrust command (THRUST_MIN .. THRUST_MAX)
  int thrust_cmd  = map(cmdThrust,  THRUST_CMD_MIN, THRUST_CMD_MAX, THRUST_MIN, THRUST_MAX);

  // control commands

  int roll_cmd    = roll_PID.evaluate  (closed_system ? roll  * 10 : 0,  roll_sp, -mpu.getGyroX());
  int pitch_cmd   = pitch_PID.evaluate (closed_system ? pitch * 10 : 0, pitch_sp, -mpu.getGyroY());
  int yaw_cmd     = yaw_PID.evaluate   (closed_system ? yaw   * 10 : 0,   yaw_sp, -mpu.getGyroZ());

  int max_cmd = max(max(abs(roll_cmd), abs(pitch_cmd)), abs(yaw_cmd));

  if (thrust_cmd + max_cmd > THRUST_MAX) {
    thrust_cmd = THRUST_MAX - max_cmd;
  }

  thrust_cmd = constrain(thrust_cmd, THRUST_MIN, THRUST_MAX);

  thrust = thrust_cmd;

  int u1 = thrust_cmd,
      u2 = thrust_cmd,
      u3 = thrust_cmd,
      u4 = thrust_cmd;

  if (roll_cmd) {
    if (roll_cmd > 0) {
      u3 += roll_cmd;
      u4 += roll_cmd;
    } else {
      u1 -= roll_cmd;
      u2 -= roll_cmd;
    }
  }

  if (pitch_cmd) {
    if (pitch_cmd > 0) {
      u2 += pitch_cmd ;
      u3 += pitch_cmd ;
    } else {
      u1 -= pitch_cmd ;
      u4 -= pitch_cmd ;
    }
  }

  if (yaw_cmd) {
    if (yaw_cmd > 0) {
      u2 += yaw_cmd ;
      u4 += yaw_cmd ;
    } else {
      u1 -= yaw_cmd ;
      u3 -= yaw_cmd ;
    }
  }

  u1 = constrain(u1, THRUST_MIN, THRUST_MAX);
  u2 = constrain(u2, THRUST_MIN, THRUST_MAX);
  u3 = constrain(u3, THRUST_MIN, THRUST_MAX);
  u4 = constrain(u4, THRUST_MIN, THRUST_MAX);

  if (PROPS_ON) {
    analogWrite(P1, u1 > PROP_CMD_THRESHOLD ? u1 : 0);
    analogWrite(P2, u2 > PROP_CMD_THRESHOLD ? u2 : 0);
    analogWrite(P3, u3 > PROP_CMD_THRESHOLD ? u3 : 0);
    analogWrite(P4, u4 > PROP_CMD_THRESHOLD ? u4 : 0);
  }

  //  Serial.print("\t T="); Serial.print(thrust);
  //  Serial.print("\t Tcmd="); Serial.print(thrust_cmd);

  //  Serial.print("\t R="); Serial.print(roll);
  //  Serial.print("\t P="); Serial.print(pitch);
  //  Serial.print("\t Y="); Serial.print(yaw);
  //
  //  Serial.print("\t Rsp="); Serial.print(roll_sp);
  //  Serial.print("\t Psp="); Serial.print(pitch_sp);
  //  Serial.print("\t Ysp="); Serial.print(yaw_sp);
  //
  //  Serial.print("\t Rcmd="); Serial.print(roll_cmd);
  //  Serial.print("\t Pcmd="); Serial.print(pitch_cmd);
  //  Serial.print("\t Ycmd="); Serial.print(yaw_cmd);
  //
  //  Serial.print("\t u1="); Serial.print(u1);
  //  Serial.print("\t u2="); Serial.print(u2);
  //  Serial.print("\t u3="); Serial.print(u3);
  //  Serial.print("\t u4="); Serial.println(u4);
}




/////////////////////////////////////////////////////////////////////////////
// REMOTE CONTROL
/////////////////////////////////////////////////////////////////////////////

void remoteControl(bool console) {

  if (console && millis() - timer[0] > 100) {

    Serial.print("*G");
    Serial.print(roll);
    Serial.print(",");
    Serial.print(pitch);
    Serial.print(",");
    Serial.print(yaw);
    Serial.println("*");

    Serial.print("*D");
    Serial.print(thrust);
    Serial.println("*");

    timer[0] = millis();
  }


  if (Serial.available() &&
      Serial.readBytesUntil(END_OF_LINE, serial_buff, BUFF_SIZE) > 0) {


    /////////////////////////////////////////////////////////////////////////
    // MODE INDEPENDENT COMMANDS
    /////////////////////////////////////////////////////////////////////////

    if (isPrefix("YAW_THRUST_")) {

      switch (*(serial_buff + 11)) {
        case '1':
          cmdThrust += THRUST_CMD_STEP;
          break;
        case '2':
          cmdYaw += YAW_CMD_STEP;
          break;
        case '3':
          cmdThrust -= THRUST_CMD_STEP;
          break;
        case '4':
          cmdYaw -= YAW_CMD_STEP;
          break;
        case '0':
          // Do nothing
          break;
        default:
          if (console) {
            Serial.print("*T");
            Serial.println("Syntax error!");
            Serial.println("*");
          }
      }

      cmdThrust = constrain(cmdThrust, THRUST_CMD_MIN, THRUST_CMD_MAX);
      cmdYaw = constrain(cmdYaw, YAW_CMD_MIN, YAW_CMD_MAX);

    } else if (isPrefix("THRUST_")) {

      int val = atoi(serial_buff + 7);
      if (console && val < 0) {
        Serial.print("*T");
        Serial.print("Error: Negative value is forbidden! THRUST: "); Serial.print(val);
        Serial.println("*");
      } else {
        cmdThrust = val;
        if (console) {
          Serial.print("*T");
          Serial.print("cmd: THRUST = "); Serial.println(cmdThrust);
          Serial.println("*");
        }
      }

    } else if (isPrefix("FB_ON")) {

      closed_system = true;

      if (console) {
        Serial.print("*T");
        Serial.println("cmd: FB_ON");
        Serial.println("*");
      }
    } else if (isPrefix("FB_OFF")) {

      closed_system = false;

      if (console) {
        Serial.print("*T");
        Serial.println("cmd: FB_OFF");
        Serial.println("*");
      }
    } else if (isPrefix("RESET_PID")) {

      roll_PID.reset();
      pitch_PID.reset();
      yaw_PID.reset();

      if (console) {
        Serial.print("*T");
        Serial.println("cmd: RESET_PID");
        Serial.println("*");
      }

    } else if (isPrefix("PROPS_ON")) {

      PROPS_ON = true;

      if (console) {
        Serial.print("*T");
        Serial.println("cmd: PROPS_ON");
        Serial.println("*");
      }

    } else if (isPrefix("PROPS_OFF")) {

      analogWrite(P1, 0);
      analogWrite(P2, 0);
      analogWrite(P3, 0);
      analogWrite(P4, 0);

      PROPS_ON = false;
      MANUAL = false;

      if (console) {
        Serial.print("*T");
        Serial.println("cmd: PROPS_OFF");
        Serial.println("cmd: AUTO");
        Serial.println("*");
      }

    } else if (isPrefix("PROP")) {

      if (isNum(serial_buff + 6)) {
        int val = atoi(serial_buff + 6);  // cmd: PROPn_val
        if (val >= 0) {
          if (console) {
            Serial.print("Value: ");
            Serial.println(val);
          }
          if (val <= 255) {
            switch (*(serial_buff + 4)) {
              case '1':
                analogWrite(P1, val);
                break;
              case '2':
                analogWrite(P2, val);
                break;
              case '3':
                analogWrite(P3, val);
                break;
              case '4':
                analogWrite(P4, val);
                break;
              case 'S':
                analogWrite(P1, val);
                analogWrite(P2, val);
                analogWrite(P3, val);
                analogWrite(P4, val);
                break;
              default:
                if (console) {
                  Serial.print("*T");
                  Serial.println("Syntax error! (PROPS)");
                  Serial.println("*");
                }
            }
          } else if (console) {
            Serial.print("*T");
            Serial.println("Value not in range 0-255");
            Serial.println("*");
          }
        } else if (console) {
          Serial.print("*T");
          Serial.println("Negative values are forbidden");
          Serial.println("*");
        }
      } else if (console) {
        Serial.print("*T");
        Serial.print("Value not a number:"); Serial.println(serial_buff + 6);
        Serial.println("*");
      }

    } else if (isPrefix("RP_KP_")) {

      int val = atoi(serial_buff + 6);
      if (console && val < 0) {
        Serial.print("*T");
        Serial.print("Error: Negative value is forbidden! RP_KP: "); Serial.print(val);
        Serial.println("*");
      } else {
        float coef = RP_KP_MAX * (float)val / 255.0f;
        roll_PID.set_k_p( coef );
        pitch_PID.set_k_p( coef );

        if (console) {
          Serial.print("*T");
          Serial.print("cmd: RP_KP = "); Serial.println(coef);
          Serial.println("*");
        }
      }

    } else if (isPrefix("RP_KI_")) {

      int val = atoi(serial_buff + 6);
      if (console && val < 0) {
        Serial.print("*T");
        Serial.print("Error: Negative value is forbidden! RP_KI: "); Serial.print(val);
        Serial.println("*");
      } else {
        float coef = RP_KI_MAX * (float)val / 255.0f;
        roll_PID.set_k_i( coef );
        pitch_PID.set_k_i( coef );

        if (console) {
          Serial.print("*T");
          Serial.print("cmd: RP_KI = "); Serial.println(coef, 3);
          Serial.println("*");
        }
      }

    } else if (isPrefix("RP_KD_")) {

      int val = atoi(serial_buff + 6);
      if (console && val < 0) {
        Serial.print("*T");
        Serial.print("Error: Negative value is forbidden! RP_KD: "); Serial.print(val);
        Serial.println("*");
      } else {
        float coef = RP_KD_MAX * (float)val / 255.0f;
        roll_PID.set_k_d( coef );
        pitch_PID.set_k_d( coef );

        if (console) {
          Serial.print("*T");
          Serial.print("cmd: RP_KD = "); Serial.println(coef);
          Serial.println("*");
        }
      }
    } else if (isPrefix("Y_KP_")) {

      int val = atoi(serial_buff + 5);
      if (console && val < 0) {
        Serial.print("*T");
        Serial.print("Error: Negative value is forbidden! Y_KP: "); Serial.print(val);
        Serial.println("*");
      } else {
        float coef = Y_KP_MAX * (float)val / 255.0f;
        yaw_PID.set_k_p( coef );

        if (console) {
          Serial.print("*T");
          Serial.print("cmd: Y_KP = "); Serial.println(coef);
          Serial.println("*");
        }
      }

    } else if (MANUAL) {

      ///////////////////////////////////////////////////////////////////////
      // MANUAL MODE COMMANDS
      ///////////////////////////////////////////////////////////////////////

      if (isPrefix("ROLL_PITCH_")) {

        if (*(serial_buff + 11) == 'X') {

          cmdRoll = atoi(serial_buff + 12);

          if (console) {
            Serial.print("*T");
            Serial.print("cmdRoll = "); Serial.println(cmdRoll);
            Serial.println("*");
          }

          { // scope of "int i;"
            int i;
            for (i = 0; i < BUFF_SIZE; i++) {
              if (serial_buff[i] == 'Y') {
                cmdPitch = atoi(serial_buff + i + 1);

                if (console) {
                  Serial.print("*T");
                  Serial.print("cmdPitch = "); Serial.println(cmdPitch);
                  Serial.println("*");
                }
                break;
              }
            }

            if (console && serial_buff[i] != 'Y') {
              Serial.print("*T");
              Serial.println("Syntax error! (ROLL_PITCH-Y)");
              Serial.println("*");
            }
          }

        } else if (console) {
          Serial.print("*T");
          Serial.println("Syntax error! (ROLL_PITCH-X)");
          Serial.println("*");
        }

      } else if (0 == strncmp("AUTO", serial_buff, 4)) {
        MANUAL = false;
        cmdRoll = 0;
        cmdPitch = 0;
        cmdYaw = 0;
        //        cmdThrust = 0;

        if (console) {
          Serial.print("*T");
          Serial.println("cmd: AUTO");
          Serial.println("*");
        }
      }
    } else if (0 == strncmp("MANUAL", serial_buff, 5)) {
      MANUAL = true;

      if (console) {
        Serial.print("*T");
        Serial.println("cmd: MANUAL");
        Serial.println("*");
      }
    } else if (console) {
      Serial.print("*T");
      Serial.print("echo: \"");
      Serial.print(serial_buff);
      Serial.println("\"");
      Serial.println("*");
    }
    memset(serial_buff, 0, sizeof(serial_buff));
    Serial.flush();
  }
}




/////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

bool isNum(char *str) {
  for (int i = 0; i < strlen(str); i++) {
    if (!isdigit(str[i])) {
      return false;
    }
  }
  return true;
}




bool isPrefix(const char *pre) {
  size_t lenpre = strlen(pre);
  size_t lenstr = strlen(serial_buff);

  return lenstr < lenpre ? false : strncmp(pre, serial_buff, lenpre) == 0;
}




/////////////////////////////////////////////////////////////////////////////
// OTHER
/////////////////////////////////////////////////////////////////////////////

void playReadySignal() {
  analogWrite(P1, 2);
  analogWrite(P2, 0);
  analogWrite(P3, 2);
  analogWrite(P4, 0);
  delay(200);

  analogWrite(P1, 0);
  analogWrite(P2, 2);
  analogWrite(P3, 0);
  analogWrite(P4, 2);
  delay(200);

  analogWrite(P1, 2);
  analogWrite(P2, 0);
  analogWrite(P3, 2);
  analogWrite(P4, 0);
  delay(200);

  analogWrite(P1, 0);
  analogWrite(P2, 0);
  analogWrite(P3, 0);
  analogWrite(P4, 0);
}




/* PROGRAM CODE END */

#else

void setup(void) {
}

void loop(void) {
}

#endif
