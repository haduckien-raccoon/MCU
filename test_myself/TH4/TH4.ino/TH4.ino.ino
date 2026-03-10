#define NUM 4

// ===== KHAI BÁO CHÂN =====
int ldrPins[NUM] = {A0, A1, A2, A3};
int ledPins[NUM] = {9, 10, 11, 12};

// ===== NGƯỠNG THEO MẠCH THỰC TẾ (0–50) =====
#define TOUCH_THRESHOLD    20
#define RELEASE_THRESHOLD  10

// ===== THỜI GIAN =====
#define DOUBLE_TAP_TIME 400   // ms
#define SWIPE_TIME      800   // ms

// ===== BIẾN TRẠNG THÁI =====
bool ledState[NUM] = {0};
bool touched[NUM]  = {0};

unsigned long lastTouchTime[NUM] = {0};
unsigned long touchOrderTime[NUM] = {0};

// ===== PROTOTYPE =====
void resetSwipe();

void setup() {
  Serial.begin(9600);
  Serial.println("=== LDR + LED DEBUG START ===");

  for (int i = 0; i < NUM; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

void loop() {
  unsigned long now = millis();

  Serial.print("LDR: ");

  // ===== ĐỌC & XỬ LÝ TỪNG QUANG TRỞ =====
  for (int i = 0; i < NUM; i++) {
    int value = analogRead(ldrPins[i]);
    Serial.print(value);
    Serial.print("\t");

    // ===== PHÁT HIỆN CHẠM =====
    if (value > TOUCH_THRESHOLD && !touched[i]) {
      touched[i] = true;

      // DOUBLE TAP → TẮT LED
      if (now - lastTouchTime[i] < DOUBLE_TAP_TIME) {
        ledState[i] = LOW;
        Serial.print("| DOUBLE TAP LDR ");
        Serial.print(i);
        Serial.print(" -> LED OFF ");
      }
      // SINGLE TAP → BẬT LED
      else {
        ledState[i] = HIGH;
        Serial.print("| SINGLE TAP LDR ");
        Serial.print(i);
        Serial.print(" -> LED ON ");
      }

      digitalWrite(ledPins[i], ledState[i]);

      lastTouchTime[i] = now;
      touchOrderTime[i] = now;   // lưu cho quét
    }

    // ===== NHẢ TAY =====
    if (value < RELEASE_THRESHOLD) {
      touched[i] = false;
    }
  }

  Serial.println();

  // ===== PHÁT HIỆN QUÉT TAY =====
  if ( touchOrderTime[0] &&
       touchOrderTime[1] &&
       touchOrderTime[2] &&
       touchOrderTime[3] ) {

    // ---- TRÁI → PHẢI ----
    if ( touchOrderTime[0] < touchOrderTime[1] &&
         touchOrderTime[1] < touchOrderTime[2] &&
         touchOrderTime[2] < touchOrderTime[3] &&
         touchOrderTime[3] - touchOrderTime[0] < SWIPE_TIME ) {

      Serial.println(">>> SWIPE LEFT -> RIGHT : ALL LED ON");

      for (int i = 0; i < NUM; i++) {
        ledState[i] = HIGH;
        digitalWrite(ledPins[i], HIGH);
      }
      resetSwipe();
    }

    // ---- PHẢI → TRÁI ----
    if ( touchOrderTime[3] < touchOrderTime[2] &&
         touchOrderTime[2] < touchOrderTime[1] &&
         touchOrderTime[1] < touchOrderTime[0] &&
         touchOrderTime[0] - touchOrderTime[3] < SWIPE_TIME ) {

      Serial.println("<<< SWIPE RIGHT -> LEFT : ALL LED OFF");

      for (int i = 0; i < NUM; i++) {
        ledState[i] = LOW;
        digitalWrite(ledPins[i], LOW);
      }
      resetSwipe();
    }
  }

  // ===== IN TRẠNG THÁI LED =====
  Serial.print("LED: ");
  for (int i = 0; i < NUM; i++) {
    Serial.print(ledState[i]);
    Serial.print(" ");
  }
  Serial.println("\n-----------------------------");

  delay(50);
}

// ===== RESET DỮ LIỆU QUÉT =====
void resetSwipe() {
  for (int i = 0; i < NUM; i++) {
    touchOrderTime[i] = 0;
  }
}
