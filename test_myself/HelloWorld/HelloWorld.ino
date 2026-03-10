#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Khai báo LCD I2C (địa chỉ, số cột, số hàng)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();        // Khởi tạo LCD
  lcd.backlight();   // Bật đèn nền

  lcd.setCursor(0, 0);
  lcd.print("Hello, world!");
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(millis() / 1000);
  lcd.print(" s   "); // Xóa ký tự thừa
}