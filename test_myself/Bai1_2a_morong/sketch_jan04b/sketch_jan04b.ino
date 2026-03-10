int leds[8] = {2,3,4,5,6,7,8,9};

void setup() {
  for(int i=0;i<8;i++){
    pinMode(leds[i], OUTPUT);
  }
}
void loop() {
  for(int i=0;i<8;i++){
    // Tắt hết
    for(int j=0;j<8;j++){
      digitalWrite(leds[j], LOW);
    }

    // Bật 3 LED liên tiếp
    digitalWrite(leds[i], HIGH);
    digitalWrite(leds[(i+1)%8], HIGH);
    digitalWrite(leds[(i+2)%8], HIGH);

    delay(300);
  }
}
