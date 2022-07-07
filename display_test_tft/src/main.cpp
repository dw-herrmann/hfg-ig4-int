#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

#define BUTTON_PIN_1 2
int button_state_1 = 0;
int button_state_1_prev = 0;

#if defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
#define TFT_CS 14
#define TFT_RST 15
#define TFT_DC 32

#elif defined(ESP8266)
#define TFT_CS 4
#define TFT_RST 16
#define TFT_DC 5

#else
// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS 10
#define TFT_RST 8 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 9
#endif

// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

float p = 3.1415926;

void setup(void)
{
  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));
  pinMode(BUTTON_PIN_1, INPUT);
  // tft.setRotation(1);

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB); // Init ST7735S chip, black tab

  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);
  tft.setRotation(3);

  Serial.println("done");
  delay(1000);
}

void loop()
{
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.setTextWrap(true);

  button_state_1 = digitalRead(BUTTON_PIN_1);
  if (button_state_1 != button_state_1_prev)
  {
    if (button_state_1 == HIGH)
    {
      Serial.println("Button 1 pressed");
      tft.fillScreen(ST77XX_BLACK);
      tft.print("pressed");
    }
    else
    {
      Serial.println("Button 1 released");
      tft.fillScreen(ST77XX_BLACK);
      tft.print("released");
    }

    button_state_1_prev = button_state_1;
  }
  
  delay(100);
}
