#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int btn[4] = {6, 7, 8, 9};
int led[4] = {2, 3, 4, 5};

void setup() {
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < 4; i++) {
    pinMode(btn[i], INPUT_PULLUP);
    pinMode(led[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < 4; i++) {
    if (digitalRead(btn[i]) == LOW) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("func");
      lcd.print(i + 1);

      // hiệu ứng LED
      for (int j = 0; j <= i; j++) {
        digitalWrite(led[j], HIGH);
        delay(200);
      }
      for (int j = 0; j <= i; j++) {
        digitalWrite(led[j], LOW);
      }

      delay(500);
    }
  }
}
