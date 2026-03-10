void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);
// 0x27 is the I2C address. This address might be different.

const int IN_A0 = A0; // analog input
const int IN_D0 = 8; // digital input

void setup() {
  pinMode (IN_A0, INPUT);
  pinMode (IN_D0, INPUT);
  lcd.begin();
    lcd.backlight();
 
  lcd.begin ();
}

int value_A0;
bool value_D0;

void loop() {

  value_A0 = analogRead(IN_A0); // reads the analog input from the IR distance sensor
  value_D0 = digitalRead(IN_D0);// reads the digital input from the IR distance sensor
  
  lcd.setCursor(0, 0); // sets the cursor of the LCD module to the first line
  lcd.print("A0:");
  lcd.setCursor(3, 0); // sets the cursor of the LCD module to the fourth character
  lcd.print(value_A0); // prints analog value on the LCD module
  
  lcd.setCursor(0, 1); // sets the cursor of the LCD module to the first line
  lcd.print("D0:");
  lcd.setCursor(3, 1); // sets the cursor of the LCD module to the fourth character
  lcd.print(value_D0); // prints digital value on the LCD module
  
  delay(1000);
}
