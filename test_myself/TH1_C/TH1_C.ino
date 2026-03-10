const int trigPin = 12;
const int echoPin = 13;

int ledPins[] = {3, 5, 6, 9, 10, 11}; 
int nLeds = 6;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  for (int i = 0; i < nLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  Serial.begin(9600);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  if (duration == 0) return;

  int distance = duration * 0.034 / 2;
  int brightness = map(distance, 3, 20, 255, 0);
  brightness = constrain(brightness, 0, 255);

  for (int i = 0; i < nLeds; i++) {
    analogWrite(ledPins[i], brightness);
  }

  Serial.print("distance:");
  Serial.print(distance);
  Serial.print(" brightness:");
  Serial.println(brightness);

  delay(100);
}
