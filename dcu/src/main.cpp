#include "Arduino.h"
#include "Wire.h"

#include <accelerometer.h>

#ifndef BAUD_RATE
#define BAUD_RATE 115200
#endif

float Ax, Ay, Az;

unsigned long timestamp;

void setup()
{
  setupAccelerometer();

  Serial.begin(BAUD_RATE);
}

void writeValues(unsigned long timestamp, Acceleration acc)
{
  String line = "l:" +
                String(timestamp) + "," +
                String(acc.x) + "," +
                String(acc.y) + "," +
                String(acc.z);

  Serial.println(line);
}

void loop()
{
  Acceleration acc = readAcceleration();

  timestamp = millis();

  writeValues(timestamp, acc);

  delay(1);
}