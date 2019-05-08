#ifndef _MPU6050_H
#define _MPU6050_H

#include "i2cmaster.h"

float get_gForceX();
float get_gForceY();
float get_gForceZ();
float get_rotX();
float get_rotY();
float get_rotZ();

void setupMPU();
void recordAccelRegisters();
void processAccelData();
void recordGyroRegisters();
void processGyroData();

#endif