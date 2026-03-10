#include <LiquidCrystal.h>
#include <math.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int MOTOR = 9;
const int CONTROLLER = A0;
int input = 0;
int input2 = 0;
float rpm = 0;

void setup() {
  lcd.begin(16, 2);
  pinMode(A0, INPUT);
  Serial.begin(9600);
  pinMode(MOTOR, OUTPUT);
}

void loop() {  
  input = analogRead(CONTROLLER);
  input2 = map(input,0,1023,0,255); // giá trị xung
  analogWrite(MOTOR,input2);
  
  rpm=map(input2,0,255.0,0,7777.0);
  
  lcd.setCursor(0,0);
  lcd.print("RPM:");
  
  lcd.setCursor(5, 0);
  lcd.print(rpm);
  
    
}
 