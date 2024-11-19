#ifndef CONNECT_H
#define CONNECT_H

#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <LittleFS.h>
#include <WiFi.h>
#include "secrets.h"
#include "sensor.h"

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC "/telemetry"
#define AWS_IOT_SUBSCRIBE_TOPIC "/downlink"

class Sensor;

class Connect {
private: 
    String THINGNAME;
    WiFiClientSecure net;
    MQTTClient client;
    long sendInterval; // Interval at which to send to AWS
    bool loadCertificates(); 
public:
    Connect();
    bool connectAWS(const char *ssid, const char *password, const char *endpoint);
    void updateSettings(JsonDocument settingsObj);
    void messageHandler(String &topic, String &payload);
    void setupShadow();
    bool publishTelemetry(const char *payload);
    void processClient();
    MQTTClient& getClient();
    void listFiles();
    void memoryMonitor();
};

#endif