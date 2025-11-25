#include <Wire.h>
#include "accelerometer.h"
#include <Arduino.h>

#ifndef MPU_ADDRESS
#define MPU_ADDRESS 0x68
#endif

#ifndef ACCEL_RANGE
#define ACCEL_RANGE 2
#endif

const float ACCEL_SENS = (1 << 15) / ACCEL_RANGE;

// Helpers (your existing code) ...
unsigned log2_pow2(unsigned n)
{
  unsigned r = 0;
  while (n >>= 1)
    r++;
  return r;
}

static int16_t readWord()
{
  // read two bytes from Wire (big-endian)
  while (Wire.available() < 2)
    ; // wait (caller requested enough bytes)

  uint8_t hi = Wire.read();
  uint8_t lo = Wire.read();
  return (int16_t)((hi << 8) | lo);
}

static void writeByte(uint8_t reg, uint8_t data)
{
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

// --- public API ---

void writeAccelConfig()
{
  unsigned mode = log2_pow2(ACCEL_RANGE) - 1; // 2 -> 0, 4 -> 1, 8 -> 2
  writeByte(0x1C, (uint8_t)(mode << 3));
}

void wakeUp()
{
  // clear sleep bit
  writeByte(0x6B, 0x00);
}

void setupAccelerometer()
{
  Wire.begin();

  wakeUp();
  writeAccelConfig();

  // Configure INT pin on MPU: INT active HIGH, push-pull, 50us pulse
  // 0x37 = INT_PIN_CFG
  writeByte(0x37, 0x10);

  // Enable DATA READY interrupt
  // 0x38 = INT_ENABLE, bit 0 = DATA_RDY_EN
  writeByte(0x38, 0x01);
}

Acceleration readAcceleration()
{
  // request 6 bytes starting at ACCEL_XOUT_H (0x3B)
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, (uint8_t)6, (uint8_t)true);
  clearAccelInterrupt();

  int16_t rawAx = readWord();
  int16_t rawAy = readWord();
  int16_t rawAz = readWord();

  Acceleration accel;
  accel.x = (float)rawAx / ACCEL_SENS;
  accel.y = (float)rawAy / ACCEL_SENS;
  accel.z = (float)rawAz / ACCEL_SENS;

  return accel;
}

void attachInterruptAccel(uint8_t pin, void (*isr)())
{
  // MPU INT is open-drain on GY-521; enable internal pull-up
  pinMode(pin, INPUT_PULLUP);
  ::attachInterrupt(digitalPinToInterrupt(pin), isr, RISING);

  // already enabled DATA_RDY_EN in setupAccelerometer, but keep for safety
  writeByte(0x38, 0x01);
}

void clearAccelInterrupt()
{
  // Reading INT_STATUS (0x3A) clears the DATA_RDY interrupt on the MPU
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3A);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, (uint8_t)1, (uint8_t)true);
  if (Wire.available())
  {
    (void)Wire.read();
  }
}