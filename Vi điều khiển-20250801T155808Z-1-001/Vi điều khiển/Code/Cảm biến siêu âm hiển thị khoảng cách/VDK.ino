#include <LiquidCrystal.h>
// LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Sensor
const int pingPin = 7;
// khai báo biến nhận sóng ??
long duration, cm;

void setup() {
  lcd.begin(16, 2);
}

void loop() {

  // khai báo chân ra
  pinMode(pingPin, OUTPUT);
  
  // phát sóng thấp để đảm bảo không có sóng cao đang phát
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  
  // phát sóng cao
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  
  // phát lại sóng thấp để ngưng
  digitalWrite(pingPin, LOW);
  
  // khai báo chân vào
  pinMode(pingPin, INPUT);
  
  // nhận sóng cao
  duration = pulseIn(pingPin, HIGH);

  // tính khoảng cách
  cm = microsecondsToCentimeters(duration);

  // in ra LCD
  lcd.setCursor(0,0);
  lcd.print("    ");
  lcd.setCursor(0,0);
  lcd.print(cm);
  delay(100);
}


long microsecondsToCentimeters(long microseconds) {  
  
  // Tốc độ của âm thanh là 340 m/s hoặc đi 1 cm hết 29 ms
  // quãng đường là hai lần khoảng cách (đi/về)
  return microseconds / 29 / 2;
}
 