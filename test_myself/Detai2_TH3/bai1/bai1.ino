#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Send text...");
}

void loop() {
  if (Serial.available()) {
    lcd.clear();
    lcd.setCursor(0,0);

    while (Serial.available()) {
      char c = Serial.read();
      lcd.print(c);
      delay(10);
    }
  }
}
