#include "Arduino.h"
#include "accelerometer.h"

hw_timer_t *timer = nullptr;
volatile bool tick = false;

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
    latestTimestamp = millis();
    latestAcc = readAcceleration();

    // reset flag
    tick = false;

    Serial.print("l:");
    Serial.print(latestTimestamp);
    Serial.print(",");
    Serial.print(latestAcc.x, 6);
    Serial.print(",");
    Serial.print(latestAcc.y, 6);
    Serial.print(",");
    Serial.println(latestAcc.z, 6);
  }
}