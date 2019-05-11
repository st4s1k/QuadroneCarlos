#include "MPU6050_tockn.h"
#include "Arduino.h"

#define _180_div_PI_ 57.295779513082320876798154814105

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

void MPU6050::begin() {
  writeMPU6050(MPU6050_SMPLRT_DIV, 0x00);
  writeMPU6050(MPU6050_CONFIG, 0x00);
  writeMPU6050(MPU6050_GYRO_CONFIG, 0x08);
  writeMPU6050(MPU6050_ACCEL_CONFIG, 0x00);
  writeMPU6050(MPU6050_PWR_MGMT_1, 0x01);
  this->update();
  angleGyroX = 0;
  angleGyroY = 0;
  angleGyroZ = 0;
  angleX = this->getAccAngleX();
  angleY = this->getAccAngleY();
  preInterval = millis();
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

void MPU6050::setAccelOffsets(float x, float y, float z) {
  accelXoffset = x;
  accelYoffset = y;
  accelZoffset = z;
}

void MPU6050::calcOffsets(bool console, uint16_t iterations, uint16_t delayBefore, uint16_t delayAfter) {

  float gx = 0, gy = 0, gz = 0;
  float ax = 0, ay = 0, az = 0;

  int16_t grx, gry, grz;
  int16_t arx, ary, arz;

  if (console) {
    Serial.print("Calibration will start in ");
    Serial.print(delayBefore / 1000.0f);
    Serial.println(" seconds");
  }

  delay(delayBefore);

  if (console) {
    Serial.println();
    Serial.println("========================================");
    Serial.println("Calculating offsets");
    Serial.print("DO NOT MOVE MPU6050");
  }

  for (int i = 0; i < iterations; i++) {

    if (console && i % 1000 == 0) {
      Serial.print(".");
    }

    wire->beginTransmission(MPU6050_ADDR);
    wire->write(MPU6050_DATA_BEGIN);
    wire->endTransmission(false);
    wire->requestFrom((int)MPU6050_ADDR, 14);

    arx = wire->read() << 8 | wire->read();
    ary = wire->read() << 8 | wire->read();
    arz = wire->read() << 8 | wire->read();
    wire->read(); // skip temperature measurements
    wire->read(); // skip temperature measurements
    grx = wire->read() << 8 | wire->read();
    gry = wire->read() << 8 | wire->read();
    grz = wire->read() << 8 | wire->read();

    ax += (float)arx / 16384.0f;
    ay += (float)ary / 16384.0f;
    az += (float)arz / 16384.0f;

    gx += (float)grx / 65.5f;
    gy += (float)gry / 65.5f;
    gz += (float)grz / 65.5f;
  }

  accelXoffset = ax / iterations;
  accelYoffset = ay / iterations;
  accelZoffset = az / iterations - 1;

  gyroXoffset  = gx / iterations;
  gyroYoffset  = gy / iterations;
  gyroZoffset  = gz / iterations;

  if (console) {
    Serial.println("\nCalibrating done.");
    Serial.println("Offsets:");
    Serial.println("--------------");
    Serial.println("Accelerometer:");
    Serial.print  ("X : "); Serial.println(accelXoffset);
    Serial.print  ("Y : "); Serial.println(accelYoffset);
    Serial.print  ("Z : "); Serial.println(accelZoffset);
    Serial.println("Gyro:");
    Serial.print  ("X : "); Serial.println(gyroXoffset);
    Serial.print  ("Y : "); Serial.println(gyroYoffset);
    Serial.print  ("Z : "); Serial.println(gyroZoffset);
    Serial.println("--------------");
    Serial.print("Program will start after ");
    Serial.print(delayAfter / 1000.0f);
    Serial.println(" seconds");
    Serial.print("========================================");
  }
  delay(delayAfter);
}

void MPU6050::update() {
  wire->beginTransmission(MPU6050_ADDR);
  wire->write(MPU6050_DATA_BEGIN);
  wire->endTransmission(false);
  wire->requestFrom((int)MPU6050_ADDR, 14);

  rawAccX = wire->read() << 8 | wire->read();
  rawAccY = wire->read() << 8 | wire->read();
  rawAccZ = wire->read() << 8 | wire->read();
  rawTemp = wire->read() << 8 | wire->read();
  rawGyroX = wire->read() << 8 | wire->read();
  rawGyroY = wire->read() << 8 | wire->read();
  rawGyroZ = wire->read() << 8 | wire->read();

  temp = (rawTemp + 12412.0f) / 340.0f;

  accX = ((float)rawAccX) / 16384.0f - accelXoffset;
  accY = ((float)rawAccY) / 16384.0f - accelYoffset;
  accZ = ((float)rawAccZ) / 16384.0f - accelZoffset;

  angleAccX =  atan2(accY, accZ + abs(accX)) * _180_div_PI_;
  angleAccY = -atan2(accX, accZ + abs(accY)) * _180_div_PI_;

  gyroX = ((float)rawGyroX) / 65.5f - gyroXoffset;
  gyroY = ((float)rawGyroY) / 65.5f - gyroYoffset;
  gyroZ = ((float)rawGyroZ) / 65.5f - gyroZoffset;

  interval = (millis() - preInterval) * 0.001f;

  angleGyroX += gyroX * interval;
  angleGyroY += gyroY * interval;
  angleGyroZ += gyroZ * interval;

  angleGyroX += angleGyroY * sin(radians(gyroZ * interval));
  angleGyroY -= angleGyroX * sin(radians(gyroZ * interval));

  angleX = (gyroCoef * (angleX + gyroX * interval)) + (accCoef * angleAccX);
  angleY = (gyroCoef * (angleY + gyroY * interval)) + (accCoef * angleAccY);
  angleZ = angleGyroZ;

  preInterval = millis();

}
