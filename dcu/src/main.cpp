#include "Arduino.h"
#include "accelerometer.h"

hw_timer_t *timer = nullptr;
volatile bool tick = false;

struct Sample
{
  uint32_t timestamp; // 4 bytes
  float x, y, z;      // 4 + 4 + 4 bytes
};

Acceleration latestAcc;
unsigned long latestTimestamp;

void IRAM_ATTR onTimer()
{
  tick = true;
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  setupAccelerometer();

  const int sampleRate = 1000; // Hz

  timer = timerBegin(0, 80, true); // 80 MHz / 80 = 1 MHz tick
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000 / sampleRate, true);
  timerAlarmEnable(timer);
}

void loop()
{
  if (tick)
  {
    // read timestamp and values in main context
    unsigned long timestamp = millis();
    Acceleration acc = readAcceleration();

    // reset flag
    tick = false;

    Sample s = {timestamp, acc.x, acc.y, acc.z};
    uint8_t header = 0xAA;
    Serial.write(&header, 1);
    Serial.write((uint8_t *)&s, sizeof(s));
  }
}