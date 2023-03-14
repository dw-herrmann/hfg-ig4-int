// #include <Arduino.h>
// #include <U8g2lib.h>
// U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// bool api_active = true;

// #include <String.h>

// #ifdef U8X8_HAVE_HW_SPI
// #include <SPI.h>
// #endif
// #ifdef U8X8_HAVE_HW_I2C
// #include <Wire.h>
// #endif

// #include <AS5600.h>
// AMS_5600 ams5600;
// #define DIR_PIN 2

// struct MessageValue
// {
//   String message;
//   String value;
// };

// struct MessageValue getMessage(String inputtedStr)
// {
//   struct MessageValue result;

//   char charArr[50];
//   inputtedStr.toCharArray(charArr, 50);
//   char *ptr = strtok(charArr, "||");
//   result.message = String(ptr);
//   ptr = strtok(NULL, "||");

//   if (ptr == NULL)
//   {
//     result.value = String("");
//     return result;
//   }

//   result.value = String(ptr);

//   return result;
// }

// int firstLED = 3;
// int secondLED = 5;
// int thirdLED = 6;
// struct MessageValue receivedData;

// // U8G2_SH1106_128X64_NONAME_F_HW_I2C(rotation, [reset [, clock, data]]) [full framebuffer, size = 1024 bytes]

// class Parameters
// {
// public:
//   // dial
//   int dial_button = 39;
//   // current, before, first on button press, rotations
//   int currentEncoderVal[5] = {0, 0, 0, 0, 0};

//   // currently selected parameter
//   int currentParameter = -1;
//   // -1 = no parameter selected
//   // 0-3 = fast parameters
//   // 4 = parameters or settings on the screen

//   int valOnClick = 0;

//   // number of parameters and displays
//   int numberOfButtons = 5;
//   int numberOfParameters = 5;
//   int numberOfDisplays = 4;

//   // names of parameters
//   char *parameterNames[5] = {
//       "Frequenz",
//       "Phoch",
//       "PEEP",
//       "FiO2",
//       "",
//   };

//   // values of parameters
//   int parameterValues[5] = {
//       12,
//       20,
//       5,
//       20,
//       0};
//   int parameterValuePrev = 0;

//   // int parameterValuesChangeDetect[4] = {0, 0, 0, 0};

//   // parameter value range, {min, max, steps per dial rotation}
//   int parameterValueRange[5][3] = {
//       {1, 60, 60},   // Frequenz
//       {6, 60, 60},   // Phoch
//       {0, 19, 20},   // PEEP
//       {21, 100, 60}, // FiO2
//       {0, 0, 60},    // custom
//   };

//   // display of parameter
//   int display_array[4][4] = {
//       {1, 0, 0, 0},
//       {0, 1, 0, 0},
//       {0, 0, 1, 0},
//       {0, 0, 0, 1}};

//   // selection pins
//   int displaySelect[4] = {23, 27, 31, 35};

//   // button pins
//   int displayButtonPins[5] = {22, 26, 30, 34, dial_button};
//   int displayButtonStates[5] = {0, 0, 0, 0, 0};

//   // selecting a parameter to edit
//   void activateParameter(int selectedParameter)
//   {

//     // if selected parameter is already active
//     // if current != -1
//     if (currentParameter == selectedParameter)
//     {
//       // deactivate parameter
//       disableParametersAll();
//       currentParameter = -1;
//       api_send("ap_stop");
//       return;
//     }
//     // if parameter is not active
//     else if (selectedParameter != 4)
//     {
//       currentParameter = selectedParameter;

//       // (re)set all encoder values
//       currentEncoderVal[0] = ams5600.getRawAngle();
//       currentEncoderVal[1] = ams5600.getRawAngle();
//       currentEncoderVal[2] = ams5600.getRawAngle();
//       currentEncoderVal[3] = 0;

//       valOnClick = parameterValues[selectedParameter];
//       disableParametersAll(selectedParameter);

//       api_send("ap_start");
//       return;
//     }

//     if (selectedParameter != currentParameter)
//     {
//     }
//   };

//   void disableParametersAll(int except = -1)
//   {
//     for (int i = 0; i < numberOfDisplays; i++)
//     {
//       if (except != -1 && i == except)
//       {
//         drawParameter(i, true);
//       }
//       else
//       {
//         drawParameter(i, false);
//       }
//     }
//   }

//   void selectDisplay(int selectedDisplay)
//   {
//     for (int i = 0; i < numberOfParameters; i++)
//     {
//       digitalWrite(displaySelect[i], display_array[selectedDisplay][i]);
//     }
//   };

//   void readParameter()
//   {
//     // if parameter is active, read value from encoder
//     if (currentParameter != -1)
//     {
//       // read encoder value
//       currentEncoderVal[0] = ams5600.getRawAngle();

//       // // // // direction
//       // get direction and set "before" for next loop
//       int direction = currentEncoderVal[0] - currentEncoderVal[1];
//       currentEncoderVal[1] = currentEncoderVal[0];

//       // // // // rotations
//       // if turned above threshhold of 0/2048
//       if (direction > 2048)
//       {
//         // make rotation -1
//         currentEncoderVal[3] -= 1;
//       }
//       else if (direction < -2048)
//       {
//         // make rotation +1
//         currentEncoderVal[3] += 1;
//       }

//       // // // // distance
//       // get distance from "first on button press" to now
//       int distance =
//           // current value
//           (currentEncoderVal[0]
//            // + rotations
//            + (currentEncoderVal[3] * 4096))
//           // - "first on button press"
//           - (currentEncoderVal[2]);

//       // // // // value
//       int stepSize = 4096 / parameterValueRange[currentParameter][2];
//       int steps = distance / stepSize;

//       parameterValues[currentParameter] = valOnClick + steps;

//       // // // // // limit
//       // if value is out of range, set to min/max
//       if (parameterValues[currentParameter] < parameterValueRange[currentParameter][0]) // min
//       {
//         // set
//         parameterValues[currentParameter] = parameterValueRange[currentParameter][0];
//       }
//       else if (parameterValues[currentParameter] > parameterValueRange[currentParameter][1]) // max
//       {
//         parameterValues[currentParameter] = parameterValueRange[currentParameter][1];
//       }

//       // api
//       api_send("ap_update");
//       return;
//     }
//   }

//   // button pressed
//   void buttonPress(int button)
//   {
//     if (
//         digitalRead(displayButtonPins[button]) != displayButtonStates[button])
//     {
//       // if clicked
//       if (
//           digitalRead(displayButtonPins[button]) == 1)
//       {
//         displayButtonStates[button] = digitalRead(displayButtonPins[button]);
//         Serial.println(">> " + String(button) + " pressed " + String(currentParameter) + "active");

//         activateParameter(button);
//       }
//       else
//       {
//         // recognize change
//         displayButtonStates[button] = digitalRead(displayButtonPins[button]);

//         Serial.println(">> " + String(button) + " released");
//       }
//     }
//   }

//   void
//   drawParameter(int display = -1, bool state = false)
//   {
//     // if parameter has a display
//     if (display < 4)
//     {
//       // if display was selected
//       if (display != -1)
//       {
//         // select display
//         selectDisplay(display);

//         // and write text
//         if (state) // if active
//         {
//           u8g2.clearBuffer();
//           // set white background
//           u8g2.setDrawColor(1);
//           u8g2.drawBox(0, 0, 128, 64);
//           // set black text
//           u8g2.setDrawColor(0);
//         }
//         else
//         {
//           u8g2.clearBuffer();
//           // set black background
//           u8g2.setDrawColor(0);
//           u8g2.drawBox(0, 0, 128, 64);
//           // set white text
//           u8g2.setDrawColor(1);
//         }

//         u8g2.setFont(u8g2_font_logisoso16_tf);
//         u8g2.setFontPosTop();
//         u8g2.drawStr(3, 43, parameterNames[display]);
//         u8g2.setFont(u8g2_font_logisoso34_tf);
//         u8g2.setFontPosTop();
//         u8g2.drawStr(2, 3, String(parameterValues[display]).c_str());
//         u8g2.sendBuffer();
//         return;
//       }
//       // draw current parameter
//       else if (currentParameter != -1)
//       {
//         selectDisplay(currentParameter);

//         u8g2.clearBuffer();
//         // set white background
//         u8g2.setDrawColor(1);
//         u8g2.drawBox(0, 0, 128, 64);
//         // set black text
//         u8g2.setDrawColor(0);
//         u8g2.setFont(u8g2_font_logisoso16_tf);
//         u8g2.setFontPosTop();
//         u8g2.drawStr(3, 43, parameterNames[currentParameter]);
//         u8g2.setFont(u8g2_font_logisoso34_tf);
//         u8g2.setFontPosTop();
//         u8g2.drawStr(2, 3, String(parameterValues[currentParameter]).c_str());
//         u8g2.sendBuffer();
//         return;
//       }
//     };
//   }

//   void api_send(
//       String message = "" /* message to send */,
//       String value = "" /* value to send */)
//   {

//     // SEND
//     if (message == "ap_start")
//     {
//       Serial.println("ap_start");

//       return;
//     }

//     if (message == "ap_stop")
//     {
//       Serial.println("ap_stop");

//       return;
//     }

//     if (message == "ap_update")
//     {
//       // if value has changed
//       if (parameterValues[currentParameter] != parameterValuePrev)
//       {
//         Serial.println("ap_update||" + String(parameterValues[currentParameter]));
//         parameterValuePrev = parameterValues[currentParameter];
//       }

//       return;
//     }
//   }

//   void api_receive()
//   {
//     // RECEIVE
//     if (receivedData.message == "pa_start")
//     {

//       return;
//     }

//     if (receivedData.message == "pa_stop")
//     {

//       return;
//     }

//     if (receivedData.message == "pa_update")
//     {

//       return;
//     }
//   }
// };

// Parameters parameters;

// void setup(void)
// {
//   Serial.begin(9600);

//   pinMode(DIR_PIN, OUTPUT);
//   Wire.begin();

//   // set Pin Modes
//   for (int i = 0; i < parameters.numberOfDisplays; i++)
//   // for (int i = 1; i < parameters.numberOfDisplays; i++)
//   {
//     pinMode(parameters.displaySelect[i], OUTPUT);
//     pinMode(parameters.displayButtonPins[i], INPUT);

//     // set pull-up resistor
//     parameters.selectDisplay(i);
//     Serial.print("display #" + String(i));
//     u8g2.begin();
//     Serial.println(" ready");
//   }

//   parameters.disableParametersAll();
//   // done
// }

// void loop(void)
// {
//   // read out sensors
//   for (int i = 0; i < parameters.numberOfParameters; i++)
//   {
//     parameters.buttonPress(i);
//   }

//   parameters.api_send();

//   parameters.readParameter();
//   parameters.drawParameter();

//   // debug
//   if (false)
//   {
//     Serial.print("c = " + String(parameters.currentParameter) + "\t");
//     Serial.print("v = " + String(ams5600.getRawAngle()) + "   \t");

//     if (parameters.currentParameter != -1)
//     {
//       Serial.print(
//           "par = \t" + String(parameters.parameterValueRange[parameters.currentParameter][0]) + // min
//           " \t" + String(parameters.parameterValues[parameters.currentParameter]) +
//           "\t" + String(parameters.parameterValueRange[parameters.currentParameter][1]));
//     }
//   }

//   // deley between each page
//   delay(80);
// }