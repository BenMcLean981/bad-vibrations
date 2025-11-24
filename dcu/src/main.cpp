#include "Arduino.h"
#include "Wire.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

#ifndef BAUD_RATE
#define BAUD_RATE 115200
#endif

#ifndef MPU_ADDRESS
#define MPU_ADDRESS 0x68
#endif

float Ax, Ay, Az;

unsigned long timestamp;

// Sensitivity constants for different ranges
const float G = 9.80665; // m/s²
float ACCEL_SENS;        // Sensitivity (will be set dynamically)

// Define the accelerometer range (±2g, ±4g, ±8g, ±16g)
#define ACCEL_RANGE 8 // Change this to 2, 4, 8, or 16

// Timer interrupt variables
hw_timer_t *timer = NULL;
volatile bool readFlag = false;

void IRAM_ATTR onTimer()
{
  readFlag = true; // Set flag to indicate it's time to read data
}

void setup()
{
  Wire.begin();

  // Set accelerometer full scale range
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x1C); // ACCEL_CONFIG

  // Set the range based on ACCEL_RANGE
  uint8_t rangeSetting = 0x00; // Default to ±2g
  switch (ACCEL_RANGE)
  {
  case 2:
    rangeSetting = 0x00;
    ACCEL_SENS = 16384.0;
    break;
  case 4:
    rangeSetting = 0x08;
    ACCEL_SENS = 8192.0;
    break;
  case 8:
    rangeSetting = 0x10;
    ACCEL_SENS = 4096.0;
    break;
  case 16:
    rangeSetting = 0x18;
    ACCEL_SENS = 2048.0;
    break;
  default:
    rangeSetting = 0x00; // Default to ±2g
    ACCEL_SENS = 16384.0;
    break;
  }

  Wire.write(rangeSetting); // Set the range
  Wire.endTransmission(true);

  Serial.begin(BAUD_RATE);

  // Configure the timer interrupt
  timer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1 µs per tick)
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000, true); // Trigger every 1000 µs (1 ms)
  timerAlarmEnable(timer);
}

int16_t readSensorData()
{
  return Wire.read() << 8 | Wire.read();
}

void writeValues()
{
  Serial.print(timestamp);
  Serial.print(",");

  Serial.print(Ax);
  Serial.print(",");

  Serial.print(Ay);
  Serial.print(",");

  Serial.print(Az);

  Serial.print("\n");
}

void readMPU6050()
{
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, 14, true);

  int16_t AcX = readSensorData();
  int16_t AcY = readSensorData();
  int16_t AcZ = readSensorData();

  readSensorData(); // Tmp

  readSensorData(); // GyX
  readSensorData(); // GyY
  readSensorData(); // GyZ

  Ax = (AcX * G) / ACCEL_SENS;
  Ay = (AcY * G) / ACCEL_SENS;
  Az = (AcZ * G) / ACCEL_SENS;

  timestamp = millis();
}

void loop()
{
  if (readFlag)
  {
    readFlag = false; // Reset the flag
    readMPU6050();
    writeValues();
  }
}