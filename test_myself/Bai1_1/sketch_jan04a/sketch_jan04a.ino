void setup() {
  pinMode(A0, OUTPUT);   // Chân 13 là OUTPUT
  pinMode(A1, OUTPUT);
}
//chay cong nao thi dien ten cong do, A0
void loop() {
  digitalWrite(A0, HIGH); // Bật LED
  digitalWrite(A1, HIGH);
  delay(1000);            // Trễ 1 giây
  digitalWrite(A0, LOW);  // Tắt LED
  digitalWrite(A1, LOW);
  delay(1000);            // Trễ 1 giây
}
