#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#ifndef APSSID
#define APSSID "TriCorp_HQ"
#define APPSK  "25C6FS7EDY924"
#endif

int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 2000;
ESP8266WiFiMulti WiFiMulti;
const char fingerprint[] PROGMEM = "38edd6abfe187fb580b6e573825cf62250604053";
const String currentBinaryLocation = "https://esp-ota-poc.s3-eu-west-1.amazonaws.com/binaries/MACADDRESS/Blink.ino.bin";
String latestBinaryLocation = "https://esp-ota-poc.s3-eu-west-1.amazonaws.com/binaries/MACADDRESS/Blink.ino.bin";
WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(APSSID, APPSK);

  // Set the fingerprint to connect the server.
  client.setFingerprint(fingerprint);
}


void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval && WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("Time elapsed");
    
    HTTPClient http;
    if (http.begin(client, "https://esp-ota-poc.s3-eu-west-1.amazonaws.com/binaries/MACADDRESS/latest.txt")) {
      int httpCode = http.GET();
      if (httpCode > 0) {
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          latestBinaryLocation = http.getString().c_str();
          Serial.println("Found latest version: " + latestBinaryLocation);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
    
    if (currentBinaryLocation != latestBinaryLocation) {
      Serial.println("Found new binary");
      t_httpUpdate_return ret = ESPhttpUpdate.update(client, latestBinaryLocation);
      switch (ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;

        case HTTP_UPDATE_OK:
          Serial.println("HTTP_UPDATE_OK");
          break;
      }
    } else {
      Serial.println("Current binary is up-to-date");
    }
    
    previousMillis = millis();
  }
}
