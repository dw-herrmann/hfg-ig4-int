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
  // -1 = no parameter selected
  // 0-3 = fast parameters
  // 4 = parameters or settings on the screen

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
  int parameterValues[5] = {
      12,
      20,
      5,
      20,
      0};
  int parameterValuePrev = 0;

  // int parameterValuesChangeDetect[4] = {0, 0, 0, 0};

  // parameter value range, {min, max, steps per dial rotation}
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
      api("disableParameter");
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

  void selectDisplay(int selectedDisplay)
  {
    for (int i = 0; i < numberOfParameters; i++)
    {
      digitalWrite(displaySelect[i], display_array[selectedDisplay][i]);
    }
  };

  void readParameter()
  {
    // if parameter is active, read value from encoder
    if (currentParameter != -1)
    {
      // read encoder value
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

      // print all values
      // Serial.println("dr=\t" + String(direction) + "\tds=\t" + String(distance) + "\tst=\t" + String(steps) + "\tvl=\t" + String(parameterValues[currentParameter]) + "\trt=\t" + String(currentEncoderVal[3]));

      // // // // // limit
      // if value is out of range, set to min/max
      if (parameterValues[currentParameter] < parameterValueRange[currentParameter][0]) // min
      {
        // set
        parameterValues[currentParameter] = parameterValueRange[currentParameter][0];
      }
      else if (parameterValues[currentParameter] > parameterValueRange[currentParameter][1]) // max
      {
        parameterValues[currentParameter] = parameterValueRange[currentParameter][1];
      }

      // if (parameterValues[currentParameter] > parameterValueRange[currentParameter][1]) // if above max
      // {
      //   // set current back to max
      //   parameterValues[currentParameter] = parameterValueRange[currentParameter][1];
      //   // set first on press to current - full range
      //   currentEncoderVal[2] = currentEncoderVal[0] - (parameterValueRange[currentParameter][1] * stepSize);
      //   // reset rotations
      //   // int fullRange = (parameterValueRange[currentParameter][1] - parameterValueRange[currentParameter][0]) * stepSize;
      //   // currentEncoderVal[3] = fullRange / parameterValueRange[currentParameter][2];
      // }
      // else if (parameterValues[currentParameter] < parameterValueRange[currentParameter][0]) // if below min
      // {
      //   parameterValues[currentParameter] = parameterValueRange[currentParameter][0]; // set to min
      //   // currentEncoderVal[2] = currentEncoderVal[0];                                  // set first on press to current
      //   // currentEncoderVal[3] = 0;                                                     // reset rotations

      //   // print
      // }

      // api
      api("updateCurrentValue");
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
    // if parameter has a display
    if (display < 4)
    {
      // if display was selected
      if (display != -1)
      {
        // select display
        selectDisplay(display);

        // and write text
        if (state) // if active
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
      else if (currentParameter != -1)
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
  }

  // string of received message
  char receivedMessage[100] = "";

  void api(
      String message = "" /* message to send */,
      String value = "" /* value to send */)
  {

    // SEND
    if (api_active & message != "")
    {
      // switch
      if (message == "activateParameter") // start interaction with displays and dial
      {
        Serial.println("currentParameter|| " + String(currentParameter));
        Serial.println("currentName|| " + String(parameterNames[currentParameter]));
        Serial.println("parameterValueRangeMin|| " + String(parameterValueRange[currentParameter][0]));
        Serial.println("parameterValueRangeMax|| " + String(parameterValueRange[currentParameter][1]));
        Serial.println("setupCurrentValue|| " + String(parameterValues[currentParameter]));

        // change encoder first value to fit slider value
        int stepSize = 4096 / parameterValueRange[currentParameter][2];
        currentEncoderVal[2] = currentEncoderVal[0] - (parameterValues[currentParameter] * stepSize);

        return;
      }
      else if (message == "updateCurrentValue") // update slider, if change detected
      {
        // check for changes

        if (parameterValues[currentParameter] != parameterValuePrev)
        {
          // send message
          Serial.println("updateCurrentValue|| " + String(parameterValues[currentParameter]));

          // set previous value
          parameterValuePrev = parameterValues[currentParameter];
        }

        return;
      }
      else if (message == "disableParameter") // stop interaction with displays and dial
      {
        Serial.println("reset");
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

    // RECEIVE

    // … change name, range, value usw. to new or no parameter, …
    if (receivedData.message == "setupCurrentParameter")
    {
      // set current parameter iPad
      currentParameter = 4;

      // reset …
      currentEncoderVal[3] = 0; // … startingpoint
      currentEncoderVal[4] = 0; // … rotations
    }

    // … get min
    else if (receivedData.message == "parameterValueRangeMin")
    {
      parameterValueRange[currentParameter][0] = receivedData.value.toInt();
    }

    // … get max
    else if (receivedData.message == "parameterValueRangeMax")
    {
      // set parameter value range max iPad
      parameterValueRange[currentParameter][1] = receivedData.value.toInt();
    }

    // setup current value to variable, …
    else if (receivedData.message == "setupCurrentValue")
    {
      if (currentParameter == 4)
      {
        Serial.println("custom parameter");
        // set parameter to value of selected parameter
        parameterValues[4] = receivedData.value.toInt();
      }
    }

    // disable current parameter, …
    else if (receivedData.message == "stopCurrentParameter")
    {
      // set current parameter iPad
      currentParameter = -1;
    }

    // update current value to variable, …
    else if (receivedData.message == "updateCurrentValue")
    {
      // set parameter to value of selected parameter
      parameterValues[currentParameter] = receivedData.value.toInt();
    }
  }
};

Parameters parameters;

void setup(void)
{
  Serial.begin(9600);

  pinMode(DIR_PIN, OUTPUT);
  Wire.begin();

  // set Pin Modes
  for (int i = 0; i < parameters.numberOfDisplays; i++)
  // for (int i = 1; i < parameters.numberOfDisplays; i++)
  {
    pinMode(parameters.displaySelect[i], OUTPUT);
    pinMode(parameters.displayButtonPins[i], INPUT);

    // set pull-up resistor
    parameters.selectDisplay(i);
    Serial.print("display #" + String(i));
    u8g2.begin();
    Serial.println(" ready");
  }

  parameters.disableParametersAll();
  // done
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