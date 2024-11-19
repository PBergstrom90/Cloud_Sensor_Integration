#include "sensor.h"

Sensor::Sensor(uint8_t pin, uint8_t type) : dht(pin, type) {}

void Sensor::readData(float &temperature, float &humidity) {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println(" ");
    Serial.println("Failed to read from DHT sensor!");
    Serial.println("No data will be sent to AWS.");
  }
}

void Sensor::processAndSendData(Connect &connect, float &temperature, float &humidity) {
// Validate the sensor data
  if (temperature < -40 || temperature > 50 || humidity < 0 || humidity > 100) {
    Serial.println(" ");
    Serial.println("Unrealistic sensor readings detected. Not sending data.");
    return;
  }
  char payload[100];
  snprintf(payload, sizeof(payload), "{\"temperature\":%.2f,\"humidity\":%.2f}", temperature, humidity);
  if (!connect.publishTelemetry(payload)) {
        Serial.println("Failed to send telemetry data. Restarting...");
        ESP.restart();
    }
}

void Sensor::printData(float &temperature, float &humidity) {
  Serial.println(" ");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
}