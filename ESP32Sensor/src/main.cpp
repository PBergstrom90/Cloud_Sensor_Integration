#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "secrets.h"
#include "connect.h"
#include "sensor.h"

Connect aws;
Sensor sensor(2, DHT11);

void setup() {
  Serial.begin(115200);
    if (!LittleFS.begin()) {
        Serial.println("Failed to initialize LittleFS. Restarting...");
        delay(2000);
        ESP.restart();
    }
    if (!aws.connectAWS(WIFI_SSID, WIFI_PASSWORD, AWS_IOT_ENDPOINT)) {
        Serial.println("Failed to connect to AWS IoT. Restarting...");
        delay(2000);
        ESP.restart();
    }
  aws.setupShadow();
  Serial.println(" ");
  Serial.println("SETUP COMPLETE.");
}

void loop() {
 static unsigned long lastSendTime = 0;

  // Check Wi-Fi status
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(1000);
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi reconnect failed. Restarting...");
      delay(2000);
      ESP.restart();
    }
  }

  // Ensure MQTT client is connected
  if (!aws.getClient().connected()) {
    Serial.println("MQTT client disconnected. Reconnecting...");
    if (!aws.connectAWS(WIFI_SSID, WIFI_PASSWORD, AWS_IOT_ENDPOINT)) {
        Serial.println("Failed to reconnect to AWS IoT. Restarting...");
        delay(2000);
        ESP.restart();
      }
  }

  aws.processClient();
  
  if (millis() - lastSendTime >= 60000) { // Send every 60 seconds
    lastSendTime = millis();
    float temperature, humidity;
    sensor.readData(temperature, humidity);
    sensor.printData(temperature, humidity);
    if (!isnan(temperature) && !isnan(humidity)) {
      sensor.processAndSendData(aws, temperature, humidity);
    }
  }

  // DEBUG - Memory Management
  aws.memoryMonitor();
  if (ESP.getFreeHeap() < 5000) {
    Serial.println("Low memory! Restarting ESP32 to prevent issues.");
    delay(2000);
    ESP.restart();
  }
}