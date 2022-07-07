#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define OLED_RESET -1
#define SCREE_WIDTH 128
#define SCREE_HEIGHT 64

Adafruit_SSD1306 display(SCREE_WIDTH, SCREE_HEIGHT, &Wire, OLED_RESET);

#define POTI_PIN A0

int poti_value = 0;
int counter = 0;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  delay(1000);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Heilischer Bimbam, desch haut hin!");
  display.display();
}

void loop()
{
  // Serial.begin(9600);
  // display.clearDisplay();
  // display.setCursor(0, 0);
  // display.setTextSize(0);

  // poti_value = analogRead(POTI_PIN);

  // delay(50);

  // // rect 
  // int poti_value_rect = map(poti_value, 0, 1023, 0, SCREE_WIDTH);
  // display.fillRect(0, 0, poti_value_rect, 20, WHITE);

  // display.setCursor(0, 30);
  // display.setTextSize(1);
  // display.setTextColor(WHITE);
  
  // display.print("Poti Value: " + String(poti_value));

  // display.display();
  

  // display.display();
}