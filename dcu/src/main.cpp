#include "Arduino.h"
#include "accelerometer.h"

volatile bool dataReady = false;
Acceleration latestAcc;
unsigned long latestTimestamp;

void IRAM_ATTR onMPUDataReady()
{
  // keep ISR tiny
  dataReady = true;
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  setupAccelerometer();

  // attach ESP32 interrupt on GPIO 27 (INT from GY-521)
  attachInterruptAccel(27, onMPUDataReady);
}

void loop()
{
  if (dataReady)
  {
    // read timestamp and values in main context
    latestTimestamp = millis();
    latestAcc = readAcceleration();

    // reset flag
    dataReady = false;

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