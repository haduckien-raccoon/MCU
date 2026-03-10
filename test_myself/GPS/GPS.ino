#include <TinyGPS++.h>

TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("Dang kiem tra GPS...");
}

void loop() {
  while (Serial1.available()) {
    gps.encode(Serial1.read());
  }

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();

    Serial.println("===== GPS INFO =====");

    Serial.print("Chars processed : ");
    Serial.println(gps.charsProcessed());

    Serial.print("Satellites      : ");
    Serial.println(gps.satellites.value());

    Serial.print("HDOP            : ");
    Serial.println(gps.hdop.hdop());

    Serial.print("Location valid  : ");
    Serial.println(gps.location.isValid() ? "YES" : "NO");

    Serial.print("Latitude raw    : ");
    Serial.println(gps.location.lat(), 6);

    Serial.print("Longitude raw   : ");
    Serial.println(gps.location.lng(), 6);

    Serial.print("Time valid      : ");
    Serial.println(gps.time.isValid() ? "YES" : "NO");

    Serial.println("=====================");
  }
}
