#include "DHT.h"

#define DHTPIN 2        // Chân DATA nối vào D2
#define DHTTYPE DHT22   // Chọn loại cảm biến

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("DHT22 test...");
  dht.begin();
}

void loop() {
  delay(2000);  // DHT22 đọc mỗi 2 giây

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();      // °C
  float temperatureF = dht.readTemperature(true); // °F

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Loi doc cam bien!");
    return;
  }

  Serial.print("Do am: ");
  Serial.print(humidity);
  Serial.print(" %\t");

  Serial.print("Nhiet do: ");
  Serial.print(temperature);
  Serial.print(" °C  |  ");
  Serial.print(temperatureF);
  Serial.println(" °F");
}