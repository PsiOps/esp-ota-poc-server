#include <Arduino.h>
#include "src/EspOtaPoc.h"

EspOtaPoc Ota("robot001", "test_ota7.bin");

int ledState = LOW;
const long blinkWaitInterval = 500;

void setup() {
  Serial.begin(115200);
  Serial.println("Setting up");

  pinMode(LED_BUILTIN, OUTPUT);

  Ota.setupOta();
  
  Serial.println("Finished setting up");
}


void loop() {
  Ota.handleLoop();

  digitalWrite(LED_BUILTIN, LOW);
  delay(blinkWaitInterval);                   
  digitalWrite(LED_BUILTIN, HIGH);
  delay(blinkWaitInterval);   
}
