#include <Arduino.h>

#define potiPin A0
int potiValue;

void setup()
{
  Serial.begin(9600);
  pinMode(potiPin, INPUT);
  potiValue = analogRead(potiPin);
}

void loop()
{
  // read poti and map to 0-100
  int prevPotiValue = potiValue;
  potiValue = map(analogRead(potiPin), 0, 1023, 100, 0);

  if (prevPotiValue != potiValue)
  {
    Serial.println("ROTATE||" + String(potiValue));
  }

  delay(50);
}