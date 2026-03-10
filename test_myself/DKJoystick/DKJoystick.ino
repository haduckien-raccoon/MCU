#include <Servo.h>

/* =================================================
   SG90 SERVO – JOYSTICK 2 CHIỀU CHUẨN
   - Trái / Phải đúng nghĩa
   - Thả tay dừng ngay
   - Không trôi – Không giật
================================================= */

/* ================= SERVO ================= */
Servo servoX;
Servo servoY;
Servo servoZ;

/* ================= PIN ================= */
#define JOY_X_PIN A0
#define JOY_Y_PIN A1
#define JOY_SW_PIN 2

#define SERVO_X_PIN 9
#define SERVO_Y_PIN 10
#define SERVO_Z_PIN 11

/* ================= GIỚI HẠN AN TOÀN ================= */
#define X_MIN 40
#define X_MAX 140

#define Y_MIN 30
#define Y_MAX 150

#define Z_OPEN 60
#define Z_CLOSE 120

/* ================= JOYSTICK ================= */
#define JOY_CENTER 512
#define DEADZONE 90

/* ================= CHUYỂN ĐỘNG ================= */
#define MAX_SPEED 2.0        // độ / chu kỳ
#define UPDATE_INTERVAL 20   // ms

/* ================= TRẠNG THÁI ================= */
float angleX = 90;
float angleY = 90;
int angleZ = Z_OPEN;

bool xAttached = false;
bool yAttached = false;

unsigned long lastUpdate = 0;
unsigned long lastButton = 0;

/* ================= SETUP ================= */
void setup() {
  Serial.begin(9600);
  pinMode(JOY_SW_PIN, INPUT_PULLUP);

  servoZ.attach(SERVO_Z_PIN);
  servoZ.write(angleZ);

  Serial.println("SG90 – JOYSTICK 2 CHIEU READY");
}

/* ================= LOOP ================= */
void loop() {
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = millis();

    controlAxis(
      JOY_X_PIN,
      servoX,
      xAttached,
      angleX,
      X_MIN,
      X_MAX,
      SERVO_X_PIN
    );

    controlAxis(
      JOY_Y_PIN,
      servoY,
      yAttached,
      angleY,
      Y_MIN,
      Y_MAX,
      SERVO_Y_PIN
    );
  }

  handleGripper();
}

/* ================= AXIS CONTROL ================= */
void controlAxis(
  int joyPin,
  Servo &servo,
  bool &attached,
  float &angle,
  int minAngle,
  int maxAngle,
  int servoPin
) {
  int raw = analogRead(joyPin);
  int offset = raw - JOY_CENTER;

  /* ===== JOYSTICK VỀ GIỮA → DỪNG ===== */
  if (abs(offset) < DEADZONE) {
    if (attached) {
      servo.write(angle);   // giữ đúng góc hiện tại
      servo.detach();       // ngắt để không rung
      attached = false;
    }
    return;
  }

  /* ===== GẮN SERVO KHI BẮT ĐẦU QUAY ===== */
  if (!attached) {
    servo.attach(servoPin);
    attached = true;
  }

  /* ===== TỐC ĐỘ TỈ LỆ ĐỘ LỆCH ===== */
  float speed = map(
    abs(offset),
    DEADZONE,
    512,
    20,
    MAX_SPEED * 100
  ) / 100.0;

  /* ===== HƯỚNG ===== */
  if (offset < 0) speed = -speed;   // trái / phải

  /* ===== CẬP NHẬT GÓC ===== */
  angle += speed;

  /* ===== GIỚI HẠN CỨNG ===== */
  angle = constrain(angle, minAngle, maxAngle);

  servo.write(angle);
}

/* ================= GRIPPER ================= */
void handleGripper() {
  if (digitalRead(JOY_SW_PIN) == LOW) {
    if (millis() - lastButton > 300) {
      angleZ = (angleZ == Z_OPEN) ? Z_CLOSE : Z_OPEN;
      servoZ.write(angleZ);
      lastButton = millis();
    }
  }
}
