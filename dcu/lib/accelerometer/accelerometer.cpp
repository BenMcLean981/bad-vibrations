#include <Wire.h>
#include "accelerometer.h"

#ifndef MPU_ADDRESS
#define MPU_ADDRESS 0x68
#endif

#ifndef ACCEL_RANGE
#define ACCEL_RANGE 2 // Change this to 2, 4, 8,
#endif

const float ACCEL_SENS = (1 << 15) / ACCEL_RANGE;

#pragma region Helpers
unsigned log2_pow2(unsigned n)
{
  unsigned r = 0;

  while (n >>= 1)
    r++;

  return r;
}

int16_t readWord()
{
  uint8_t hi = Wire.read();
  uint8_t lo = Wire.read();

  return (int16_t)((hi << 8) | lo);
}

void writeByte(uint8_t reg, uint8_t data)
{
  Wire.beginTransmission(MPU_ADDRESS);

  Wire.write(reg);
  Wire.write(data);

  Wire.endTransmission();
}

void writeAccelConfig()
{
  unsigned mode = log2_pow2(ACCEL_RANGE) - 1;

  writeByte(0x1C, mode << 3);
}

void wakeUp()
{
  writeByte(0x6B, 0x00);
}
#pragma endregion

void setupAccelerometer()
{
  Wire.begin();

  wakeUp();
  writeAccelConfig();
}

Acceleration readAcceleration()
{
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, 6, 1);

  int16_t rawAx = readWord();
  int16_t rawAy = readWord();
  int16_t rawAz = readWord();

  Acceleration accel;
  accel.x = (float)rawAx / ACCEL_SENS;
  accel.y = (float)rawAy / ACCEL_SENS;
  accel.z = (float)rawAz / ACCEL_SENS;

  return accel;
}
