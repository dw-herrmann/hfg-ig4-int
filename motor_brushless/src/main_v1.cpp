// #include <Arduino.h>
// const byte in_A = 2;
// const byte in_B = 3;
// const byte in_C = 4;

// const byte en_A = 8;
// const byte en_B = 9;
// const byte en_C = 10;

// int speed_motor = 6;
// int rotari = 0;
// int step = 0;

// #define rotari_pin A1

// void setup()
// {
//   pinMode(en_A, OUTPUT);
//   pinMode(in_A, OUTPUT);
//   pinMode(en_B, OUTPUT);
//   pinMode(in_B, OUTPUT);
//   pinMode(en_C, OUTPUT);
//   pinMode(in_C, OUTPUT);

//   pinMode(rotari_pin, INPUT);

//   // start serial port at 9600 bps:
//   Serial.begin(9600);
//   while (!Serial)
//   {
//     ; // wait for serial port to connect. Needed for native USB port only
//   }
// }

// void motor_run(int step)
// {
//   delay(speed_motor / 5); // Phase 1

//   switch (step)
//   {
//   case 1:
//     digitalWrite(en_A, 1);
//     digitalWrite(in_A, 0);
//     digitalWrite(en_B, 0);
//     digitalWrite(in_B, 0);
//     digitalWrite(en_C, 1);
//     digitalWrite(in_C, 1);
//     break;

//   case 2:
//     digitalWrite(en_A, 1);
//     digitalWrite(in_A, 0);
//     digitalWrite(en_B, 1);
//     digitalWrite(in_B, 1);
//     digitalWrite(en_C, 0);
//     digitalWrite(in_C, 0);
//     break;

//   case 3:
//     digitalWrite(en_A, 0);
//     digitalWrite(in_A, 0);
//     digitalWrite(en_B, 1);
//     digitalWrite(in_B, 1);
//     digitalWrite(en_C, 1);
//     digitalWrite(in_C, 0);
//     break;

//   case 4:
//     digitalWrite(en_A, 1);
//     digitalWrite(in_A, 1);
//     digitalWrite(en_B, 0);
//     digitalWrite(in_B, 0);
//     digitalWrite(en_C, 1);
//     digitalWrite(in_C, 0);
//     break;

//   case 5:
//     digitalWrite(en_A, 1);
//     digitalWrite(in_A, 1);
//     digitalWrite(en_B, 1);
//     digitalWrite(in_B, 0);
//     digitalWrite(en_C, 0);
//     digitalWrite(in_C, 0);
//     break;

//   case 6:
//     digitalWrite(en_A, 0);
//     digitalWrite(in_A, 0);
//     digitalWrite(en_B, 1);
//     digitalWrite(in_B, 0);
//     digitalWrite(en_C, 1);
//     digitalWrite(in_C, 1);
//     break;

//   default:
//     break;
//   }
// }

// void loop()
// {
//   rotari = analogRead(rotari_pin);
//   speed_motor = map(rotari, 0, 1023, 20, 2);

//   motor_run(step);
//   step++;

//   // for (int i = 0; i <= (21 - speed_motor) * 5; i++)
//   // {
//   //   motor_run();
//   // }

//   if (step >= 6)
//   {
//     step = 0;
//   }

//   Serial.println("rotari = " + String(rotari) + "\tspeed " + String(speed_motor));
// }
