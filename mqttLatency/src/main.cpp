#include <Arduino.h>
#define MQTT_SOCKET_TIMEOUT 1
#include <WiFi.h>
#include <PubSubClient.h>

const char* SSID = "VIVO-0070";
const char* PASSWORD = "m3VPvY9Uqz";
//const char* SSID = "DESKTOP-MG4B1AC 8576";
//const char* PASSWORD = "5Wf566=2";
//const char* broker = "192.168.15.3";
IPAddress broker(3,21,159,213);
const char* topic = "/hello";
int qos = 0;
const uint16_t port = 1883;
long lastReconnectAttempt = 0;
long lastTime = 0;
int numberOfMessages = 100;
int i = 0;
long t1 = 0;
long t2 = 0;

WiFiClient client;
PubSubClient mqttClient(client);

void calcLatency();
void messageHandler(char* topic, byte* payload, int length);
void setup_wifi();
boolean reconnect();

void setup() {
  Serial.begin(115200);
  setup_wifi();
  mqttClient.setServer(broker, port);
  mqttClient.setCallback(messageHandler);
  //reconnect();
}

void loop() {
  if (!mqttClient.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 1000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    mqttClient.loop();
    if (millis() - lastTime > 1000 && (i < numberOfMessages)) {
      lastTime = millis();
      if (t2 == 0) {
        mqttClient.publish(topic, "world");
        t2 = micros();
        i++;
      }
    }
  }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

boolean reconnect() {
  if (mqttClient.connect("esp")) {
    mqttClient.publish(topic, "hello world");
    mqttClient.subscribe(topic);
  }
  return client.connected();
}

void calcLatency() {
  double DeltaTs = ((t1 - t2) / 1000);
  double DeltaTs2 = DeltaTs / 2;
  
  Serial.print("TsE is: ");
  Serial.print(t1);
  Serial.print("µs TsB is: ");
  Serial.print(t2);
  Serial.print("µs Delta is: ");
  Serial.print(t1 - t2);
  Serial.print("µs Exchange Latency is: ");
  Serial.print(DeltaTs);
  Serial.print(" ms Sending Latency is: ");
  Serial.print(DeltaTs2);
  Serial.print(" ms");
  Serial.println();

  t2 = 0;
  t1 = 0;
}


void messageHandler(char* topic, byte* payload, int length) {
  if (t1 == 0 && t2 != 0) {
    t1 = micros();
    calcLatency();
  }
  else if (t2 != 0 && t1 != 0) {
    calcLatency();
  }
}
