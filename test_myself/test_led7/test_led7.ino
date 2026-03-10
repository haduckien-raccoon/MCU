// // #define DATA 11
// // #define CLOCK 13
// // #define LATCH 10

// // #define DIGIT1 8
// // #define DIGIT2 9

// // byte numbers[10] = {
// //   0b00111111, //0
// //   0b00000110, //1
// //   0b01011011, //2
// //   0b01001111, //3
// //   0b01100110, //4
// //   0b01101101, //5
// //   0b01111101, //6
// //   0b00000111, //7
// //   0b01111111, //8
// //   0b01101111  //9
// // };

// // int counter = 0;

// // void setup() {
// //   pinMode(DATA, OUTPUT);
// //   pinMode(CLOCK, OUTPUT);
// //   pinMode(LATCH, OUTPUT);

// //   pinMode(DIGIT1, OUTPUT);
// //   pinMode(DIGIT2, OUTPUT);
// // }

// // void displayDigit(byte num, int digitPin) {

// //   digitalWrite(DIGIT1, HIGH);
// //   digitalWrite(DIGIT2, HIGH);

// //   digitalWrite(LATCH, LOW);
// //   shiftOut(DATA, CLOCK, MSBFIRST, numbers[num]);
// //   digitalWrite(LATCH, HIGH);

// //   digitalWrite(digitPin, LOW); // bật digit
// //   delay(2);
// // }

// // void loop() {

// //   int tens = counter / 10;
// //   int units = counter % 10;

// //   displayDigit(tens, DIGIT1);
// //   displayDigit(units, DIGIT2);

// //   static unsigned long lastUpdate = 0;

// //   if(millis() - lastUpdate >= 1000) {
// //     counter++;
// //     if(counter > 99) counter = 0;
// //     lastUpdate = millis();
// //   }
// // }
// #define DATA 11
// #define CLOCK 12
// #define LATCH 10

// #define DIGIT1 4
// #define DIGIT2 5

// byte numbers[10] = {
//   0b11000000,
//   0b11111001,
//   0b10100100,
//   0b10110000,
//   0b10011001,
//   0b10010010,
//   0b10000010,
//   0b11111000,
//   0b10000000,
//   0b10010000
// };

// int counter = 0;
// unsigned long lastUpdate = 0;

// void displayDigit(byte num, int digitPin) {

//   digitalWrite(DIGIT1, LOW);
//   digitalWrite(DIGIT2, LOW);

//   digitalWrite(LATCH, LOW);
//   shiftOut(DATA, CLOCK, MSBFIRST, numbers[num]);
//   digitalWrite(LATCH, HIGH);

//   digitalWrite(digitPin, HIGH);

//   delay(2);
// }

// void setup() {

//   pinMode(DATA, OUTPUT);
//   pinMode(CLOCK, OUTPUT);
//   pinMode(LATCH, OUTPUT);

//   pinMode(DIGIT1, OUTPUT);
//   pinMode(DIGIT2, OUTPUT);
// }

// void loop() {

//   int tens = counter / 10;
//   int units = counter % 10;

//   displayDigit(tens, DIGIT1);
//   displayDigit(units, DIGIT2);

//   if(millis() - lastUpdate >= 1000) {
//     counter++;
//     if(counter > 99) counter = 0;
//     lastUpdate = millis();
//   }
// }
#define LED_BUILTIN 5  // Thử 2 trước, nếu không sáng đổi sang 5

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // Bật LED
  delay(500);                       // 0.5 giây
  digitalWrite(LED_BUILTIN, LOW);   // Tắt LED
  delay(500);                       // 0.5 giây
}
