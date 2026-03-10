// #define ENA 5
// #define IN1 6
// #define IN2 7

// #define RAIN A0
// #define LIMIT_OPEN 3
// #define LIMIT_CLOSE 2

// const int RAIN_THRESHOLD = 600;
// const int RAIN_HYST = 50;

// const int NUM_SAMPLES = 10;
// int rainBuffer[NUM_SAMPLES];
// long rainSum = 0;
// int rainIndex = 0;
// bool rainState = false;

// enum State { STOPPED, OPENING, CLOSING };
// State systemState = STOPPED;

// unsigned long lastSample = 0;
// unsigned long lastPrint = 0;
// unsigned long lastDirectionChange = 0;

// const unsigned long SAMPLE_INTERVAL = 100;
// const unsigned long PRINT_INTERVAL = 1000;
// const unsigned long SAFE_DELAY = 500;

// bool openStable = LOW;
// bool closeStable = LOW;
// unsigned long lastOpenChange = 0;
// unsigned long lastCloseChange = 0;
// const unsigned long DEBOUNCE_TIME = 30;

// void stopMotor() {
//   analogWrite(ENA, 0);
//   digitalWrite(IN1, LOW);
//   digitalWrite(IN2, LOW);
//   systemState = STOPPED;
// }

// void openRoof() {
//   if (millis() - lastDirectionChange < SAFE_DELAY) return;

//   digitalWrite(IN1, HIGH);
//   digitalWrite(IN2, LOW);
//   analogWrite(ENA, 200);

//   systemState = OPENING;
//   lastDirectionChange = millis();
// }

// void closeRoof() {
//   if (millis() - lastDirectionChange < SAFE_DELAY) return;

//   digitalWrite(IN1, LOW);
//   digitalWrite(IN2, HIGH);
//   analogWrite(ENA, 200);

//   systemState = CLOSING;
//   lastDirectionChange = millis();
// }

// void setup() {

//   Serial.begin(9600);

//   pinMode(ENA, OUTPUT);
//   pinMode(IN1, OUTPUT);
//   pinMode(IN2, OUTPUT);

//   pinMode(LIMIT_OPEN, INPUT_PULLUP);
//   pinMode(LIMIT_CLOSE, INPUT_PULLUP);

//   stopMotor();

//   for (int i = 0; i < NUM_SAMPLES; i++) {
//     rainBuffer[i] = analogRead(RAIN);
//     rainSum += rainBuffer[i];
//   }

//   Serial.println("=== SYSTEM READY (NC MODE) ===");
// }

// void loop() {

//   unsigned long now = millis();

//   if (now - lastSample >= SAMPLE_INTERVAL) {

//     lastSample = now;

//     rainSum -= rainBuffer[rainIndex];
//     rainBuffer[rainIndex] = analogRead(RAIN);
//     rainSum += rainBuffer[rainIndex];
//     rainIndex = (rainIndex + 1) % NUM_SAMPLES;

//     int rainAvg = rainSum / NUM_SAMPLES;

//     if (!rainState && rainAvg < RAIN_THRESHOLD)
//       rainState = true;
//     else if (rainState && rainAvg > (RAIN_THRESHOLD + RAIN_HYST))
//       rainState = false;

//     bool openRead = digitalRead(LIMIT_OPEN);
//     if (openRead != openStable && (now - lastOpenChange) > DEBOUNCE_TIME) {
//       openStable = openRead;
//       lastOpenChange = now;
//     }

//     bool closeRead = digitalRead(LIMIT_CLOSE);
//     if (closeRead != closeStable && (now - lastCloseChange) > DEBOUNCE_TIME) {
//       closeStable = closeRead;
//       lastCloseChange = now;
//     }

//     if (systemState == OPENING && openStable == HIGH) {
//       stopMotor();
//     }

//     if (systemState == CLOSING && closeStable == HIGH) {
//       stopMotor();
//     }

//     if (rainState) {

//       if (closeStable == LOW) {
//         if (systemState != CLOSING)
//           closeRoof();
//       }
//       else {
//         stopMotor();
//       }
//     }
//     else {

//       if (openStable == LOW) {
//         if (systemState != OPENING)
//           openRoof();
//       }
//       else {
//         stopMotor();
//       }
//     }
//   }

//   if (now - lastPrint >= PRINT_INTERVAL) {

//     lastPrint = now;

//     Serial.print("Rain: ");
//     Serial.print(rainState);

//     Serial.print(" | State: ");
//     switch (systemState) {
//       case STOPPED: Serial.print("STOPPED"); break;
//       case OPENING: Serial.print("OPENING"); break;
//       case CLOSING: Serial.print("CLOSING"); break;
//     }

//     Serial.print(" | Open: ");
//     Serial.print(openStable);
//     Serial.print(" | Close: ");
//     Serial.println(closeStable);
//   }
// }
#define ENA 5
#define IN1 6
#define IN2 7

#define RAIN A0
#define LIGHT A1

#define LIMIT_OPEN 3
#define LIMIT_CLOSE 2

// ===== THRESHOLD =====

const int RAIN_THRESHOLD  = 600;
const int RAIN_HYST       = 50;

const int LIGHT_THRESHOLD = 450;
const int LIGHT_HYST      = 50;

// ===== TIME =====

const unsigned long SAMPLE_INTERVAL = 100;
const unsigned long PRINT_INTERVAL  = 100;
const unsigned long SAFE_DELAY      = 500;
const unsigned long DEBOUNCE_TIME   = 30;

// ===== STATE MACHINE =====

enum State { STOPPED, OPENING, CLOSING };
State systemState = STOPPED;

// ===== TIME =====

unsigned long lastSample = 0;
unsigned long lastPrint = 0;
unsigned long lastDirectionChange = 0;

// ===== SENSOR STATE =====

bool rainState = false;
bool darkState = false;

// ===== SENSOR VALUES =====

int rainRaw = 0;
int rainFiltered = 0;

int lightRaw = 0;
int lightFiltered = 0;

// ===== FILTER =====

const int NUM_SAMPLES = 50;

int rainBuffer[NUM_SAMPLES];
int lightBuffer[NUM_SAMPLES];

long rainSum = 0;
long lightSum = 0;

int filterIndex = 0;

// ===== LIMIT SWITCH =====

bool openStable = LOW;
bool closeStable = LOW;

unsigned long lastOpenChange = 0;
unsigned long lastCloseChange = 0;

// ================= MOTOR =================

void stopMotor() {

  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  systemState = STOPPED;
}

void openRoof() {

  if (millis() - lastDirectionChange < SAFE_DELAY) return;

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 200);

  systemState = OPENING;
  lastDirectionChange = millis();
}

void closeRoof() {

  if (millis() - lastDirectionChange < SAFE_DELAY) return;

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 200);

  systemState = CLOSING;
  lastDirectionChange = millis();
}

// ================= SETUP =================

void setup() {

  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(LIMIT_OPEN, INPUT_PULLUP);
  pinMode(LIMIT_CLOSE, INPUT_PULLUP);

  stopMotor();

  for (int i = 0; i < NUM_SAMPLES; i++) {

    rainBuffer[i] = analogRead(RAIN);
    lightBuffer[i] = analogRead(LIGHT);

    rainSum += rainBuffer[i];
    lightSum += lightBuffer[i];
  }

  Serial.println("rainRaw\trainFiltered\tlightRaw\tlightFiltered\tstate");
}

// ================= LOOP =================

void loop() {

  unsigned long now = millis();

  if (now - lastSample >= SAMPLE_INTERVAL) {

    lastSample = now;

    // ===== READ RAW =====

    rainRaw = analogRead(RAIN);
    lightRaw = analogRead(LIGHT);

    // ===== FILTER RAIN =====

    rainSum -= rainBuffer[filterIndex];
    rainBuffer[filterIndex] = rainRaw;
    rainSum += rainBuffer[filterIndex];

    rainFiltered = rainSum / NUM_SAMPLES;

    // ===== FILTER LIGHT =====

    lightSum -= lightBuffer[filterIndex];
    lightBuffer[filterIndex] = lightRaw;
    lightSum += lightBuffer[filterIndex];

    lightFiltered = lightSum / NUM_SAMPLES;

    filterIndex = (filterIndex + 1) % NUM_SAMPLES;

    // ===== RAIN LOGIC =====

    if (!rainState && rainFiltered < RAIN_THRESHOLD)
      rainState = true;
    else if (rainState && rainFiltered > (RAIN_THRESHOLD + RAIN_HYST))
      rainState = false;

    // ===== LIGHT LOGIC =====

    if (!darkState && lightFiltered > LIGHT_THRESHOLD)
      darkState = true;
    else if (darkState && lightFiltered < (LIGHT_THRESHOLD - LIGHT_HYST))
      darkState = false;

    // ===== LIMIT SWITCH =====

    bool openRead = digitalRead(LIMIT_OPEN);

    if (openRead != openStable && (now - lastOpenChange) > DEBOUNCE_TIME) {

      openStable = openRead;
      lastOpenChange = now;
    }

    bool closeRead = digitalRead(LIMIT_CLOSE);

    if (closeRead != closeStable && (now - lastCloseChange) > DEBOUNCE_TIME) {

      closeStable = closeRead;
      lastCloseChange = now;
    }

    // ===== STOP LIMIT =====

    if (systemState == OPENING && openStable == HIGH)
      stopMotor();

    if (systemState == CLOSING && closeStable == HIGH)
      stopMotor();

    // ===== DECISION =====

    if (darkState || rainState) {

      if (closeStable == LOW) {

        if (systemState != CLOSING)
          closeRoof();

      } else stopMotor();
    }
    else {

      if (openStable == LOW) {

        if (systemState != OPENING)
          openRoof();

      } else stopMotor();
    }
  }

  // ===== SERIAL PLOTTER =====

  if (now - lastPrint >= PRINT_INTERVAL) {

    lastPrint = now;

    Serial.print(rainRaw);
    Serial.print("\t");

    Serial.print(rainFiltered);
    Serial.print("\t");

    Serial.print(lightRaw);
    Serial.print("\t");

    Serial.print(lightFiltered);
    Serial.print("\t");

    Serial.println(systemState);
  }
}