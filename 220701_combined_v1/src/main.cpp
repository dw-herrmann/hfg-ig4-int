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

int firstLED = 3;
int secondLED = 5;
int thirdLED = 6;
struct MessageValue receivedData;

// U8G2_SH1106_128X64_NONAME_F_HW_I2C(rotation, [reset [, clock, data]]) [full framebuffer, size = 1024 bytes]

class Parameters
{
public:
  // dial
  int dial_button = 39;
  // current, before, first on button press, rotations
  int currentEncoderVal[5] = {0, 0, 0, 0, 0};

  // currently selected parameter
  int currentParameter = -1;
  int valOnClick = 0;

  // number of parameters and displays
  int numberOfButtons = 5;
  int numberOfParameters = 5;
  int numberOfDisplays = 4;

  // names of parameters
  char *parameterNames[5] = {
      "Frequenz",
      "Phoch",
      "PEEP",
      "FiO2",
      "",
  };

  // values of parameters
  int parameterValues[5] = {12, 20, 5, 20, 0};
  int parameterValuePrev = 0;

  // int parameterValuesChangeDetect[4] = {0, 0, 0, 0};

  // parameter value range, min, max, steps per rotation
  int parameterValueRange[5][3] = {
      {1, 60, 60},   // Frequenz
      {6, 60, 60},   // Phoch
      {0, 19, 20},   // PEEP
      {21, 100, 60}, // FiO2
      {0, 0, 60},    // custom
  };

  // display of parameter
  int display_array[4][4] = {
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1}};

  // selection pins
  int displaySelect[4] = {23, 27, 31, 35};

  // button pins
  int displayButtonPins[5] = {22, 26, 30, 34, dial_button};
  int displayButtonStates[4] = {0, 0, 0, 0};

  // selecting a parameter to edit
  void activateParameter(int selectedParameter)
  {
    if (selectedParameter != currentParameter)
    {
      currentParameter = selectedParameter;

      // (re)set all encoder values
      currentEncoderVal[0] = ams5600.getRawAngle();
      currentEncoderVal[1] = ams5600.getRawAngle();
      currentEncoderVal[2] = ams5600.getRawAngle();
      currentEncoderVal[3] = 0;

      valOnClick = parameterValues[selectedParameter];
      disableParametersAll(selectedParameter);

      api("activateParameter");
      return;
    }
    else
    {
      disableParametersAll();
      currentParameter = -1;
      api("activateParameter");
      return;
    }
  };

  void disableParametersAll(int except = -1)
  {
    for (int i = 0; i < numberOfDisplays; i++)
    {
      if (except != -1 && i == except)
      {
        drawParameter(i, true);
      }
      else
      {
        drawParameter(i, false);
      }
    }
  }

  void
  selectDisplay(int selectedDisplay)
  {
    for (int i = 0; i < numberOfParameters; i++)
    {
      digitalWrite(displaySelect[i], display_array[selectedDisplay][i]);
    }
  };

  void readParameter()
  {
    if (currentParameter != -1)
    {
      // read sensor
      currentEncoderVal[0] = ams5600.getRawAngle();

      // // // // direction
      // get direction and set "before" for next loop
      int direction = currentEncoderVal[0] - currentEncoderVal[1];
      currentEncoderVal[1] = currentEncoderVal[0];

      // // // // rotations
      // if turned above threshhold of 0/2048
      if (direction > 2048)
      {
        // make rotation -1
        currentEncoderVal[3] -= 1;
      }
      else if (direction < -2048)
      {
        // make rotation +1
        currentEncoderVal[3] += 1;
      }

      // // // // distance
      // get distance from "first on button press" to now
      int distance =
          // current value
          (currentEncoderVal[0]
           // + rotations
           + (currentEncoderVal[3] * 4096))
          // - "first on button press"
          - (currentEncoderVal[2]);

      // // // // value
      int stepSize = 4096 / parameterValueRange[currentParameter][2];
      int steps = distance / stepSize;

      parameterValues[currentParameter] = valOnClick + steps;

      // // // // limit
      if (parameterValues[currentParameter] > parameterValueRange[currentParameter][1])
      {
        parameterValues[currentParameter] = parameterValueRange[currentParameter][1];
        // reset encoder
        currentEncoderVal[2] = currentEncoderVal[0] - (parameterValueRange[currentParameter][1] * stepSize);
        currentEncoderVal[3] = 0;
      }
      else if (parameterValues[currentParameter] < parameterValueRange[currentParameter][0])
      {
        parameterValues[currentParameter] = parameterValueRange[currentParameter][0];
        currentEncoderVal[2] = currentEncoderVal[0];
        currentEncoderVal[3] = 0;
      }

      // api
      api("updateValue");
      return;
    }
  }

  // button pressed
  void buttonPress(int button)
  {
    if (
        digitalRead(displayButtonPins[button]) != displayButtonStates[button])
    {
      // if clicked
      if (
          digitalRead(displayButtonPins[button]) == 1)
      {
        displayButtonStates[button] = digitalRead(displayButtonPins[button]);

        // if main dial
        if (button == 4)
        {
          Serial.println("dial pressed");

          if (currentParameter != -1)
          {
            activateParameter(-1);
          }
        }
        // if display buttons
        else
        {
          // recognize change
          Serial.println("button pressed");

          // state toggle
          activateParameter(button);
        }
      }
      else
      {
        // recognize change
        displayButtonStates[button] = digitalRead(displayButtonPins[button]);

        if (button == 4)
        {
          Serial.println("dial released");
        }
        else
        {
          Serial.println("button released");
        }
      }
    }
  }

  void drawParameter(int display = -1, bool state = false)
  {
    if (display != -1 && display != 4)
    {
      selectDisplay(display);

      if (state)
      {
        u8g2.clearBuffer();
        // set white background
        u8g2.setDrawColor(1);
        u8g2.drawBox(0, 0, 128, 64);
        // set black text
        u8g2.setDrawColor(0);
      }
      else
      {
        u8g2.clearBuffer();
        // set black background
        u8g2.setDrawColor(0);
        u8g2.drawBox(0, 0, 128, 64);
        // set white text
        u8g2.setDrawColor(1);
      }

      u8g2.setFont(u8g2_font_logisoso16_tf);
      u8g2.setFontPosTop();
      u8g2.drawStr(3, 43, parameterNames[display]);
      u8g2.setFont(u8g2_font_logisoso34_tf);
      u8g2.setFontPosTop();
      u8g2.drawStr(2, 3, String(parameterValues[display]).c_str());
      u8g2.sendBuffer();
      return;
    }
    // draw current parameter
    else if (currentParameter != -1 && display != 4)
    {
      selectDisplay(currentParameter);

      u8g2.clearBuffer();
      // set white background
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, 0, 128, 64);
      // set black text
      u8g2.setDrawColor(0);
      u8g2.setFont(u8g2_font_logisoso16_tf);
      u8g2.setFontPosTop();
      u8g2.drawStr(3, 43, parameterNames[currentParameter]);
      u8g2.setFont(u8g2_font_logisoso34_tf);
      u8g2.setFontPosTop();
      u8g2.drawStr(2, 3, String(parameterValues[currentParameter]).c_str());
      u8g2.sendBuffer();
      return;
    }
  };

  // string of received message
  char receivedMessage[100] = "";

  void api(
      String message = "" /* message to send */,
      String value = "" /* value to send */)
  {
    // send
    // - - current parameter
    // - - parameter

    // receive
    // - - current parameter
    // - - parameter

    // send parameter
    // if (parameterValuesChangeDetect[currentParameter] != parameterValues[currentParameter])
    // {
    // check for changes

    // parameterValuesChangeDetect[currentParameter] = parameterValues[currentParameter];

    // send message
    if (api_active & message != "")
    {
      // switch
      if (message == "activateParameter")
      {
        Serial.println("currentParameter|| " + String(currentParameter));
        Serial.println("currentName|| " + String(parameterNames[currentParameter]));
        Serial.println("currentValue|| " + String(parameterValues[currentParameter]));
        Serial.println("parameterValueRangeMin|| " + String(parameterValueRange[currentParameter][0]));
        Serial.println("parameterValueRangeMax|| " + String(parameterValueRange[currentParameter][1]));

        return;
      }
      else if (message == "updateValue")
      {
        // check for changes
        if (parameterValuePrev != parameterValues[currentParameter])
        {
          // send message
          Serial.println("currentValue|| " + String(parameterValues[currentParameter]));

          // set previous value
          parameterValuePrev = parameterValues[currentParameter];
        }

        return;
      }
      else if (message == "disableParameter")
      {
        Serial.println("currentParameter||-1");
        return;
      }
    }
    // }

    // // // // receive parameter
    while (Serial.available() > 0)
    {                                                       // Take out strings until Serial is empty
      String receivedString = Serial.readStringUntil('\0'); // From 1.9.0 version, We can use '\0' as delimiter in Arduino Serial
      receivedData = getMessage(receivedString);
    }

    // Serial.print(receivedData.message);
    // Serial.print(receivedData.value);

    // switch
    if (receivedData.message == "currentValue")
    {
      if (parameterValues[currentParameter] == 4)
      {
        Serial.println("custom parameter");
        // set parameter to value of selected parameter
        parameterValues[4] = receivedData.value.toInt();
      }
    }
    else if (receivedData.message == "currentParameter")
    {
      // set current parameter iPad
      currentParameter = receivedData.value.toInt();

      //
      if (currentParameter == 4)
      {
        // reset
        currentEncoderVal[3] = 0; // startingpoint
        currentEncoderVal[4] = 0; // rotations
      }
    }
    else if (receivedData.message == "parameterValueRangeMin")
    {
      // set parameter value range min iPad
      parameterValueRange[currentParameter][0] = receivedData.value.toInt();
    }
    else if (receivedData.message == "parameterValueRangeMax")
    {
      // set parameter value range max iPad
      parameterValueRange[currentParameter][1] = receivedData.value.toInt();

      // calculate size of range
      int range = parameterValueRange[currentParameter][1] - parameterValueRange[currentParameter][0];

      if (range <= 20)
      {
        parameterValueRange[currentParameter][2] = 20;
      }
      else if (range <= 40)
      {
        parameterValueRange[currentParameter][2] = 40;
      }
      else if (range <= 80)
      {
        parameterValueRange[currentParameter][2] = 60;
      }
    }
    // else if (receivedData.message == "parameterValueRange")
    // {
    //   // set parameter value range iPad
    //   parameterValueRange[currentParameter][0] = receivedData.value.substring(0, receivedData.value.indexOf(",")).toInt();
    //   parameterValueRange[currentParameter][1] = receivedData.value.substring(receivedData.value.indexOf(",") + 1).toInt();
    // }
    // else if (receivedData.message == "parameterValue")
    // {
    //   // set parameter value iPad
    //   parameterValues[currentParameter] = receivedData.value.toInt();
    // }
    // else if (receivedData.message == "parameterName")
    // {
    //   // set parameter name iPad
    //   parameterNames[currentParameter] = receivedData.value;
    // }
    // else if (receivedData.message == "parameter")
    // {
    //   // set parameter iPad
    //   parameterValues[receivedData.value.toInt()] = receivedData.value.substring(receivedData.value.indexOf(",") + 1).toInt();
    // }
    // else if (receivedData.message == "activateParameter")
    // {
    //   // activate parameter iPad
    //   activateParameter(receivedData.value.toInt());
    // }
    // else if (receivedData.message == "disableParameter")
    // {
    //   // disable parameter iPad
    //   activateParameter(-1);
    // }
    // else if (receivedData.message == "updateValue")
    // {
    //   // update value iPad
    //   parameterValues[receivedData.value.toInt()] = receivedData.value.substring(receivedData.value.indexOf(",") + 1).toInt();
    // }
  }
};

Parameters parameters;

void setup(void)
{
  Serial.begin(9600);
  pinMode(DIR_PIN, OUTPUT);
  Wire.begin();
  

  // set Pin Modes
  for (int i = 0; i < parameters.numberOfParameters; i++)
  {
    pinMode(parameters.displaySelect[i], OUTPUT);
    pinMode(parameters.displayButtonPins[i], INPUT);


    // set pull-up resistor
    parameters.selectDisplay(i);
    u8g2.begin();
  }

  parameters.disableParametersAll();
}

void loop(void)
{
  // read out sensors
  for (int i = 0; i < parameters.numberOfParameters; i++)
  {
    parameters.buttonPress(i);
  }

  parameters.api();

  parameters.readParameter();
  parameters.drawParameter();

  // debug
  if (false)
  {
    Serial.print("c = " + String(parameters.currentParameter) + "\t");
    Serial.print("v = " + String(ams5600.getRawAngle()) + "   \t");

    if (parameters.currentParameter != -1)
    {
      Serial.print(
          "par = \t" + String(parameters.parameterValueRange[parameters.currentParameter][0]) + // min
          " \t" + String(parameters.parameterValues[parameters.currentParameter]) +
          "\t" + String(parameters.parameterValueRange[parameters.currentParameter][1]));
    }
  }

  // deley between each page
  delay(80);
}