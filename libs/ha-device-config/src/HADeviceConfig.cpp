#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include "HADeviceConfig.h"

bool HADeviceConfig::connectWiFi(const char* apName) {
  Serial.println("Starting WiFiManager...");

  WiFiManager wm;

  Serial.println("Before autoConnect");

  if (!wm.autoConnect(apName)) {
    Serial.println("Wi-Fi configuration failed");
    return false;
  }

  Serial.println("After autoConnect");
  Serial.println("Wi-Fi connected!");
  Serial.print("IP address:");
  Serial.println(WiFi.localIP());

  return true;
}
