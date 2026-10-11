#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Preferences.h>
#include <cstring>

#include "HADeviceConfig.h"

namespace {
  constexpr char PREF_NAMESPACE[] = "ha-device";

  void loadMqttConfig(HAMqttConfig& config) {
    Preferences prefs;

    if (!prefs.begin(PREF_NAMESPACE, true)) {
      Serial.println("Failed to open Preferences for reading");
      return;
    }

    prefs.getString("mqtt_host", config.host, sizeof(config.host));
    prefs.getString("mqtt_port", config.port, sizeof(config.port));
    prefs.getString("mqtt_username", config.username, sizeof(config.username));
    prefs.getString("mqtt_password", config.password, sizeof(config.password));

    prefs.end();
  }

  bool saveMqttConfig(const HAMqttConfig& config) {
    Preferences prefs;

    if (!prefs.begin(PREF_NAMESPACE, false)) {
      Serial.println("Failed to open Preferences for writing");
      return false;
    }

    if (config.host[0] == '\0' || config.port[0] == '\0') {
      Serial.println("MQTT Host and Port are required");
      prefs.end();
      return false;
    }

    prefs.putString("mqtt_host", config.host);
    prefs.putString("mqtt_port", config.port);
    prefs.putString("mqtt_username", config.username);
    prefs.putString("mqtt_password", config.password);

    // Read back and verify the saved values.
    HAMqttConfig saved{};
    prefs.getString("mqtt_host", saved.host, sizeof(saved.host));
    prefs.getString("mqtt_port", saved.port, sizeof(saved.port));
    prefs.getString("mqtt_username", saved.username, sizeof(saved.username));
    prefs.getString("mqtt_password", saved.password, sizeof(saved.password));

    prefs.end();

    return strcmp(config.host, saved.host) == 0 &&
           strcmp(config.port, saved.port) == 0 &&
           strcmp(config.username, saved.username) == 0 &&
           strcmp(config.password, saved.password) == 0;
  }

  bool hasMqttConfig() {
    Preferences prefs;

    if (!prefs.begin(PREF_NAMESPACE, true)) {
      return false;
    }

    bool configured =
      prefs.isKey("mqtt_host") &&
      prefs.isKey("mqtt_port");

    HAMqttConfig c;
    if (configured) {
      prefs.getString("mqtt_host", c.host, sizeof(c.host));
      prefs.getString("mqtt_port", c.port, sizeof(c.port));
      configured = c.host[0] != '\0' && c.port[0] != '\0';
    }
    prefs.end();
    return configured;
  }

  bool mqttConfigSaved = false;

  void saveConfigCallback() {
    mqttConfigSaved = true;
    Serial.println("WiFiManager save callback called");
  }
} // namespace

const HAMqttConfig& HADeviceConfig::mqttConfig() const {
  return mqttConfig_;
}

void HADeviceConfig::copyParamsToMqttConfig()
{
  strlcpy(mqttConfig_.host,
          mqttHost.getValue(),
          sizeof(mqttConfig_.host));

  strlcpy(mqttConfig_.port,
          mqttPort.getValue(),
          sizeof(mqttConfig_.port));

  strlcpy(mqttConfig_.username,
          mqttUsername.getValue(),
          sizeof(mqttConfig_.username));

  strlcpy(mqttConfig_.password,
          mqttPassword.getValue(),
          sizeof(mqttConfig_.password));
}

void HADeviceConfig::copyParamsFromMqttConfig()
{
  mqttHost.setValue(mqttConfig_.host, sizeof(mqttConfig_.host));
  mqttPort.setValue(mqttConfig_.port, sizeof(mqttConfig_.port));
  mqttUsername.setValue(mqttConfig_.username, sizeof(mqttConfig_.username));
  mqttPassword.setValue(mqttConfig_.password, sizeof(mqttConfig_.password));
}

bool HADeviceConfig::startConfigPortal(const char* apName, bool autoConnect) {
  Serial.println("startConfigPortal...");

  // load saved MQTT settings before creating the form field.
  loadMqttConfig(mqttConfig_);
  copyParamsFromMqttConfig();

  WiFiManager wm;

  wm.addParameter(&mqttHost);
  wm.addParameter(&mqttPort);
  wm.addParameter(&mqttUsername);
  wm.addParameter(&mqttPassword);
  
  wm.setSaveParamsCallback([&]() {
    Serial.println("Save callback");
    // need to save parameter, then restart the device
    copyParamsToMqttConfig();

    if (saveMqttConfig(mqttConfig_)) {
      Serial.println("MQTT settings saved successfully");
    } else {
      // FIXME: shouldn't reboot, right? then, what should I do?
      Serial.println("Failed to save MQTT settings");
    }

    wm.stopConfigPortal();
    Serial.println("stopConfigPortal");
  });

  bool connected = false;

  if (!hasMqttConfig() || !autoConnect) {
    bool status = wm.startConfigPortal(apName);
    Serial.printf("startConfigPortal status: %d\n", status);
  } else if (autoConnect) {
    connected = wm.autoConnect(apName);
    if (!connected) {
      Serial.println("Wi-Fi configuration failed");
      return false;
    }
  }

  return true;
}

bool HADeviceConfig::connectWiFi(const char* apName) {
  Serial.println("connectWiFi...");

  return startConfigPortal(apName, true);
}

void HADeviceConfig::resetSettings()
{
  WiFiManager wm;
  wm.resetSettings();
}