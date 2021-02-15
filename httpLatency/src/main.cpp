#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* SSID = "VIVO-0070";
const char* PASSWORD = "m3VPvY9Uqz";
//const char* SSID = "DESKTOP-MG4B1AC 8576";
//const char* PASSWORD = "5Wf566=2";
//String url = "http://192.168.15.3:8000/";
String url = "http://3.21.159.213:8000/";
long lastReconnectAttempt = 0;
long lastTime = 0;
int numberOfMessages = 100;
int i = 0;
long t1 = 0;
long t2 = 0;

HTTPClient http;

void calcLatency();
void connectWiFi();
void post(String payload);

void setup() {
  Serial.begin(115200);
  connectWiFi();
}

void loop() {
  if (millis() - lastTime > 1000 && (i < numberOfMessages)) {
    lastTime = millis();
    if (t2 == 0) {
      post("{\"hello\":\"world\"}");
      i++;
    }
  }
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
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

void post(String payload)
{
    http.begin(url);
    //http.addHeader("Content-Type", "application/json");
    //http.setConnectTimeout(100);
    //Serial.print("[HTTP] GET...\n");
    t2 = micros();
    int httpCode = http.GET();
    if(httpCode > 0) 
    {
        t1 = micros();
        calcLatency();
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        if(httpCode == HTTP_CODE_OK) 
        {
            String payload = http.getString();
            Serial.println(payload);
        }
    }
    else 
    {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}