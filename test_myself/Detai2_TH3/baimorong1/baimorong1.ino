#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  for (int i = 0; i <= 16; i++) {
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.print("hello world");
    delay(300);
  }
}
