const byte NUM_LED = 6;
const byte ledPin[NUM_LED] = {3, 5, 6, 9, 10, 11};
const byte bientro = A0;

void setup() {
  Serial.begin(9600);

  for (byte i = 0; i < NUM_LED; i++) {
    pinMode(ledPin[i], OUTPUT);
  }
}

void loop() {
  int adcValue = analogRead(bientro);

  float voltage = adcValue * (5.0 / 1023.0);

  int brightness = map(adcValue, 0, 1023, 0, 255);

  for (byte i = 0; i < NUM_LED; i++) {
    analogWrite(ledPin[i], brightness);
  }

  Serial.print("ADC: ");
  Serial.print(adcValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");

  Serial.print(0);
  Serial.print(" ");
  Serial.print(5);
  Serial.print(" ");
  Serial.println(voltage);

  delay(50);
}
