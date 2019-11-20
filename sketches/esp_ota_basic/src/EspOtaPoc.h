/*
	Library containing stuff for updating ESP OTA
*/
#ifndef EspOtaPoc_h
#define EspOtaPoc_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h>

class EspOtaPoc {
	public:
		EspOtaPoc(String robotId, String currentBinary);
		void setupOta();
		void handleLoop();
};

#endif