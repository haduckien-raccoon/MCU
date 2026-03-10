#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

/* ================= PIN CONFIG ================= */

#define MQ2_PIN      4
#define MQ7_PIN      5
#define DUST_PIN     6
#define DUST_LED_PIN 7

#define DHT_PIN      15
#define DHT_TYPE     DHT22

#define LED_GREEN    16
#define LED_YELLOW   17
#define LED_RED      18
#define BUZZER_PIN   21

#define SDA_PIN      8
#define SCL_PIN      9

/* ================= CONFIG ================= */

#define FILTER_SIZE 7
#define RL_VALUE    10.0f
#define ALPHA       0.8f

#define ADC_REF     3.3f
#define ADC_RES     4095.0f

/* ================= DUST CALIBRATION ================= */
/*
 *  GP2Y1010AU0F datasheet:
 *    Vo(mV) = K × dust(µg/m³) + Vno_dust
 *    K ≈ 0.5 mV per µg/m³  (tức 0.0005 V per µg/m³)
 *
 *  Vno_dust (điện ��p khi không bụi) KHÁC NHAU theo từng module,
 *  nên ta đo thực tế lúc khởi động để hiệu chuẩn.
 *
 *  Nếu có mạch chia áp 1:1 (2 điện trở bằng nhau) → DIVIDER = 0.5
 *  Nếu nối thẳng (ngõ ra cảm biến < 3.3V)         → DIVIDER = 1.0
 */
#define DUST_DIVIDER_RATIO  1
#define DUST_K              0.0005f    // V per µg/m³ (datasheet)
#define DUST_CAL_SAMPLES    50         // số mẫu đo khi hiệu chuẩn

float dustVnoDust = 0.0f;             // sẽ đo thực tế lúc setup()

/* ================= GLOBAL DATA ================= */

float co_ppm      = 0;
float gas_raw     = 0;
float dust_pm     = 0;       // µg/m³
float temperature  = 25;
float humidity     = 50;

float coBuf[FILTER_SIZE]   = {0};
float gasBuf[FILTER_SIZE]  = {0};
float dustBuf[FILTER_SIZE] = {0};
int   bufIndex = 0;

uint8_t systemState = 0;    // 0 SAFE, 1 WARN, 2 DANGER

/* ================= DEVICES ================= */

DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* ================= BUZZER (ESP32 Arduino Core 3.x) ================= */

#define BUZZER_FREQ 2000
#define BUZZER_RES  8

void buzzerInit(){
  ledcAttach(BUZZER_PIN, BUZZER_FREQ, BUZZER_RES);
}

void buzzerTone(uint32_t freq){
  ledcWriteTone(BUZZER_PIN, freq);
}

void buzzerOff(){
  ledcWriteTone(BUZZER_PIN, 0);
}

/* ================= FILTER ================= */

float median(float *arr){
  float temp[FILTER_SIZE];
  memcpy(temp, arr, sizeof(temp));
  for(int i = 0; i < FILTER_SIZE - 1; i++)
    for(int j = i + 1; j < FILTER_SIZE; j++)
      if(temp[j] < temp[i]){
        float t = temp[i];
        temp[i] = temp[j];
        temp[j] = t;
      }
  return temp[FILTER_SIZE / 2];
}

/* ================= MQ7 – CO ================= */

float calcCO(float v3){
  if(v3 <= 0.1f) return 0;

  float v5 = v3 * (5.0f / 3.3f);
  float Rs    = ((5.0f - v5) / v5) * RL_VALUE;
  float ratio = Rs / RL_VALUE;

  return powf(10.0f, (log10f(ratio) - 0.77f) / -0.45f);
}

/* ================= DUST – GP2Y1010AU0F ================= */
/*
 *  Quy trình đo 1 lần (theo datasheet timing):
 *    1. LED LOW  (bật IR)
 *    2. Chờ 280 µs → đọc ADC
 *    3. Chờ  40 µs
 *    4. LED HIGH (tắt IR)
 *    5. Chờ 9680 µs → hoàn thành chu kỳ 10 ms
 *
 *  Trả về điện áp thực ở ngõ ra cảm biến (đã bù chia áp).
 */

float readDustVoltage(){
  uint32_t sum = 0;
  const int samples = 10;

  for(int i = 0; i < samples; i++){
    digitalWrite(DUST_LED_PIN, LOW);
    delayMicroseconds(280);
    sum += analogRead(DUST_PIN);
    delayMicroseconds(40);
    digitalWrite(DUST_LED_PIN, HIGH);
    delayMicroseconds(9680);
  }

  float avg  = sum / (float)samples;
  float vAdc = avg * ADC_REF / ADC_RES;             // 0 – 3.3 V
  float vSensor = vAdc / DUST_DIVIDER_RATIO;         // điện áp thực cảm biến

  return vSensor;
}

float readDust(){
  float vSensor = readDustVoltage();

  /*  dust(µg/m³) = (Vsensor − Vno_dust) / K  */
  float d = (vSensor - dustVnoDust) / DUST_K;

  if(d < 0.0f) d = 0.0f;

  return d;   // µg/m³
}

/* ================= DUST CALIBRATION ================= */
/*
 *  Gọi lúc setup(), trong không khí sạch.
 *  Đo nhiều mẫu, lấy trung bình làm baseline (Vno_dust).
 */

void calibrateDust(){
  Serial.println("Dust sensor calibrating...");

  float sum = 0;

  for(int i = 0; i < DUST_CAL_SAMPLES; i++){
    sum += readDustVoltage();
    delay(20);   // mỗi lần đọc ≈100 ms (10×10 ms) + 20 ms
  }

  dustVnoDust = sum / DUST_CAL_SAMPLES;

  Serial.printf("Dust calibration done.  Vno_dust = %.3f V\n", dustVnoDust);
}

/* ================= STATE ================= */

void evaluate(){
  if(co_ppm > 70 || gas_raw > 700 || dust_pm > 80 || temperature > 32)
    systemState = 2;
  else if(co_ppm >= 9 || gas_raw >= 300 || dust_pm >= 25)
    systemState = 1;
  else
    systemState = 0;
}

void updateAlarm(){
  digitalWrite(LED_GREEN,  LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED,    LOW);
  buzzerOff();

  if(systemState == 2){
    digitalWrite(LED_RED, HIGH);
    buzzerTone(2500);
  }
  else if(systemState == 1){
    digitalWrite(LED_YELLOW, HIGH);
  }
  else{
    digitalWrite(LED_GREEN, HIGH);
  }
}

/* ================= SETUP ================= */

void setup(){
  Serial.begin(115200);

  analogReadResolution(12);
  analogSetPinAttenuation(MQ2_PIN,  ADC_11db);
  analogSetPinAttenuation(MQ7_PIN,  ADC_11db);
  analogSetPinAttenuation(DUST_PIN, ADC_11db);

  pinMode(DUST_LED_PIN, OUTPUT);
  digitalWrite(DUST_LED_PIN, HIGH);   // tắt IR-LED (active LOW)

  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED,    OUTPUT);

  buzzerInit();

  Wire.begin(SDA_PIN, SCL_PIN);
  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");

  /* Chờ cảm biến ổn định rồi hiệu chuẩn baseline */
  delay(2000);
  calibrateDust();

  lcd.clear();
}

/* ================= LOOP ================= */

void loop(){

  static unsigned long lastSensor = 0;
  static unsigned long lastDHT    = 0;
  static unsigned long lastLCD    = 0;
  static unsigned long lastSerial = 0;
  static bool page = false;

  unsigned long now = millis();

  /* ---- SENSOR 300 ms ---- */
  if(now - lastSensor >= 300){
    lastSensor = now;

    int mq2 = analogRead(MQ2_PIN);
    int mq7 = analogRead(MQ7_PIN);

    float v7 = mq7 * ADC_REF / ADC_RES;

    float rawCO   = calcCO(v7);
    float rawDust = readDust();

    coBuf[bufIndex]   = rawCO;
    gasBuf[bufIndex]  = (float)mq2;
    dustBuf[bufIndex] = rawDust;
    bufIndex = (bufIndex + 1) % FILTER_SIZE;

    co_ppm  = ALPHA * median(coBuf)   + (1 - ALPHA) * co_ppm;
    gas_raw = ALPHA * median(gasBuf)  + (1 - ALPHA) * gas_raw;
    dust_pm = ALPHA * median(dustBuf) + (1 - ALPHA) * dust_pm;

    evaluate();
    updateAlarm();
  }

  /* ---- DHT 2 s ---- */
  if(now - lastDHT >= 2000){
    lastDHT = now;

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if(!isnan(t)) temperature = ALPHA * t + (1 - ALPHA) * temperature;
    if(!isnan(h)) humidity    = ALPHA * h + (1 - ALPHA) * humidity;
  }

  /* ---- LCD 500 ms ---- */
  if(now - lastLCD >= 500){
    lastLCD = now;

    static unsigned long pageTimer = 0;
    if(now - pageTimer >= 5000){
      page = !page;
      pageTimer = now;
      lcd.clear();
    }

    if(!page){
      lcd.setCursor(0, 0);
      lcd.printf("T:%.1f H:%.0f%%", temperature, humidity);
      lcd.setCursor(0, 1);
      lcd.printf("PM:%.0f CO:%.1f", dust_pm, co_ppm);
    } else {
      lcd.setCursor(0, 0);
      lcd.printf("GAS:%.0f", gas_raw);
      lcd.setCursor(0, 1);
      lcd.print("STATE:");
      if(systemState == 0)      lcd.print("SAFE");
      else if(systemState == 1) lcd.print("WARN");
      else                      lcd.print("DANGER");
    }
  }

  /* ---- SERIAL 1 s ---- */
  if(now - lastSerial >= 1000){
    lastSerial = now;
    Serial.printf("CO:%.1f GAS:%.0f PM:%.0f T:%.1f H:%.1f  [Vnd=%.3f]\n",
                  co_ppm, gas_raw, dust_pm, temperature, humidity, dustVnoDust);
  }
}