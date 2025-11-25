#pragma once

void setupAccelerometer();

struct Acceleration
{
  float x;
  float y;
  float z;
};

Acceleration readAcceleration();
