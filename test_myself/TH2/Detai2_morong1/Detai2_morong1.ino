const byte NUM_LED = 6;
const byte ledPin[NUM_LED] = {3, 5, 6, 9, 10, 11};

const byte PWM_MAX = 255;

const float data_led[NUM_LED] = {
  0.08, 0.16, 0.32, 0.47, 0.70, 1.0
};

float ledLogic[NUM_LED] = {0};

uint8_t ledPWM[NUM_LED] = {0};

void setup() {
  for (byte i = 0; i < NUM_LED; i++) {
    pinMode(ledPin[i], OUTPUT);
  }
}

void loop() {

  for (int k = NUM_LED - 1; k > -NUM_LED; k--) {

    for (int i = NUM_LED - 1; i > 0; i--) {
      ledLogic[i] = ledLogic[i - 1];
    }
    ledLogic[0] = (k >= 0) ? data_led[k] : 0.0;

    for (byte i = 0; i < NUM_LED; i++) {
      ledPWM[i] = (uint8_t)(ledLogic[i] * PWM_MAX);
    }

    for (byte i = 0; i < NUM_LED; i++) {
      analogWrite(ledPin[i], ledPWM[i]);
    }

    delay(100);
  }
}
