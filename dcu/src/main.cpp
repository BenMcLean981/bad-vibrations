
#include "Arduino.h"

#ifndef LED_BUILTIN
  #define LED_BUILTIN 2
#endif

void setup()
{
  Serial.begin(9600); 

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  Serial.print("Current time (ms): "); 
  Serial.println(millis()); 

  digitalWrite(LED_BUILTIN, HIGH);

  delay(1000);

  digitalWrite(LED_BUILTIN, LOW);

  delay(1000);
}