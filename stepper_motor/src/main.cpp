#include <Arduino.h>
#include <Stepper.h>

const float STEPS_PER_REV = 32;
const float GEAR_RED = 64;

const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_RED;

int StepsRequired;

 
Stepper steppermotor(STEPS_PER_REV, 8, 10, 9, 11);

#define ROTARI_PIN_1 A0

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(ROTARI_PIN_1, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  steppermotor.setSpeed(analogRead(ROTARI_PIN_1));

  steppermotor.step(STEPS_PER_OUT_REV / 2);
  // delay(300);

  delay(30);
  Serial.println(analogRead(ROTARI_PIN_1));

}