#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

/* ================= LCD ================= */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* ================= PIN ================= */
#define MQ2_PIN A0
#define MQ7_PIN A1
#define DUST_PIN A2
#define DUST_LED_PIN 6

#define DHT_PIN 8
#define DHT_TYPE DHT22

#define LED_GREEN 2
#define LED_YELLOW 3
#define LED_RED 4
#define BUZZER 5

DHT dht(DHT_PIN, DHT_TYPE);

/* ================= CONFIG ================= */
#define READ_INTERVAL 2000
#define RL_VALUE 10.0
#define ADC_SAMPLES 15
#define DUST_SAMPLES 7

float R0_VALUE = 10.0;
float alpha = 0.25;

/* ================= DATA ================= */
float co_ppm = 0;
float gas_raw = 0;
float dust_pm = 0;
float temperature = 25;
float humidity = 50;

unsigned long lastReadTime = 0;

enum Level { SAFE, WARNING, DANGER };
Level systemState = SAFE;

/* ===================================================== */
/* ================= UTILITY ================= */

int readADCFiltered(int pin) {

  long sum = 0;

  // dummy read để ổn định ADC khi đổi kênh
  analogRead(pin);
  delay(3);

  for (int i = 0; i < ADC_SAMPLES; i++) {
    sum += analogRead(pin);
    delay(2);
  }

  return sum / ADC_SAMPLES;
}

/* ================= MEDIAN FILTER ================= */

int median(int *arr, int size) {
  for (int i = 0; i < size - 1; i++) {
    for (int j = i + 1; j < size; j++) {
      if (arr[j] < arr[i]) {
        int t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
      }
    }
  }
  return arr[size / 2];
}

/* ===================================================== */

float calculateCOppm(float voltage) {
  if (voltage <= 0.1) return 0;

  float sensorRs = ((5.0 - voltage) / voltage) * RL_VALUE;
  float ratio = sensorRs / R0_VALUE;

  return pow(10, ((log10(ratio) - 0.77) / -0.45));
}

/* ================= DUST SENSOR ================= */

float readDustSensor() {

  int samples[DUST_SAMPLES];

  for (int i = 0; i < DUST_SAMPLES; i++) {

    digitalWrite(DUST_LED_PIN, LOW);
    delayMicroseconds(280);

    samples[i] = analogRead(DUST_PIN);

    delayMicroseconds(40);
    digitalWrite(DUST_LED_PIN, HIGH);
    delayMicroseconds(9680);
  }

  int medianRaw = median(samples, DUST_SAMPLES);

  float voltage = medianRaw * (5.0 / 1023.0);

  float density = (voltage - 0.6) * 1000.0 / 0.5;

  if (density < 0) density = 0;

  return density;
}

/* ===================================================== */

void setup() {

  Serial.begin(115200);
  analogReference(DEFAULT);

  pinMode(DUST_LED_PIN, OUTPUT);
  digitalWrite(DUST_LED_PIN, HIGH);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  dht.begin();

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("ENV MONITOR");
  delay(1500);
  lcd.clear();
}

/* ===================================================== */

void loop() {

  if (millis() - lastReadTime >= READ_INTERVAL) {

    lastReadTime = millis();

    readSensors();
    evaluateEnvironment();
    updateOutputs();
    updateLCD();
    debugSerial();
  }
}

/* ===================================================== */

void readSensors() {

  int mq2Value = readADCFiltered(MQ2_PIN);
  int mq7Value = readADCFiltered(MQ7_PIN);

  float mq7Voltage = mq7Value * (5.0 / 1023.0);

  float newCO = calculateCOppm(mq7Voltage);
  float newDust = readDustSensor();

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  gas_raw = alpha * mq2Value + (1 - alpha) * gas_raw;
  co_ppm  = alpha * newCO + (1 - alpha) * co_ppm;
  dust_pm = alpha * newDust + (1 - alpha) * dust_pm;

  if (!isnan(t)) temperature = alpha * t + (1 - alpha) * temperature;
  if (!isnan(h)) humidity = alpha * h + (1 - alpha) * humidity;
}

/* ===================================================== */

void evaluateEnvironment() {

  systemState = SAFE;

  if (co_ppm > 70) { systemState = DANGER; return; }
  else if (co_ppm >= 9) systemState = WARNING;

  if (gas_raw > 700) { systemState = DANGER; return; }
  else if (gas_raw >= 300 && systemState != WARNING)
    systemState = WARNING;

  if (dust_pm > 56) { systemState = DANGER; return; }
  else if (dust_pm >= 13 && systemState != WARNING)
    systemState = WARNING;

  if (temperature > 32) { systemState = DANGER; return; }
  else if ((temperature < 16 || temperature > 29) && systemState == SAFE)
    systemState = WARNING;

  if ((humidity < 30 || humidity > 85) && systemState == SAFE)
    systemState = WARNING;
}

/* ===================================================== */

void updateOutputs() {

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  noTone(BUZZER);

  switch(systemState) {

    case SAFE:
      digitalWrite(LED_GREEN, HIGH);
      break;

    case WARNING:
      digitalWrite(LED_YELLOW, HIGH);
      break;

    case DANGER:
      digitalWrite(LED_RED, HIGH);
      tone(BUZZER, 2000);
      break;
  }
}

/* ===================================================== */

void updateLCD() {

  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(temperature,1);
  lcd.print(" H:");
  lcd.print(humidity,0);
  lcd.print("% ");

  lcd.setCursor(0,1);
  lcd.print("CO:");
  lcd.print(co_ppm,1);
  lcd.print(" PM:");
  lcd.print(dust_pm,0);
  lcd.print("   ");
}

/* ===================================================== */

void debugSerial() {

  Serial.print("CO: "); Serial.print(co_ppm,1);
  Serial.print(" | GAS: "); Serial.print(gas_raw);
  Serial.print(" | PM2.5: "); Serial.print(dust_pm,1);
  Serial.print(" | T: "); Serial.print(temperature,1);
  Serial.print(" | H: "); Serial.print(humidity,1);
  Serial.print(" | STATE: ");

  if(systemState==SAFE) Serial.println("SAFE");
  else if(systemState==WARNING) Serial.println("WARNING");
  else Serial.println("DANGER");
}