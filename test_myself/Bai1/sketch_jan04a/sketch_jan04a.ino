void setup() {
  pinMode(13, OUTPUT);   // Chân 13 là OUTPUT
  // pinMode(A1, OUTPUT);
}

void loop() {
  digitalWrite(13, HIGH); // Bật LED
  // digitalWrite(A1, HIGH);
  delay(1000);            // Trễ 1 giây
  digitalWrite(13, LOW);  // Tắt LED
  // digitalWrite(A1, LOW);
  delay(1000);            // Trễ 1 giây
}
