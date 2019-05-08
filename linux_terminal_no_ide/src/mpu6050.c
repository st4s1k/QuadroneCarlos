#include "mpu6050.h"

#define MPU6050 0b1101000

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;

float get_gForceX() { return gForceX; }
float get_gForceY() { return gForceY; }
float get_gForceZ() { return gForceZ; }
float get_rotX() { return rotX; }
float get_rotY() { return rotY; }
float get_rotZ() { return rotZ; }

void setupMPU(){
  i2c_init();

  i2c_start_wait(MPU6050+I2C_WRITE); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  i2c_write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  i2c_write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  i2c_stop();

  i2c_start_wait(MPU6050+I2C_WRITE); //I2C address of the MPU
  i2c_write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  i2c_write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  i2c_stop();

  i2c_start_wait(MPU6050+I2C_WRITE); //I2C address of the MPU
  i2c_write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  i2c_write(0b00000000); //Setting the accel to +/- 2g
  i2c_stop();
}

void recordAccelRegisters() {
  i2c_start_wait(MPU6050+I2C_WRITE); //I2C address of the MPU
  i2c_write(0x3B); //Starting register for Accel Readings
  i2c_stop();
  i2c_rep_start(MPU6050+I2C_READ); //Request Accel Registers (3B - 40)
  accelX = i2c_readAck()<<8|i2c_readAck(); //Store first two bytes into accelX
  accelY = i2c_readAck()<<8|i2c_readAck(); //Store middle two bytes into accelY
  accelZ = i2c_readAck()<<8|i2c_readNak(); //Store last two bytes into accelZ
  i2c_stop();  
  processAccelData();
}

void processAccelData(){
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
}

void recordGyroRegisters() {
  i2c_start_wait(MPU6050+I2C_WRITE); //I2C address of the MPU
  i2c_write(0x43); //Starting register for Gyro Readings
  i2c_stop();
  i2c_rep_start(MPU6050+I2C_READ); //Request Gyro Registers (43 - 48)
  gyroX = i2c_readAck()<<8|i2c_readAck(); //Store first two bytes into accelX
  gyroY = i2c_readAck()<<8|i2c_readAck(); //Store middle two bytes into accelY
  gyroZ = i2c_readAck()<<8|i2c_readNak(); //Store last two bytes into accelZ
  i2c_stop(); 
  processGyroData();
}

void processGyroData() {
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0; 
  rotZ = gyroZ / 131.0;
}