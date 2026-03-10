#include <LiquidCrystal_I2C.h>

#define ENA 9
#define IN1 22
#define IN2 24
#define ENCODER 2
#define JOY A0

LiquidCrystal_I2C lcd(0x27,16,2);

volatile int pulseCount = 0;
unsigned long lastTime = 0;
float rpm = 0;

void countPulse() {
  pulseCount++;
}

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENCODER, INPUT);

  attachInterrupt(digitalPinToInterrupt(ENCODER), countPulse, RISING);

  lcd.init();
  lcd.backlight();

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void loop() {

  int joyValue = analogRead(JOY);
  int pwm = map(joyValue, 0, 1023, 0, 255);
  analogWrite(ENA, pwm);

  if(millis() - lastTime >= 1000) {

    noInterrupts();
    int count = pulseCount;
    pulseCount = 0;
    interrupts();

    rpm = (count / 20.0) * 60.0;

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("RPM: ");
    lcd.print(rpm);

    lastTime = millis();
  }
}
