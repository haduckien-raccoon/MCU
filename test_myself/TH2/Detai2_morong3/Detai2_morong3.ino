const byte NUM_LED = 6;
const byte ledPin[NUM_LED] = {3, 5, 6, 9, 10, 11};

const byte bientro = A0;
const byte PWM_MAX = 255;

const float tailPattern[] = {1.0, 0.69, 0.25};

int headPos = 0;

void setup() {
  Serial.begin(9600);

  for (byte i = 0; i < NUM_LED; i++) {
    pinMode(ledPin[i], OUTPUT);
  }
}

void loop() {
  int pot = analogRead(bientro);         
  float voltage = pot * (5.0 / 1023.0);   

  int delayTime = map(pot, 0, 1023, 20, 250);

  for (byte i = 0; i < NUM_LED; i++) {

    int d = headPos - i;
    int pwm = 0;

    if (d >= 0 && d < 3) {
      pwm = (int)(tailPattern[d] * PWM_MAX);
    }

    analogWrite(ledPin[i], pwm);
  }

  headPos++;
  if (headPos >= NUM_LED + 3) {
    headPos = 0;
  }

  Serial.print("ADC: ");
  Serial.print(pot);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 2);
  Serial.print(" V | Delay: ");
  Serial.print(delayTime);
  Serial.println(" ms");

  Serial.print(voltage);
  Serial.print("\t");
  Serial.println(delayTime);

  delay(delayTime);
}
