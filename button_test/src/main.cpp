#include <Arduino.h>

// button  read
#define BUTTON_PIN_1 2

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUTTON_PIN_1, INPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  Serial.println(digitalRead(BUTTON_PIN_1));
  delay(100);
}