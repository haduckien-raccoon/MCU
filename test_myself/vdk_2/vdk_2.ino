#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

#define IR1_PIN 2  
#define IR2_PIN 3  
#define IR3_PIN 4  

#define MOTOR_ENA 5 
#define MOTOR_IN1 6 
#define MOTOR_IN2 7 

#define SERVO1_PIN 9
#define SERVO2_PIN 10
Servo servo1;
Servo servo2;

int countGreen = 0;
int countRed = 0;
int countUnknown = 0;

int baseR = 86;
int baseG = 150;
int baseB = 147;
#define THRESHOLD_NONE 20
#define THRESHOLD_COLOR 35  

bool isConveyorRunning = false;

void startConveyor() {
  if (!isConveyorRunning) { 
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
    analogWrite(MOTOR_ENA, 250); 
    isConveyorRunning = true;
  }
}

void stopConveyor() {
  if (isConveyorRunning) {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
    analogWrite(MOTOR_ENA, 0);
    isConveyorRunning = false;
  }
}

void updateLCD(String lastDetected) {
  lcd.setCursor(0, 0);
  lcd.print("G:"); lcd.print(countGreen);
  lcd.print(" R:"); lcd.print(countRed);
  lcd.print(" U:"); lcd.print(countUnknown);
  lcd.print("   "); 
  
  lcd.setCursor(0, 1);
  lcd.print("Last: ");
  lcd.print(lastDetected);
  lcd.print("      "); 
}

String detectColor(int r, int g, int b) {
  int dR = r - baseR;
  int dG = g - baseG;
  int dB = b - baseB;

  if (abs(dR) < THRESHOLD_NONE && abs(dG) < THRESHOLD_NONE && abs(dB) < THRESHOLD_NONE) return "NONE";
  if (dR > THRESHOLD_COLOR && dR > dG + 20 && dR > dB + 20) return "RED";
  if (dG > THRESHOLD_COLOR && dG > dR + 20 && dG > dB + 20) return "GREEN";
  
  return "UNKNOWN";
}

bool readColor(int &r, int &g, int &b) {
  Wire.beginTransmission(0x5A);
  Wire.write(0x00);
  if (Wire.endTransmission() != 0) return false;

  Wire.requestFrom(0x5A, 8);
  if (Wire.available() >= 8) {
    r = (Wire.read() << 8) | Wire.read();
    g = (Wire.read() << 8) | Wire.read();
    b = (Wire.read() << 8) | Wire.read();
    int c = (Wire.read() << 8) | Wire.read();
    return true;
  }
  return false;
}

bool readAverageColor(int &avgR, int &avgG, int &avgB, int numSamples = 30) {
  long sumR = 0, sumG = 0, sumB = 0;
  int validSamples = 0;
  int tempR, tempG, tempB;

  for (int i = 0; i < numSamples; i++) {
    if (readColor(tempR, tempG, tempB)) {
      sumR += tempR;
      sumG += tempG;
      sumB += tempB;
      validSamples++;
    }
    delay(10);
  }

  if (validSamples > 0) {
    avgR = sumR / validSamples;
    avgG = sumG / validSamples;
    avgB = sumB / validSamples;
    return true;
  }
  
  return false;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(100000);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Khoi dong...");
  lcd.setCursor(0, 1);
  lcd.print("He thong OK!");
  delay(2000); 
  updateLCD("NONE"); 

  pinMode(IR1_PIN, INPUT_PULLUP);
  pinMode(IR2_PIN, INPUT_PULLUP);
  pinMode(IR3_PIN, INPUT_PULLUP);

  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(75);
  servo2.write(75);

  Serial.println("He thong san sang!");
}

void loop() {
  startConveyor();

  if (digitalRead(IR1_PIN) == LOW) {
    stopConveyor(); 
    delay(500);

    int r, g, b;
    String color = "NONE";

    if (readAverageColor(r, g, b, 30)) {
      color = detectColor(r, g, b);
      
      Serial.print("Avg R: "); Serial.print(r);
      Serial.print(" | G: "); Serial.print(g);
      Serial.print(" | B: "); Serial.print(b);
      Serial.print(" -> "); Serial.println(color);
    }

    if (color == "GREEN") countGreen++;
    else if (color == "RED") countRed++;
    else if (color == "UNKNOWN") countUnknown++;

    updateLCD(color);
    
    delay(200); 

    startConveyor();

    if (color == "GREEN") {
      while(digitalRead(IR2_PIN) == HIGH) { }
      servo1.write(15);
      while(digitalRead(IR2_PIN) == LOW) { }
      servo1.write(75);
    }
    
    else if (color == "RED") {
      while(digitalRead(IR3_PIN) == HIGH) { }
      servo2.write(15);
      while(digitalRead(IR3_PIN) == LOW) { }
      servo2.write(75);
    }
    
    else if (color == "UNKNOWN") {
      while(digitalRead(IR1_PIN) == LOW) { }
      delay(200); 
    }
  }
}