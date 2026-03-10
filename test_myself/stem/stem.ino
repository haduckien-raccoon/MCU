// // // #include <DHT.h>
// // // #include <LiquidCrystal_I2C.h>
// // // #include <math.h>

// // // /* ================= HARDWARE ================= */
// // // #define MQ2_PIN A2
// // // #define MQ7_PIN A1
// // // #define DHT_PIN 8
// // // #define DHT_TYPE DHT22
// // // #define LED_GREEN 7
// // // #define LED_YELLOW 3
// // // #define LED_RED 4
// // // #define BUZZER 5

// // // LiquidCrystal_I2C lcd(0x27, 16, 2);
// // // DHT dht(DHT_PIN, DHT_TYPE);

// // // /* ================= TIMING ================= */
// // // unsigned long sensorTimer = 0;
// // // unsigned long lcdTimer = 0;
// // // unsigned long blinkTimer = 0;
// // // unsigned long lcdRefreshTimer = 0;

// // // #define SENSOR_INTERVAL 1000
// // // #define LCD_INTERVAL 4000
// // // #define BLINK_INTERVAL 500
// // // #define LCD_REFRESH_INTERVAL 250

// // // /* ================= DATA ================= */
// // // float co_ppm = 0;
// // // float temperature = 25;
// // // float humidity = 50;
// // // float gas_raw = 0;

// // // float co_prev = 0;
// // // float gas_prev = 0;

// // // /* ================= MEDIAN BUFFER ================= */
// // // #define MEDIAN_SIZE 5
// // // float coBuf[MEDIAN_SIZE];
// // // float gasBuf[MEDIAN_SIZE];
// // // int bufIndex = 0;

// // // /* ================= STATE ================= */
// // // enum AlarmState { SAFE, PREWARN, WARNING, DANGER };
// // // AlarmState alarmState = SAFE;
// // // bool lcdPage = false;
// // // bool blinkState = true;
// // // int systemScore = 0; 

// // // /* ================= UTIL ================= */
// // // int readStableADC(int pin) {
// // //   analogRead(pin); 
// // //   long sum = 0;
// // //   for (int i = 0; i < 10; i++) sum += analogRead(pin);
// // //   return sum / 10;
// // // }

// // // float median(float *arr) {
// // //   float temp[MEDIAN_SIZE];
// // //   for (int i = 0; i < MEDIAN_SIZE; i++) temp[i] = arr[i];
// // //   for (int i = 0; i < MEDIAN_SIZE - 1; i++)
// // //     for (int j = i + 1; j < MEDIAN_SIZE; j++)
// // //       if (temp[j] < temp[i]) {
// // //         float t = temp[i]; temp[i] = temp[j]; temp[j] = t;
// // //       }
// // //   return temp[MEDIAN_SIZE / 2];
// // // }

// // // float ema(float prev, float now, float alpha) {
// // //   return alpha * now + (1 - alpha) * prev;
// // // }

// // // /* ================= SETUP ================= */
// // // void setup() {
// // //   Serial.begin(115200);
// // //   pinMode(LED_GREEN, OUTPUT);
// // //   pinMode(LED_YELLOW, OUTPUT);
// // //   pinMode(LED_RED, OUTPUT);
// // //   pinMode(BUZZER, OUTPUT);
  
// // //   dht.begin();
// // //   lcd.init();
// // //   lcd.backlight();
  
// // //   lcd.setCursor(0, 0);
// // //   lcd.print("SYSTEM WARMING..");
// // //   lcd.setCursor(0, 1);
// // //   for(int i = 0; i < 16; i++) {
// // //     lcd.print(char(255)); 
// // //     delay(200);           
// // //   }
// // //   lcd.clear();
// // // }

// // // /* ================= LOOP ================= */
// // // void loop() {
// // //   unsigned long now = millis();
  
// // //   if (now - sensorTimer >= SENSOR_INTERVAL) {
// // //     sensorTimer = now;
// // //     readSensors();
// // //     processIndustrial();
// // //     printTelemetry(); 
// // //   }
  
// // //   handleBlink(now);
// // //   updateOutputs(); 
// // //   updateLCD(now);
// // // }

// // // /* ================= READ ================= */
// // // void readSensors() {
// // //   int mq2 = readStableADC(MQ2_PIN);
// // //   int mq7 = readStableADC(MQ7_PIN);
// // //   float voltage = mq7 * (5.0 / 1023.0);
// // //   float rs = (5.0 - voltage) / voltage * 10.0;
// // //   float ratio = rs / 10.0;
// // //   float co = pow(10, ((log10(ratio) - 0.77) / -0.45));
  
// // //   coBuf[bufIndex] = co;
// // //   gasBuf[bufIndex] = mq2;
// // //   bufIndex++;
// // //   if (bufIndex >= MEDIAN_SIZE) bufIndex = 0;
  
// // //   co_ppm = median(coBuf);
// // //   gas_raw = median(gasBuf);
  
// // //   float t = dht.readTemperature();
// // //   float h = dht.readHumidity();
// // //   if (!isnan(t)) temperature = ema(temperature, t, 0.6);
// // //   if (!isnan(h)) humidity = ema(humidity, h, 0.6);
// // // }

// // // /* ================= INDUSTRIAL PROCESS ================= */
// // // void processIndustrial() {
// // //   // 1. Làm mượt tín hiệu bằng bộ lọc EMA
// // //   co_ppm = ema(co_prev, co_ppm, 0.5);
// // //   gas_raw = ema(gas_prev, gas_raw, 0.5);
  
// // //   // 2. Tính tốc độ gia tăng của khí
// // //   float co_delta = co_ppm - co_prev;
// // //   float gas_delta = gas_raw - gas_prev;
  
// // //   co_prev = co_ppm;
// // //   gas_prev = gas_raw;
  
// // //   /* --- LỚP 1: CHẤM ĐIỂM RỦI RO (Tối đa 6 điểm) --- */
// // //   systemScore = 0; 
  
// // //   // Điều kiện Khí độc & Gas
// // //   if (co_ppm >= 9) systemScore++;         // Chớm nguy hiểm
// // //   if (gas_raw >= 300) systemScore++;      // Rò rỉ nhẹ
// // //   if (co_delta > 10) systemScore++;       // CO tăng vọt
// // //   if (gas_delta > 100) systemScore++;     // Gas tăng vọt
  
// // //   // Điều kiện Vi khí hậu theo WHO
// // //   if (temperature < 16.0 || temperature > 32.0) systemScore++; 
// // //   if (humidity < 30.0 || humidity > 90.0) systemScore++;       
  
// // //   // Xử lý Máy trạng thái
// // //   switch (alarmState) {
// // //     case SAFE:
// // //       if (systemScore >= 1) alarmState = PREWARN;
// // //       break;
// // //     case PREWARN:
// // //       if (systemScore >= 2) alarmState = WARNING;
// // //       else if (systemScore == 0) alarmState = SAFE;
// // //       break;
// // //     case WARNING:
// // //       if (systemScore >= 4) alarmState = DANGER;
// // //       else if (systemScore <= 1) alarmState = PREWARN;
// // //       break;
// // //     case DANGER:
// // //       if (systemScore <= 3) alarmState = WARNING;
// // //       break;
// // //   }

// // //   /* --- LỚP 2: NGƯỠNG TUYỆT ĐỐI (Dựa trên số liệu chuẩn) --- */
// // //   // 🚨 Nguy hiểm - Yêu cầu sơ tán: CO > 70ppm hoặc Gas > 700
// // //   if (co_ppm >= 70 || gas_raw >= 700) {
// // //     alarmState = DANGER;
// // //   } 
// // //   // ⚠️ Ngưỡng báo động 1 giờ / Rò rỉ rõ rệt: CO >= 35ppm hoặc Gas >= 450
// // //   else if ((co_ppm >= 35 || gas_raw >= 450) && alarmState < WARNING) {
// // //     alarmState = WARNING;
// // //   }
// // // }

// // // /* ================= TELEMETRY ================= */
// // // void printTelemetry() {
// // //   Serial.print("CO_PPM:"); Serial.print(co_ppm); Serial.print(",");
// // //   Serial.print("GAS_RAW:"); Serial.print(gas_raw); Serial.print(",");
// // //   Serial.print("TEMP:"); Serial.print(temperature); Serial.print(",");
// // //   Serial.print("DANGER_SCORE:"); Serial.print(systemScore * 20); 
// // //   Serial.println();
// // // }

// // // /* ================= OUTPUT ================= */
// // // void updateOutputs() {
// // //   switch (alarmState) {
// // //     case SAFE:
// // //       digitalWrite(LED_GREEN, HIGH);
// // //       digitalWrite(LED_YELLOW, LOW);
// // //       digitalWrite(LED_RED, LOW);
// // //       noTone(BUZZER);
// // //       break;
// // //     case PREWARN:
// // //       digitalWrite(LED_GREEN, LOW);
// // //       digitalWrite(LED_YELLOW, HIGH);
// // //       digitalWrite(LED_RED, LOW);
// // //       noTone(BUZZER);
// // //       break;
// // //     case WARNING:
// // //       digitalWrite(LED_GREEN, LOW);
// // //       digitalWrite(LED_YELLOW, HIGH);
// // //       digitalWrite(LED_RED, LOW);
// // //       tone(BUZZER, 1500);
// // //       break;
// // //     case DANGER:
// // //       digitalWrite(LED_GREEN, LOW);
// // //       digitalWrite(LED_YELLOW, LOW);
// // //       if (blinkState) {
// // //         digitalWrite(LED_RED, HIGH);
// // //         tone(BUZZER, 2000);
// // //       } else {
// // //         digitalWrite(LED_RED, LOW);
// // //         noTone(BUZZER);
// // //       }
// // //       break;
// // //   }
// // // }

// // // /* ================= BLINK ================= */
// // // void handleBlink(unsigned long now) {
// // //   if (alarmState == DANGER) {
// // //     if (now - blinkTimer >= BLINK_INTERVAL) {
// // //       blinkTimer = now;
// // //       blinkState = !blinkState;
// // //     }
// // //   } else {
// // //     blinkState = true;
// // //   }
// // // }

// // // /* ================= LCD ================= */
// // // void updateLCD(unsigned long now) {
// // //   if (now - lcdTimer >= LCD_INTERVAL) {
// // //     lcdTimer = now;
// // //     lcdPage = !lcdPage;
// // //     lcd.clear();
// // //   }

// // //   if (now - lcdRefreshTimer >= LCD_REFRESH_INTERVAL) {
// // //     lcdRefreshTimer = now;

// // //     if (!lcdPage) {
// // //       lcd.setCursor(0, 0);
// // //       lcd.print("CO:");
// // //       lcd.print(co_ppm, 0);
// // //       lcd.print(" GAS:");
// // //       lcd.print(gas_raw, 0);
// // //       lcd.print("   "); 
      
// // //       lcd.setCursor(0, 1);
// // //       lcd.print("T:");
// // //       lcd.print(temperature, 1);
// // //       lcd.print(" H:");
// // //       lcd.print(humidity, 0);
// // //       lcd.print("   ");
// // //     }
// // //     else {
// // //       lcd.setCursor(0, 0);
// // //       lcd.print("SCORE: ");
// // //       lcd.print(systemScore);
// // //       lcd.print("/6        "); 
      
// // //       lcd.setCursor(0, 1);
// // //       lcd.print("STATE: ");
// // //       if (alarmState == DANGER) {
// // //         if (blinkState) lcd.print("DANGER ");
// // //         else lcd.print("       ");
// // //       }
// // //       else if (alarmState == WARNING) lcd.print("WARNING");
// // //       else if (alarmState == PREWARN) lcd.print("PREWARN");
// // //       else lcd.print("SAFE   ");
// // //     }
// // //   }
// // // }
// // #include <DHT.h>
// // #include <LiquidCrystal_I2C.h>
// // #include <math.h>

// // /* ================= HARDWARE ================= */
// // #define MQ2_PIN A2
// // #define MQ7_PIN A1
// // #define DHT_PIN 8
// // #define DHT_TYPE DHT22

// // #define LED_GREEN 7
// // #define LED_YELLOW 3
// // #define LED_RED 4
// // #define BUZZER 5

// // LiquidCrystal_I2C lcd(0x27, 16, 2);
// // DHT dht(DHT_PIN, DHT_TYPE);

// // /* ================= TIMING ================= */
// // #define SENSOR_INTERVAL 1000
// // #define LCD_INTERVAL 4000
// // #define BLINK_INTERVAL 500
// // #define LCD_REFRESH_INTERVAL 300

// // unsigned long sensorTimer = 0;
// // unsigned long lcdTimer = 0;
// // unsigned long blinkTimer = 0;
// // unsigned long lcdRefreshTimer = 0;

// // /* ================= DATA ================= */
// // float co_ppm = 0;
// // float temperature = 25;
// // float humidity = 50;
// // float gas_raw = 0;

// // bool lcdPage = false;
// // bool blinkState = true;

// // /* ================= STATE ================= */
// // enum AlarmState { SAFE, PREWARN, WARNING, DANGER };
// // AlarmState alarmState = SAFE;

// // int systemScore = 0;

// // /* ================= UTIL ================= */
// // int readStableADC(int pin) {
// //   analogRead(pin);
// //   long sum = 0;
// //   for (int i = 0; i < 10; i++) sum += analogRead(pin);
// //   return sum / 10;
// // }

// // float ema(float prev, float now, float alpha) {
// //   return alpha * now + (1 - alpha) * prev;
// // }

// // /* ================= CO SCORING ================= */
// // int scoreCO(float co){
// //   if(co < 9) return 0;
// //   else if(co < 35) return 20;
// //   else if(co < 70) return 40;
// //   else if(co < 100) return 70;
// //   else return 100;
// // }

// // /* ================= GAS SCORING ================= */
// // int scoreGas(float gas){
// //   if(gas < 250) return 0;
// //   else if(gas < 450) return 25;
// //   else if(gas < 650) return 60;
// //   else return 100;
// // }

// // /* ================= TEMP SCORING ================= */
// // int scoreTemp(float t){
// //   if(t >= 40) return 100;
// //   if(t > 32) return 70;
// //   if(t > 30) return 30;
// //   if(t > 24) return 10;
// //   return 0;
// // }

// // /* ================= HUM SCORING ================= */
// // int scoreHum(float h){
// //   if(h > 85) return 70;
// //   if(h > 70) return 40;
// //   if(h < 30) return 30;
// //   if(h < 40) return 10;
// //   if(h <= 60) return 0;
// //   return 10;
// // }

// // /* ================= SENSOR READ ================= */
// // void readSensors() {

// //   gas_raw = ema(gas_raw, readStableADC(MQ2_PIN), 0.5);

// //   int mq7 = readStableADC(MQ7_PIN);
// //   float voltage = mq7 * (5.0 / 1023.0);
// //   float rs = (5.0 - voltage) / voltage * 10.0;
// //   float ratio = rs / 10.0;
// //   float co = pow(10, ((log10(ratio) - 0.77) / -0.45));
// //   co_ppm = ema(co_ppm, co, 0.5);

// //   float t = dht.readTemperature();
// //   float h = dht.readHumidity();

// //   if (!isnan(t)) temperature = ema(temperature, t, 0.5);
// //   if (!isnan(h)) humidity = ema(humidity, h, 0.5);
// // }

// // /* ================= PROCESS ================= */
// // void processIndustrial(){

// //   int coScore   = scoreCO(co_ppm);
// //   int gasScore  = scoreGas(gas_raw);
// //   int tempScore = scoreTemp(temperature);
// //   int humScore  = scoreHum(humidity);

// //   int airRisk = max(coScore, gasScore);

// //   systemScore = airRisk * 0.6
// //               + tempScore * 0.25
// //               + humScore * 0.15;

// //   /* ===== HARD OVERRIDE ===== */
// //   if(co_ppm >= 100 || gas_raw >= 700 || temperature >= 40){
// //     systemScore = 100;
// //   }

// //   /* ===== STATE MAP ===== */
// //   if(systemScore > 60) alarmState = DANGER;
// //   else if(systemScore > 35) alarmState = WARNING;
// //   else if(systemScore > 15) alarmState = PREWARN;
// //   else alarmState = SAFE;
// // }

// // /* ================= OUTPUT ================= */
// // void updateOutputs(){

// //   switch (alarmState) {

// //     case SAFE:
// //       digitalWrite(LED_GREEN, HIGH);
// //       digitalWrite(LED_YELLOW, LOW);
// //       digitalWrite(LED_RED, LOW);
// //       noTone(BUZZER);
// //       break;

// //     case PREWARN:
// //       digitalWrite(LED_GREEN, LOW);
// //       digitalWrite(LED_YELLOW, HIGH);
// //       digitalWrite(LED_RED, LOW);
// //       noTone(BUZZER);
// //       break;

// //     case WARNING:
// //       digitalWrite(LED_GREEN, LOW);
// //       digitalWrite(LED_YELLOW, HIGH);
// //       digitalWrite(LED_RED, LOW);
// //       tone(BUZZER, 1500);
// //       break;

// //     case DANGER:
// //       digitalWrite(LED_GREEN, LOW);
// //       digitalWrite(LED_YELLOW, LOW);
// //       if(blinkState){
// //         digitalWrite(LED_RED, HIGH);
// //         tone(BUZZER, 2000);
// //       } else {
// //         digitalWrite(LED_RED, LOW);
// //         noTone(BUZZER);
// //       }
// //       break;
// //   }
// // }

// // /* ================= BLINK ================= */
// // void handleBlink(unsigned long now){
// //   if(alarmState == DANGER){
// //     if(now - blinkTimer >= BLINK_INTERVAL){
// //       blinkTimer = now;
// //       blinkState = !blinkState;
// //     }
// //   } else {
// //     blinkState = true;
// //   }
// // }

// // /* ================= LCD ================= */
// // void updateLCD(unsigned long now){

// //   if(now - lcdTimer >= LCD_INTERVAL){
// //     lcdTimer = now;
// //     lcdPage = !lcdPage;
// //     lcd.clear();
// //   }

// //   if(now - lcdRefreshTimer >= LCD_REFRESH_INTERVAL){
// //     lcdRefreshTimer = now;

// //     if(!lcdPage){
// //       lcd.setCursor(0,0);
// //       lcd.print("CO:");
// //       lcd.print(co_ppm,0);
// //       lcd.print(" GAS:");
// //       lcd.print(gas_raw,0);
// //       lcd.print("   ");

// //       lcd.setCursor(0,1);
// //       lcd.print("T:");
// //       lcd.print(temperature,1);
// //       lcd.print(" H:");
// //       lcd.print(humidity,0);
// //       lcd.print("   ");
// //     }
// //     else{
// //       lcd.setCursor(0,0);
// //       lcd.print("RISK:");
// //       lcd.print(systemScore);
// //       lcd.print("%   ");

// //       lcd.setCursor(0,1);
// //       lcd.print("STATE:");

// //       if(alarmState == DANGER){
// //         if(blinkState) lcd.print("DANGER ");
// //         else lcd.print("       ");
// //       }
// //       else if(alarmState == WARNING) lcd.print("WARNING");
// //       else if(alarmState == PREWARN) lcd.print("PREWARN");
// //       else lcd.print("SAFE   ");
// //     }
// //   }
// // }

// // /* ================= TELEMETRY ================= */
// // void printTelemetry(){
// //   Serial.print("CO:"); Serial.print(co_ppm);
// //   Serial.print(" GAS:"); Serial.print(gas_raw);
// //   Serial.print(" TEMP:"); Serial.print(temperature);
// //   Serial.print(" HUM:"); Serial.print(humidity);
// //   Serial.print(" SCORE:"); Serial.print(systemScore);
// //   Serial.println();
// // }

// // /* ================= SETUP ================= */
// // void setup(){

// //   Serial.begin(115200);

// //   pinMode(LED_GREEN, OUTPUT);
// //   pinMode(LED_YELLOW, OUTPUT);
// //   pinMode(LED_RED, OUTPUT);
// //   pinMode(BUZZER, OUTPUT);

// //   dht.begin();
// //   lcd.init();
// //   lcd.backlight();

// //   lcd.setCursor(0,0);
// //   lcd.print("SYSTEM STARTING");
// //   delay(2000);
// //   lcd.clear();
// // }

// // /* ================= LOOP ================= */
// // void loop(){

// //   unsigned long now = millis();

// //   if(now - sensorTimer >= SENSOR_INTERVAL){
// //     sensorTimer = now;
// //     readSensors();
// //     processIndustrial();
// //     printTelemetry();
// //   }

// //   handleBlink(now);
// //   updateOutputs();
// //   updateLCD(now);
// // }
// #include <DHT.h>
// #include <LiquidCrystal_I2C.h>
// #include <math.h>

// /* ================= HARDWARE ================= */
// #define MQ2_PIN A2
// #define MQ7_PIN A1
// #define DHT_PIN 8
// #define DHT_TYPE DHT22

// #define LED_GREEN 7
// #define LED_YELLOW 3
// #define LED_RED 4
// #define BUZZER 5

// LiquidCrystal_I2C lcd(0x27, 16, 2);
// DHT dht(DHT_PIN, DHT_TYPE);

// /* ================= TIMING ================= */
// #define SENSOR_INTERVAL 1000
// #define LCD_INTERVAL 4000
// #define BLINK_INTERVAL 500
// #define LCD_REFRESH_INTERVAL 300

// unsigned long sensorTimer = 0;
// unsigned long lcdTimer = 0;
// unsigned long blinkTimer = 0;
// unsigned long lcdRefreshTimer = 0;

// /* ================= DATA ================= */
// float co_ppm = 0;
// float temperature = 25;
// float humidity = 50;
// float gas_raw = 0;

// bool lcdPage = false;
// bool blinkState = true;

// /* ================= STATE ================= */
// enum AlarmState { SAFE, PREWARN, WARNING, DANGER };
// AlarmState alarmState = SAFE;

// int systemScore = 0;

// /* ================= UTIL ================= */
// int readStableADC(int pin) {
//   analogRead(pin);
//   long sum = 0;
//   for (int i = 0; i < 10; i++) sum += analogRead(pin);
//   return sum / 10;
// }

// float ema(float prev, float now, float alpha) {
//   return alpha * now + (1 - alpha) * prev;
// }

// /* ================= CO SCORING ================= */
// int scoreCO(float co){
//   if(co < 9) return 0;
//   else if(co < 35) return 20;
//   else if(co < 70) return 40;
//   else if(co < 100) return 70;
//   else return 100;
// }

// /* ================= GAS SCORING ================= */
// int scoreGas(float gas){
//   if(gas < 250) return 0;
//   else if(gas < 450) return 25;
//   else if(gas < 650) return 60;
//   else return 100;
// }

// /* ================= TEMP SCORING ================= */
// int scoreTemp(float t){
//   if(t >= 40) return 100;
//   if(t > 34) return 70;
//   if(t > 30) return 30;
//   if(t > 24) return 10;
//   return 0;
// }

// /* ================= HUM SCORING ================= */
// int scoreHum(float h){
//   if(h > 85) return 70;
//   if(h > 70) return 40;
//   if(h < 30) return 30;
//   if(h < 40) return 10;
//   if(h <= 60) return 0;
//   return 10;
// }

// /* ================= SENSOR READ ================= */
// void readSensors() {

//   gas_raw = ema(gas_raw, readStableADC(MQ2_PIN), 0.5);

//   int mq7 = readStableADC(MQ7_PIN);
//   float voltage = mq7 * (5.0 / 1023.0);
//   float rs = (5.0 - voltage) / voltage * 10.0;
//   float ratio = rs / 10.0;
//   float co = pow(10, ((log10(ratio) - 0.77) / -0.45));
//   co_ppm = ema(co_ppm, co, 0.5);

//   float t = dht.readTemperature();
//   float h = dht.readHumidity();

//   if (!isnan(t)) temperature = ema(temperature, t, 0.5);
//   if (!isnan(h)) humidity = ema(humidity, h, 0.5);
// }

// /* ================= PROCESS ================= */
// void processIndustrial(){

//   int coScore   = scoreCO(co_ppm);
//   int gasScore  = scoreGas(gas_raw);
//   int tempScore = scoreTemp(temperature);
//   int humScore  = scoreHum(humidity);

//   int airRisk = max(coScore, gasScore);

//   systemScore = airRisk * 0.6
//               + tempScore * 0.25
//               + humScore * 0.15;

//   /* ===== HARD OVERRIDE ===== */
//   if(co_ppm >= 100 || gas_raw >= 700 || temperature >= 40){
//     systemScore = 100;
//   }

//   /* ===== STATE MAP ===== */
//   if(systemScore > 60) alarmState = DANGER;
//   else if(systemScore > 35) alarmState = WARNING;
//   else if(systemScore > 15) alarmState = PREWARN;
//   else alarmState = SAFE;
// }

// /* ================= OUTPUT ================= */
// void updateOutputs(){

//   switch (alarmState) {

//     case SAFE:
//       digitalWrite(LED_GREEN, HIGH);
//       digitalWrite(LED_YELLOW, LOW);
//       digitalWrite(LED_RED, LOW);
//       noTone(BUZZER);
//       break;

//     case PREWARN:
//       digitalWrite(LED_GREEN, LOW);
//       digitalWrite(LED_YELLOW, HIGH);
//       digitalWrite(LED_RED, LOW);
//       noTone(BUZZER);
//       break;

//     case WARNING:
//       digitalWrite(LED_GREEN, LOW);
//       digitalWrite(LED_YELLOW, HIGH);
//       digitalWrite(LED_RED, LOW);
//       tone(BUZZER, 1500);
//       break;

//     case DANGER:
//       digitalWrite(LED_GREEN, LOW);
//       digitalWrite(LED_YELLOW, LOW);
//       if(blinkState){
//         digitalWrite(LED_RED, HIGH);
//         tone(BUZZER, 2000);
//       } else {
//         digitalWrite(LED_RED, LOW);
//         noTone(BUZZER);
//       }
//       break;
//   }
// }

// /* ================= BLINK ================= */
// void handleBlink(unsigned long now){
//   if(alarmState == DANGER){
//     if(now - blinkTimer >= BLINK_INTERVAL){
//       blinkTimer = now;
//       blinkState = !blinkState;
//     }
//   } else {
//     blinkState = true;
//   }
// }

// /* ================= LCD ================= */
// void updateLCD(unsigned long now){

//   if(now - lcdTimer >= LCD_INTERVAL){
//     lcdTimer = now;
//     lcdPage = !lcdPage;
//     lcd.clear();
//   }

//   if(now - lcdRefreshTimer >= LCD_REFRESH_INTERVAL){
//     lcdRefreshTimer = now;

//     if(!lcdPage){
//       lcd.setCursor(0,0);
//       lcd.print("CO:");
//       lcd.print(co_ppm,0);
//       lcd.print(" GAS:");
//       lcd.print(gas_raw,0);
//       lcd.print("   ");

//       lcd.setCursor(0,1);
//       lcd.print("T:");
//       lcd.print(temperature,1);
//       lcd.print(" H:");
//       lcd.print(humidity,0);
//       lcd.print("   ");
//     }
//     else{
//       lcd.setCursor(0,0);
//       lcd.print("RISK:");
//       lcd.print(systemScore);
//       lcd.print("%   ");

//       lcd.setCursor(0,1);
//       lcd.print("STATE:");

//       if(alarmState == DANGER){
//         if(blinkState) lcd.print("DANGER ");
//         else lcd.print("       ");
//       }
//       else if(alarmState == WARNING) lcd.print("WARNING");
//       else if(alarmState == PREWARN) lcd.print("PREWARN");
//       else lcd.print("SAFE   ");
//     }
//   }
// }

// /* ================= TELEMETRY ================= */
// void printTelemetry(){
//   Serial.print("CO:"); Serial.print(co_ppm);
//   Serial.print(" GAS:"); Serial.print(gas_raw);
//   Serial.print(" TEMP:"); Serial.print(temperature);
//   Serial.print(" HUM:"); Serial.print(humidity);
//   Serial.print(" SCORE:"); Serial.print(systemScore);
//   Serial.println();
// }

// /* ================= SETUP ================= */
// void setup(){

//   Serial.begin(115200);

//   pinMode(LED_GREEN, OUTPUT);
//   pinMode(LED_YELLOW, OUTPUT);
//   pinMode(LED_RED, OUTPUT);
//   pinMode(BUZZER, OUTPUT);

//   dht.begin();
//   lcd.init();
//   lcd.backlight();

//   lcd.setCursor(0,0);
//   lcd.print("SYSTEM STARTING");
//   delay(2000);
//   lcd.clear();
// }

// /* ================= LOOP ================= */
// void loop(){

//   unsigned long now = millis();

//   if(now - sensorTimer >= SENSOR_INTERVAL){
//     sensorTimer = now;
//     readSensors();
//     processIndustrial();
//     printTelemetry();
//   }

//   handleBlink(now);
//   updateOutputs();
//   updateLCD(now);
// }

#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

/* ===== PIN ===== */
#define MQ2_PIN A2
#define MQ7_PIN A1
#define DHT_PIN 8
#define DHT_TYPE DHT22

#define LED_GREEN 7
#define LED_YELLOW 3
#define LED_RED 4
#define BUZZER 5

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHT_PIN, DHT_TYPE);

/* ===== STATE ===== */
enum AlarmState { SAFE, PREWARN, WARNING, DANGER };
AlarmState alarmState = SAFE;

/* ===== DATA ===== */
float co_ppm = 0;
float gas_raw = 0;
float temperature = 25;
float humidity = 50;

unsigned long blinkTimer = 0;
bool blinkState = true;

/* ===== READ ADC ===== */
int readStableADC(int pin){
  long sum = 0;
  for(int i=0;i<10;i++) sum += analogRead(pin);
  return sum/10;
}

/* ===== DETECT LEVEL ===== */
AlarmState detectCO(float co){
  if(co >= 70) return DANGER;
  else if(co >= 35) return WARNING;
  else if(co >= 9) return PREWARN;
  else return SAFE;
}

AlarmState detectGas(float gas){
  if(gas >= 600) return DANGER;
  else if(gas >= 450) return WARNING;
  else if(gas >= 300) return PREWARN;
  else return SAFE;
}

AlarmState detectTemp(float t){
  if(t >= 36) return DANGER;
  else if(t >= 34) return WARNING;
  else return SAFE;
}

AlarmState detectHum(float h){
  if(h < 30 || h > 90) return WARNING;
  else if(h < 40 || h > 88) return PREWARN;
  else return SAFE;
}

/* ===== UPDATE STATE ===== */
void updateState(){

  AlarmState coState = detectCO(co_ppm);
  AlarmState gasState = detectGas(gas_raw);
  AlarmState tempState = detectTemp(temperature);
  AlarmState humState = detectHum(humidity);

  // lấy mức nguy hiểm cao nhất
  alarmState = SAFE;

  if(coState > alarmState) alarmState = coState;
  if(gasState > alarmState) alarmState = gasState;
  if(tempState > alarmState) alarmState = tempState;
  if(humState > alarmState) alarmState = humState;
}

/* ===== OUTPUT ===== */
void updateOutput(){

  switch(alarmState){

    case SAFE:
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED, LOW);
      noTone(BUZZER);
      break;

    case PREWARN:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      digitalWrite(LED_RED, LOW);
      noTone(BUZZER);
      break;

    case WARNING:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      digitalWrite(LED_RED, LOW);
      noTone(BUZZER);
      // tone(BUZZER, 1500);
      break;

    case DANGER:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, LOW);
      if(blinkState){
        digitalWrite(LED_RED, HIGH);
        tone(BUZZER, 2000);
      } else {
        digitalWrite(LED_RED, LOW);
        noTone(BUZZER);
      }
      break;
  }
}

/* ===== SETUP ===== */
void setup(){

  Serial.begin(9600);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();
  dht.begin();
}

unsigned long screenTimer = 0;
bool screenMode = true; // true = màn 1, false = màn 2
/* ===== LOOP ===== */
void loop(){

  gas_raw = readStableADC(MQ2_PIN);

  int mq7 = readStableADC(MQ7_PIN);
  float voltage = mq7 * (5.0 / 1023.0);
  float rs = (5.0 - voltage) / voltage * 10.0;
  float ratio = rs / 10.0;
  co_ppm = pow(10, ((log10(ratio) - 0.77) / -0.45));

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if(!isnan(t)) temperature = t;
  if(!isnan(h)) humidity = h;

  updateState();
  updateOutput();

    /* ===== SCREEN SWITCH 2s ===== */
  if(millis() - screenTimer > 2000){
    screenTimer = millis();
    screenMode = !screenMode;
    lcd.clear();
  }

  if(screenMode){
    // ===== SCREEN 1 =====
    lcd.setCursor(0,0);
    lcd.print("CO:");
    lcd.print(co_ppm,0);
    lcd.print(" G:");
    lcd.print(gas_raw);
    lcd.print("   ");

    lcd.setCursor(0,1);
    lcd.print("....STEM....");
  }
  else{
    // ===== SCREEN 2 =====
    lcd.setCursor(0,0);
    lcd.print("T:");
    lcd.print(temperature,1);
    lcd.print(" H:");
    lcd.print(humidity,0);
    lcd.print("   ");

    lcd.setCursor(0,1);

    switch(alarmState){
      case SAFE:
        lcd.print("STATUS: SAFE    ");
        break;
      case PREWARN:
        lcd.print("STATUS: PREWARN ");
        break;
      case WARNING:
        lcd.print("STATUS: WARNING ");
        break;
      case DANGER:
        lcd.print("STATUS: DANGER  ");
        break;
    }
  }

  /* Blink */
  if(millis() - blinkTimer > 500){
    blinkTimer = millis();
    blinkState = !blinkState;
  }

  delay(1000);
}