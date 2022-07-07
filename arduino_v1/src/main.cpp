#include <Arduino.h>
#include <LiquidCrystal.h>

// define rotari encoder pins
#define ROTARY A1
#define SENSOR A1
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int counter = 0;

int value_fio2;

void setup()
{
  delay(1000);
  Serial.begin(9600);

  lcd.begin(16, 2);
}

void loop()
{
  // read rotary encoder
  value_fio2 = analogRead(ROTARY);

  lcd.setCursor(0, 0);

  lcd.print("FIO ");

  lcd.setCursor(4, 0);
  if (value_fio2 < 999)
  {
    lcd.print(" ");
    lcd.setCursor(5, 0);
    lcd.print(value_fio2);
  }
  else
  {
    lcd.print(value_fio2);
  }

  // lcd.setCursor(0, 0);
  // lcd.print("test");

  // print rotary encoder values
  Serial.print("c: \t");
  Serial.print(counter);
  counter += 1;
  delay(200);
}