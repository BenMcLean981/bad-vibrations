
#include "Arduino.h"

#ifndef LED_BUILTIN
  #define LED_BUILTIN 2
  #define SOUND_SPEED 0.034

  #define TRIG_PIN 5
  #define ECHO_PIN 18
#endif

//define sound speed in cm/uS

long duration;
unsigned long timeStamp;
float distanceCm;
float distanceInch;

void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN, INPUT); // Sets the echoPin as an Input
}

void loop() {
  // Clears the trigPin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  timeStamp = millis();
  
  Serial.println(String(timeStamp) + "," +  String(distanceCm));

  delay(100);
}