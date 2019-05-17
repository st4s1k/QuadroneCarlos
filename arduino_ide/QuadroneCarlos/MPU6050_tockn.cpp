
#include "MPU6050_tockn.h"

#include "Arduino.h"

#include "Kalman.h"

#define _180_over_PI_ 57.295779513082320876798154814105

Kalman  kalman_x;
Kalman  kalman_y;

MPU6050::MPU6050(TwoWire &w) {
  wire = &w;
  accCoef = 0.02f;
  gyroCoef = 0.98f;
}

MPU6050::MPU6050(TwoWire &w, float aC, float gC) {
  wire = &w;
  accCoef = aC;
  gyroCoef = gC;
}

int MPU6050::begin() {
  this->filter = filter;
  writeMPU6050(MPU6050_SMPLRT_DIV, 0x00);
  writeMPU6050(MPU6050_CONFIG, 0x03);
  writeMPU6050(MPU6050_GYRO_CONFIG, 0x08);
  writeMPU6050(MPU6050_ACCEL_CONFIG, 0x00);
  writeMPU6050(MPU6050_PWR_MGMT_1, 0x01);
  this->update();
  angleGyroX = 0;
  angleGyroY = 0;
  angleGyroZ = 0;
  angleX = this->getAccAngleX();
  angleY = this->getAccAngleY();
  kalman_x.setAngle(angleX);
  kalman_y.setAngle(angleY);
  preInterval = micros();
}

void MPU6050::writeMPU6050(byte reg, byte data) {
  wire->beginTransmission(MPU6050_ADDR);
  wire->write(reg);
  wire->write(data);
  wire->endTransmission();
}

byte MPU6050::readMPU6050(byte reg) {
  wire->beginTransmission(MPU6050_ADDR);
  wire->write(reg);
  wire->endTransmission(true);
  wire->requestFrom(MPU6050_ADDR, 1);
  byte data =  wire->read();
  return data;
}

void MPU6050::setGyroOffsets(float x, float y, float z) {
  gyroXoffset = x;
  gyroYoffset = y;
  gyroZoffset = z;
}

void MPU6050::setAngleOffsets(float x, float y, float z) {
  angleXoffset = x;
  angleYoffset = y;
  angleZoffset = z;
}

void MPU6050::calcGyroOffsets(bool console, uint16_t iterations, uint16_t delayBefore, uint16_t delayAfter) {

  float gx = 0, gy = 0, gz = 0;

  int16_t grx, gry, grz;

  if (console) {
    Serial.print("Gyro calibration will start in ");
    Serial.print(delayBefore / 1000.0f);
    Serial.println(" seconds");
  }

  delay(delayBefore);

  if (console) {
    Serial.println();
    Serial.println("========================================");
    Serial.println("Calculating gyro offsets");
    Serial.print("DO NOT MOVE MPU6050");
  }

  for (int i = 0; i < iterations; i++) {

    if (console && i % 1000 == 0) {
      Serial.print(".");
    }

    wire->beginTransmission(MPU6050_ADDR);
    wire->write(MPU6050_GYRO_H);
    wire->endTransmission(false);
    wire->requestFrom((int)MPU6050_ADDR, 6);

    grx = wire->read() << 8 | wire->read();
    gry = wire->read() << 8 | wire->read();
    grz = wire->read() << 8 | wire->read();

    gx += (float)grx / 65.5f;
    gy += (float)gry / 65.5f;
    gz += (float)grz / 65.5f;
  }

  gyroXoffset = gx / iterations;
  gyroYoffset = gy / iterations;
  gyroZoffset = gz / iterations;

  if (console) {
    Serial.println("\nCalibrating gyro done.");
    Serial.println("Offsets:");
    Serial.println("--------------");
    Serial.print  ("X : "); Serial.println(gyroXoffset);
    Serial.print  ("Y : "); Serial.println(gyroYoffset);
    Serial.print  ("Z : "); Serial.println(gyroZoffset);
    Serial.println("--------------");
    Serial.print("Program will start after ");
    Serial.print(delayAfter / 1000.0f);
    Serial.println(" seconds");
    Serial.println("========================================");
  }
  delay(delayAfter);
}

void MPU6050::calcAngleOffsets(bool console, uint16_t iterations, uint16_t delayBefore, uint16_t delayAfter) {

  float ax = 0, ay = 0, az = 0;

  if (console) {
    Serial.print("Angle calibration will start in ");
    Serial.print(delayBefore / 1000.0f);
    Serial.println(" seconds");
  }

  delay(delayBefore);

  if (console) {
    Serial.println();
    Serial.println("========================================");
    Serial.println("Calculating angle offsets");
    Serial.print("DO NOT MOVE MPU6050");
  }

  for (int i = 0; i < iterations; i++) {

    if (console && i % 1000 == 0) {
      Serial.print(".");
    }

    this->update();

    ax += angleAccX;
    ay += angleAccY;
    az += angleAccZ;
  }

  angleXoffset  = ax / iterations;
  angleYoffset  = ay / iterations;
  angleZoffset  = az / iterations;

  if (console) {
    Serial.println("\nCalibrating angles done.");
    Serial.println("Offsets:");
    Serial.println("--------------");
    Serial.print  ("X : "); Serial.println(angleXoffset);
    Serial.print  ("Y : "); Serial.println(angleYoffset);
    Serial.print  ("Z : "); Serial.println(angleZoffset);
    Serial.println("--------------");
    Serial.print("Program will start after ");
    Serial.print(delayAfter / 1000.0f);
    Serial.println(" seconds");
    Serial.println("========================================");
  }
  delay(delayAfter);
}

void MPU6050::update() {
  wire->beginTransmission(MPU6050_ADDR);
  wire->write(MPU6050_DATA_BEGIN);
  wire->endTransmission(false);
  wire->requestFrom((int)MPU6050_ADDR, 14);

  rawAccX  = wire->read() << 8 | wire->read();
  rawAccY  = wire->read() << 8 | wire->read();
  rawAccZ  = wire->read() << 8 | wire->read();
  rawTemp  = wire->read() << 8 | wire->read();
  rawGyroX = wire->read() << 8 | wire->read();
  rawGyroY = wire->read() << 8 | wire->read();
  rawGyroZ = wire->read() << 8 | wire->read();

  temp = (rawTemp + 12412.0f) / 340.0f;

  accX = ((float)rawAccX) / 16384.0f;
  accY = ((float)rawAccY) / 16384.0f;
  accZ = ((float)rawAccZ) / 16384.0f;

  angleAccX =  atan2(accY, accZ + abs(accX)) * _180_over_PI_;
  angleAccY = -atan2(accX, accZ + abs(accY)) * _180_over_PI_;

  gyroX = ((float)rawGyroX) / 65.5f - gyroXoffset;
  gyroY = ((float)rawGyroY) / 65.5f - gyroYoffset;
  gyroZ = ((float)rawGyroZ) / 65.5f - gyroZoffset;

  interval = (float)(micros() - preInterval) / 1000000.0f;

  angleGyroX += gyroX * interval;
  angleGyroY += gyroY * interval;
  angleGyroZ += gyroZ * interval;

  //  // For this correction, the sensor has to be parallel to the body
  //  angleGyroX += angleGyroY * sin(radians(gyroZ * interval));
  //  angleGyroY -= angleGyroX * sin(radians(gyroZ * interval));

  // Complementary filter
  //  angleX = (gyroCoef * (angleX + gyroX * interval)) + (accCoef * angleAccX);
  //  angleY = (gyroCoef * (angleY + gyroY * interval)) + (accCoef * angleAccY);
  //  angleZ = angleGyroZ;

  // Kalman filter
  angleX = kalman_x.getAngle(angleAccX, gyroX, interval);
  angleY = kalman_y.getAngle(angleAccY, gyroY, interval);
  angleZ = angleGyroZ;

  preInterval = micros();

}
