//

#include <Arduino.h>
#include <U8g2lib.h>
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

bool api_active = true;

#include <String.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <AS5600.h>
AMS_5600 ams5600;
#define DIR_PIN 2

struct MessageValue
{
  String message;
  String value;
};

struct MessageValue getMessage(String inputtedStr)
{
  struct MessageValue result;

  char charArr[50];
  inputtedStr.toCharArray(charArr, 50);
  char *ptr = strtok(charArr, "||");
  result.message = String(ptr);
  ptr = strtok(NULL, "||");

  if (ptr == NULL)
  {
    result.value = String("");
    return result;
  }

  result.value = String(ptr);

  return result;
}
struct MessageValue receivedData;









class Disp
{
public:
  int amount = 4;                       // amount of displays
  int selectPins[4] = {23, 27, 31, 35}; // active pins for displays

  // select display
  void select(int display)
  {
    for (int i = 0; i < amount; i++)
    {
      if (i == display)
      {
        digitalWrite(selectPins[i], HIGH);
      }
      else
      {
        digitalWrite(selectPins[i], LOW);
      }
    }
  }

  // start displays
  void start()
  {
    for (int i = 0; i < amount; i++)
    {
      select(i);
      u8g2.begin();
    }
  }

  // write to display
  void write(int display, String message = "", String value = "", bool inverted = false)
  {
      select(display);
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_helvR08_tr);
      u8g2.drawStr(0, 10, message.c_str());
      u8g2.drawStr(0, 30, value.c_str());
      u8g2.sendBuffer();
  }
};









class Btns
{
public:
  // amount of buttons
  int amount = 4;

  // pins for buttons
  int pinPins[4] = {22, 26, 30, 34};
  int states[4] = {0, 0, 0, 0};

  // check if button is pressed or released
  void check()
  {
    for (int i = 0; i < amount; i++)
    {
      if (digitalRead(pinPins[i]) == HIGH)
      {
        if (states[i] == 0)
        {
          states[i] = 1;
          Serial.println("Button " + String(i) + " pressed");
        }
      }
      else
      {
        if (states[i] == 1)
        {
          states[i] = 0;
          Serial.println("Button " + String(i) + " released");
        }
      }
    }
  };
};







class Dial
{
public:
  // button pin
  int pin = 39;
  int state = 0;

  // dial value
  int val = 0;

  // check if button is pressed or released
  void check()
  {
    if (digitalRead(pin) == HIGH)
    {
      if (state == 0)
      {
        state = 1;
        Serial.println("Dial pressed");
      }
    }
    else
    {
      if (state == 1)
      {
        state = 0;
        Serial.println("Dial released");
      }
    }
  };
};

class Para
{
public:
  // amount
  int amount = 5;

  // current
  int current = -1;

  // names
  char *names[5] = {
      "Frequenz",
      "Phoch",
      "PEEP",
      "FiO2",
      ""};

  // values
  int val[5] = {12, 20, 5, 20, 0};

  // min values
  int minVal[5] = {1, 6, 0, 21, 0};

  // max values
  int maxVal[5] = {60, 60, 19, 100, 0};

  // current
  void setCurrent(int current)
  {
    this->current = current;
  }
};




class API
{
public:
  //
};

Disp disp;
Btns btns;
Dial dial;
Para para;
API api;

void setup(void)
{
  Serial.begin(9600);
  Wire.begin();

  disp.start();

}

void loop(void)
{
  btns.check();
  dial.check();


  delay(80);
}