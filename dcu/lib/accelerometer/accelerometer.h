#pragma once

void setupAccelerometer();

struct Acceleration
{
  float x;
  float y;
  float z;
};

Acceleration readAcceleration();

void attachInterruptAccel(uint8_t pin, void (*isr)());
