#include <Arduino.h>
#include <U8g2lib.h>
#include <string.h>


#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif



struct MessageValue {
  String message;
  String value;
};

struct MessageValue getMessage(String inputtedStr) {
  struct MessageValue result;

  char charArr[50];
  inputtedStr.toCharArray(charArr, 50);
  char* ptr = strtok(charArr, "||");
  result.message = String(ptr);
  ptr = strtok(NULL, "||");

  if (ptr == NULL) {
    result.value = String("");
    return result;
  }

  result.value = String(ptr);

  return result;
}

int firstLED = 3;
int secondLED = 5;
int thirdLED = 6;
struct MessageValue receivedData;


// U8G2_SH1106_128X64_NONAME_F_HW_I2C(rotation, [reset [, clock, data]]) [full framebuffer, size = 1024 bytes]
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

class Parameters
{
  public:
    // dial
    int dial_pin = A14;
    int dial_value = 0;
    int dial_button = 37;


    // currently selected parameter
    int currentParameter = 1;
    
    // number of parameters and displays
    int numberOfParameters = 4;
    int numberOfDisplays = 4;

    // names of parameters
    char *parameterNames[4] = {
      "FiO2",
      "RR",
      "PEEP",
      "Phoch",
    };

    // values of parameters
    int parameterValues[4] = { 0, 0, 0, 0};

    int parameterValuesChangeDetect[4] = {0,0,0,0};

    // parameter value range
    int parameterValueRange[4][2] = {
      {0, 100},
      {0, 100},
      {0, 100},
      {0, 100}
    };

    // display of parameter
    int display_array[4][4] = { 
      {1,0,0,0}, 
      {0,1,0,0}, 
      {0,0,1,0}, 
      {0,0,0,1} 
    };

    // selection pins
    int displaySelect[4] = {23, 27, 31, 35};

    // button pins
    int displayButtonPins[4] = {22, 26, 30, 34};
    int displayButtonStates[4] = {0, 0, 0, 0};

    // selecting a parameter to edit
    void activateParameter(int selectedParameter) {
      if (selectedParameter != currentParameter) {
        currentParameter = selectedParameter;
        return;
      } else {
        currentParameter = -1;
        return;
      }
    };

    void selectDisplay(int selectedDisplay) {
      for (int i = 0; i < numberOfParameters; i++) {
        digitalWrite(displaySelect[i], display_array[selectedDisplay][i]);
      }
    };

    void readParameter() {
      if(currentParameter != -1) {
        parameterValues[currentParameter] = dial_value;
        return;
      }
    }

    // button pressed
    void buttonPress (int button) {
      if(
        digitalRead(displayButtonPins[button]) != displayButtonStates[button]
      ) {
        // if clicked
        if (
          digitalRead(displayButtonPins[button]) == 1
        ) {
          // recognize change
          Serial.println("button pressed");
          displayButtonStates[button] = digitalRead(displayButtonPins[button]);
          
          // state toggle
          activateParameter(button);

        } else {
          // recognize change
          displayButtonStates[button] = digitalRead(displayButtonPins[button]);
          Serial.println("button released");
        }
      }
    }

    void drawParameter() {
      // draw current parameter, if current parameter is not -1
      if (currentParameter != -1) {
        selectDisplay(currentParameter);

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_logisoso16_tf);
        u8g2.setFontPosTop();
        u8g2.drawStr(0, 12, parameterNames[currentParameter]);
        u8g2.setFont(u8g2_font_logisoso28_tf);
        u8g2.setFontPosTop();
        // u8g2.drawStr(0, 32, map(parameterValues[currentParameter], parameterValueRange[currentParameter][0], parameterValueRange[currentParameter][1], 0, 1023));
        u8g2.drawStr(0, 34, String(map(parameterValues[currentParameter], 0, 1023, parameterValueRange[currentParameter][0], parameterValueRange[currentParameter][1])).c_str());
        u8g2.sendBuffer();
        return;
      }
    };

    // string of received message
    char receivedMessage[100] = "";

    void api () {
      // send parameter
      // if (parameterValuesChangeDetect[currentParameter] != parameterValues[currentParameter]) {
        // check for changes

        parameterValuesChangeDetect[currentParameter] = parameterValues[currentParameter];


        Serial.println("currentParameter|| " + String(currentParameter));
        Serial.println("parameterValue|| " + String(parameterValues[currentParameter]));
        Serial.println("parameterNames|| " + String(parameterNames[currentParameter]));
      // }


      // receive parameter
      while (Serial.available() > 0) { // Take out strings until Serial is empty
        String receivedString = Serial.readStringUntil('\0'); // From 1.9.0 version, We can use '\0' as delimiter in Arduino Serial
        receivedData = getMessage(receivedString);
      }

      Serial.print(receivedData.message);
      Serial.print(receivedData.value);

      // switch
      if (receivedData.message == "buttonPress") {
        currentParameter = receivedData.value.toInt();
      }  
    }
};


















Parameters parameters;

void setup(void) {
  Serial.begin(9600);

  // set Pin Modes
  for (int i = 0; i < parameters.numberOfParameters; i++) {
    pinMode(parameters.displaySelect[i], OUTPUT);
    pinMode(parameters.displayButtonPins[i], INPUT);

    // set pull-up resistor
    parameters.selectDisplay(i);
    u8g2.begin();
  }
}

void loop(void) {
  // read out sensors
  for (int i = 0; i < parameters.numberOfParameters; i++) {
    parameters.buttonPress(i);
  }
  parameters.dial_value = analogRead(parameters.dial_pin);



  parameters.api();  

  // Serial.println("cur:" + String(parameters.currentParameter) + " val:" + String(parameters.parameterValues[parameters.currentParameter]));
  parameters.readParameter();
  parameters.drawParameter();

  // deley between each page
  delay(80);
}