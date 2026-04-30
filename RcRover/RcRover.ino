#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          // Library oleh tzapu
#include <PubSubClient.h>         // Library MQTT
#include <DHT.h>                  // Library Sensor Suhu
#include <LittleFS.h>             // Untuk penyimpanan memori internal

// --- KONFIGURASI MQTT ---
char mqtt_topic[40] = "rcrover01"; // Default Root Topic
const char* mqtt_broker = "broker.hivemq.com";

// --- PIN HARDWARE ---
#define DHTPIN D3
#define DHTTYPE DHT22
#define PIN_MQ135 A0
#define M_A1 D1 
#define M_A2 D2
#define M_B1 D5
#define M_B2 D6

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastMsg = 0;
bool shouldSaveConfig = false;

// Callback simpan konfigurasi
void saveConfigCallback() {
  shouldSaveConfig = true;
}

void stopMotors() {
  digitalWrite(M_A1, 0); digitalWrite(M_A2, 0);
  digitalWrite(M_B1, 0); digitalWrite(M_B2, 0);
}

// Handler perintah MQTT dari Dashboard
void callback(char* topic, byte* payload, unsigned int length) {
  char cmd = (char)payload[0];
  if(cmd == 'F') { digitalWrite(M_A1, 1); digitalWrite(M_A2, 0); digitalWrite(M_B1, 1); digitalWrite(M_B2, 0); }
  else if(cmd == 'B') { digitalWrite(M_A1, 0); digitalWrite(M_A2, 1); digitalWrite(M_B1, 0); digitalWrite(M_B2, 1); }
  else if(cmd == 'L') { digitalWrite(M_A1, 0); digitalWrite(M_A2, 1); digitalWrite(M_B1, 1); digitalWrite(M_B2, 0); }
  else if(cmd == 'R') { digitalWrite(M_A1, 1); digitalWrite(M_A2, 0); digitalWrite(M_B1, 0); digitalWrite(M_B2, 1); }
  else { stopMotors(); }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "RoverRivaldi-" + String(random(0, 999));
    if (client.connect(clientId.c_str())) {
      client.subscribe((String(mqtt_topic) + "/control").c_str());
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(M_A1, OUTPUT); pinMode(M_A2, OUTPUT);
  pinMode(M_B1, OUTPUT); pinMode(M_B2, OUTPUT);
  stopMotors();

  // Load konfigurasi topik dari LittleFS
  if (LittleFS.begin()) {
    if (LittleFS.exists("/config.txt")) {
      File configFile = LittleFS.open("/config.txt", "r");
      if (configFile) {
        String t = configFile.readStringUntil('\n');
        t.trim();
        if(t.length() > 0) t.toCharArray(mqtt_topic, 40);
        configFile.close();
      }
    }
  }

  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // Parameter Custom Topic di halaman WiFiManager
  WiFiManagerParameter custom_mqtt_topic("topic", "ID Rover (e.g. rcrover01)", mqtt_topic, 40);
  wifiManager.addParameter(&custom_mqtt_topic);

  // Nama AP diatur menjadi "RC Rover"
  if (!wifiManager.autoConnect("RC Rover")) {
    delay(3000);
    ESP.restart();
  }

  strcpy(mqtt_topic, custom_mqtt_topic.getValue());

  // Simpan jika ada perubahan topik baru
  if (shouldSaveConfig) {
    File configFile = LittleFS.open("/config.txt", "w");
    if (configFile) {
      configFile.println(mqtt_topic);
      configFile.close();
    }
  }

  client.setServer(mqtt_broker, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // Kirim data sensor setiap 2 detik
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    float t = dht.readTemperature();
    int g = analogRead(PIN_MQ135);
    String payload = "{\"t\":" + String(isnan(t)?0:t) + ",\"g\":" + String(g) + "}";
    client.publish((String(mqtt_topic) + "/data").c_str(), payload.c_str());
  }
}