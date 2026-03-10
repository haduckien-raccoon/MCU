int leds[8] = {2,3,4,5,6,7,8,9};

void setup() {
  for(int i=0;i<8;i++)
    pinMode(leds[i], OUTPUT);
}

void loop() {
  // Tăng dần
  for(int i=0;i<8;i++){
    digitalWrite(leds[i], HIGH);
    delay(200);
  }

  // Giảm dần
  for(int i=7;i>=0;i--){
    digitalWrite(leds[i], LOW);
    delay(200);
  }
}
