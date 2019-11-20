#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h>
#include "EspOtaPoc.h"

String RobotId;
String _currentBinary;
String _latestBinary;

EspOtaPoc::EspOtaPoc(String robotId, String currentBinary)
{
	RobotId = robotId;
	_currentBinary = currentBinary;
	_latestBinary = currentBinary;
}


char accessPointSSID[30] = "";
char accessPointKey[30] = "";

unsigned long previousMillis = 0;
const long interval = 10000;

const char fingerprint[] PROGMEM = "2bb36b4815c37e5fd6ae05ffd8d8d5897a4a4834";
const String binaryLocation = "https://esp-ota-binaries.s3-us-east-2.amazonaws.com/binaries";

ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server(80);
WiFiClientSecure client;

void setupWifiStationMode(){
  WiFi.mode(WIFI_STA);

  if(accessPointSSID[0] != '\0') {
    Serial.printf("Adding Access Point with SSID: ");
    Serial.println(accessPointSSID);
    WiFiMulti.addAP(accessPointSSID, accessPointKey);  
  }
}

const char *page = 
  "<h1>You are connected to robot001</h1>"
  "<form method='post' enctype='application/x-www-form-urlencoded' action='postform/'>"
    "<input type='text' name='SSID' >"
    "<input type='text' name='Key' >"
    "<input type='submit' value='Submit'>"
  "</form>";

// http://192.168.4.1 
void handleRoot() {
  server.send(200, "text/html", page);
}

void handleForm() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    WiFi.disconnect();
    server.arg(0).toCharArray(accessPointSSID, 30);
    server.arg(1).toCharArray(accessPointKey, 30);
    WiFiMulti.addAP(accessPointSSID, accessPointKey);

    String message = "Connected!";
    server.send(200, "text/plain", message);
  }
}

void setupWifiAccessPointMode(){
  WiFi.softAP(RobotId);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/postform/", handleForm);
  server.begin();
}

void setupWifiClient(){
  client.setFingerprint(fingerprint); // Set the SSL fingerprint for HTTPS S3 file downloads
}

void EspOtaPoc::setupOta(){
	setupWifiStationMode();
	setupWifiAccessPointMode();
	setupWifiClient();
}

void checkForUpdates(){
	Serial.println("Time elapsed");
	
	HTTPClient http;
	Serial.println("Getting latest id from: " + binaryLocation + "/" + RobotId + "/latest.txt");
	if (http.begin(client, binaryLocation + "/" + RobotId + "/latest.txt")) {
		int httpCode = http.GET();
		if (httpCode > 0) {
			// file found at server
			if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
			_latestBinary = http.getString();
			Serial.print("Latest binary: ");
			Serial.println(_latestBinary);
			}
		} else {
			Serial.printf("[HTTP] GET... failed, error: %s", http.errorToString(httpCode).c_str());
		}
	} else {
		Serial.printf("[HTTP} Unable to connect");
	}
	http.end();
	
	if (_currentBinary != _latestBinary) {
		Serial.println("Proceed to update to latest binary");
		
		t_httpUpdate_return ret = ESPhttpUpdate.update(client, binaryLocation + "/" + RobotId + "/" + _latestBinary);
		switch (ret) {
			case HTTP_UPDATE_FAILED:
			Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
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
	
}

void EspOtaPoc::handleLoop(){
	server.handleClient();
	unsigned long currentMillis = millis();
	if (currentMillis - previousMillis >= interval && WiFiMulti.run() == WL_CONNECTED) {
		checkForUpdates();
		previousMillis = millis();
	}
}

