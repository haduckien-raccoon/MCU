const uint8_t leds[] = {4, 5, 6, 7, 8, 9, 10, 11};
const uint8_t NUM_LED = 8;

volatile uint8_t mode = 0; 

// ===== Chống nảy =====
const unsigned long DEBOUNCE_TIME = 200;
volatile unsigned long lastPressA = 0;
volatile unsigned long lastPressB = 0;

// ===== Đếm số lần nhấn =====
volatile unsigned int countA = 0;
volatile unsigned int countB = 0;

void clearLeds() {
  for (uint8_t i = 0; i < NUM_LED; i++) {
    digitalWrite(leds[i], LOW);
  }
}

void runEffectA() {
  // Serial.print("Cau A da nhan: ");
  // Serial.println(countA);

  for (int i = 0; i < NUM_LED + 2; i++) {
    clearLeds();

    if (i < NUM_LED) digitalWrite(leds[i], HIGH);
    if (i - 1 >= 0 && i - 1 < NUM_LED) digitalWrite(leds[i - 1], HIGH);
    if (i - 2 >= 0 && i - 2 < NUM_LED) digitalWrite(leds[i - 2], HIGH);

    delay(300);
  }

  clearLeds();
  mode = 0;
}

void runEffectB() {
  // Serial.print("Cau B da nhan: ");
  // Serial.println(countB);

  for (uint8_t i = 0; i < NUM_LED; i++) {
    clearLeds();
    for (uint8_t j = 0; j <= i; j++) {
      digitalWrite(leds[j], HIGH);
    }
    delay(300);
  }

  for (int8_t i = NUM_LED - 1; i >= 0; i--) {
    clearLeds();
    for (uint8_t j = 0; j <= i; j++) {
      digitalWrite(leds[j], HIGH);
    }
    delay(300);
  }

  clearLeds();
  mode = 0;
}

void CauA() {
  unsigned long now = millis();
  if (now - lastPressA >= DEBOUNCE_TIME) {
    lastPressA = now;
    countA++;
    Serial.print("Cau A da nhan: ");
    Serial.println(countA);
    mode = 1;
  }
}

void CauB() {
  unsigned long now = millis();
  if (now - lastPressB >= DEBOUNCE_TIME) {
    lastPressB = now;
    countB++;
    Serial.print("Cau B da nhan: ");
  Serial.println(countB);
    mode = 2;
  }
}

void setup() {
  Serial.begin(9600);

  for (uint8_t i = 0; i < NUM_LED; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  pinMode(20, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(20), CauA, FALLING);
  attachInterrupt(digitalPinToInterrupt(21), CauB, FALLING);

  Serial.println("He thong san sang - Cho nhan nut");
}

void loop() {
  if (mode == 0) {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
      Serial.println("Toi cho ban nhan nut ...");
      lastPrint = millis();
    }
  }
  else if (mode == 1) {
    Serial.println("Dang chay Cau A...");
    runEffectA();
  }
  else if (mode == 2) {
    Serial.println("Dang chay Cau B...");
    runEffectB();
  }
}
