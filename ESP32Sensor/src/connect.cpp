#include "connect.h"

Connect::Connect() : client(1024), sendInterval(60000) {}

bool Connect::connectAWS(const char *ssid, const char *password, const char *endpoint) {
  if (!loadCertificates()) {
    Serial.println("ERROR: Failed to load certificates. Restarting...");
    delay(2000);
    ESP.restart();
    return false;
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Get the macAddress
  THINGNAME = WiFi.macAddress();
  // Remove colons from the MAC address string
  for (int i = 0; i < THINGNAME.length(); i++) {
    if (THINGNAME.charAt(i) == ':') {
      THINGNAME.remove(i, 1);
      i--;
    }
  }

  Serial.println();
  Serial.print("MAC Address: ");
  Serial.println(THINGNAME);
  Serial.println("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() > 10000) { // Timeout after 10 seconds
            Serial.println("ERROR: Failed to connect to Wi-Fi!");
            return false;
        }
  }
  // Connect to the MQTT broker on the AWS endpoint
  client.begin(AWS_IOT_ENDPOINT, 8883, net);
  // Create a message handler
  client.onMessage([this](String &topic, String &payload) { messageHandler(topic, payload); });
    
  Serial.println("Connecting to AWS IOT");
  unsigned long connectStart = millis();
  while (!client.connect(THINGNAME.c_str())) {
      Serial.print(".");
      delay(100);
      if (millis() - connectStart > 10000) {  // Timeout after 10 seconds
        Serial.println("ERROR: AWS IoT Connection Timeout. Restarting...");
        ESP.restart();
        return false;
      }
    }

  if (!client.connected()) {
    Serial.println("ERROR: AWS IoT Timeout!");
    return false;
  }

  // Subscribe to a topic
  client.subscribe(THINGNAME + AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("AWS IoT Connected!");
  return true;
}

void Connect::updateSettings(JsonDocument settingsObj) {
  sendInterval = settingsObj["sendIntervalSeconds"].as<int>() * 1000;
  JsonDocument docResponse;
  docResponse["state"]["reported"] = settingsObj;
  char jsonBuffer[512];
  serializeJson(docResponse, jsonBuffer);
  Serial.println("Sending reported state to AWS: ");
  serializeJson(docResponse, Serial);
  client.publish("$aws/things/" + THINGNAME + "/shadow/update", jsonBuffer);
}

void Connect::messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  JsonDocument doc;
  deserializeJson(doc, payload);

  // Handle device shadow
  if (topic.endsWith("/shadow/get/accepted")) {
    updateSettings(doc["state"]["desired"]);
  } else if (topic.endsWith("/shadow/update/delta")) {
    updateSettings(doc["state"]);
  }
}

bool Connect::loadCertificates() {
  // Load CA Certificate
  File ca = LittleFS.open("/certs/AmazonRootCA1.pem", "r");
  if (!ca) {
    Serial.println("ERROR: Failed to open CA certificate");
    return false;
  }
  size_t caSize = ca.size();
  if (!net.loadCACert(ca, caSize)) {
    Serial.println("ERROR: Failed to load CA certificate");
    ca.close();
    return false;
  }
  ca.close();

  // Load Client Certificate
  File clientCert = LittleFS.open("/certs/client-certificate.pem.crt", "r");
  if (!clientCert) {
    Serial.println("ERROR: Failed to open client certificate");
    return false;
  }
  size_t clientCertSize = clientCert.size();
  if (!net.loadCertificate(clientCert, clientCertSize)) {
    Serial.println("ERROR: Failed to load client certificate");
    clientCert.close();
    return false;
  }
  clientCert.close();

  // Load Private Key
  File privateKey = LittleFS.open("/certs/private.pem.key", "r");
  if (!privateKey) {
    Serial.println("ERROR: Failed to open private key");
    return false;
  }
  size_t privateKeySize = privateKey.size();
  if (!net.loadPrivateKey(privateKey, privateKeySize)) {
    Serial.println("ERROR: Failed to load private key");
    privateKey.close();
    return false;
  }
  privateKey.close();
  return true;
}

void Connect::setupShadow() {
  client.subscribe("$aws/things/" + THINGNAME + "/shadow/get/accepted");
  client.subscribe("$aws/things/" + THINGNAME + "/shadow/get/rejected");
  client.subscribe("$aws/things/" + THINGNAME + "/shadow/update/accepted");
  client.subscribe("$aws/things/" + THINGNAME + "/shadow/update/delta");
  client.publish("$aws/things/" + THINGNAME + "/shadow/get");
}

bool Connect::publishTelemetry(const char *payload) {
  Serial.println(" ");
  Serial.println("Publishing data: ");
  Serial.println(payload);
  return client.publish(THINGNAME + AWS_IOT_PUBLISH_TOPIC, payload);
}

void Connect::processClient() {
    client.loop();
}

MQTTClient& Connect::getClient() {
    return client;
}

// DEBUG Features
void Connect::listFiles() {
  Serial.println("Listing files in LittleFS:");
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    Serial.print("FILE: ");
    Serial.println(file.name());
    file = root.openNextFile();
  }
}

void Connect::memoryMonitor() {
    static unsigned long previousMillis = 0;
    if (millis() - previousMillis >= 60000) {
      previousMillis = millis();
      Serial.println("DEBUG: Heap Info");
      Serial.print("Total Heap: ");
      Serial.println(ESP.getHeapSize());
      Serial.print("Free Heap: ");
      Serial.println(ESP.getFreeHeap());
      }
}