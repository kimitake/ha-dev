#pragma once

#include <WiFiManager.h>

struct HAMqttConfig {
  char host[128];
  char port[6];
  char username[64];
  char password[64];
};

class HADeviceConfig {
public:
  bool connectWiFi(const char* apName);
  const HAMqttConfig& mqttConfig() const;
  bool startConfigPortal(const char* apName, bool autoConnect=false);
  void resetSettings();

private:
  HAMqttConfig mqttConfig_{};

  WiFiManagerParameter mqttHost{
    "mqtt_host", "MQTT Broker Host"};

  WiFiManagerParameter mqttPort{
    "mqtt_port", "MQTT Port"};

  WiFiManagerParameter mqttUsername{
    "mqtt_username", "MQTT Username"};

  WiFiManagerParameter mqttPassword{
    "mqtt_password", "MQTT Password",
    "", 0, "type=\"password\""};

  bool runConfigPortal(const char* apName, bool autoConnect);
  void copyParamsToMqttConfig();
  void copyParamsFromMqttConfig();
};
