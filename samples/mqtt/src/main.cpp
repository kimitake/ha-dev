#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HADeviceConfig.h>
#include <cstdlib>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

HADeviceConfig deviceConfig;

const char* apName = "Atom-Lite-Setup";
const char* mqttName = "atom-lite";

bool connectMQTT() {
  const HAMqttConfig& config = deviceConfig.mqttConfig();

  if (config.host[0] == '\0' || config.port[0] == '\0') {
    Serial.println("MQTT configuration is imcomplete");
    return false;
  }

  Serial.println("Connecting to MQTT...");
  mqttClient.setServer(config.host, atoi(config.port));

  bool connected;
  if (config.username[0] != '\0') {
    connected = mqttClient.connect(
      mqttName,
      config.username,
      config.password
    );
  } else {
    connected = mqttClient.connect(mqttName);
  }

  if (connected) {
    Serial.println("MQTT connected");
    return true;
  }

  Serial.print("MQTT connection failed, state=");
  Serial.println(mqttClient.state());
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!deviceConfig.connectWiFi(apName)) {
    ESP.restart();
  }

  // connectMQTT();
}

void handleSerialCommand() {
  if (Serial.available() <= 0) {
    return;
  }

  String command = Serial.readStringUntil('\n');
  command.trim();

  Serial.print("Received command: ");
  Serial.println(command);

  if (command == "help") {
    Serial.println("Available command:");
    Serial.println("  help          - Show this help");
    Serial.println("  config        - Open configuration portal");
    Serial.println("  reset-wifi    - Clear Wi-Fi settings");
    Serial.println("  reset-mqtt    - Clear MQTT settings");
    Serial.println("  factory-reset - Clear all settings");
    Serial.println("  reboot        - Restart the device");
  } else if (command == "config") {
    Serial.println("Configuration portal requested");    
    deviceConfig.startConfigPortal(apName);
  } else if (command == "reboot") {
    Serial.println("Rebooting...");
    delay(100);
    ESP.restart();
  } else if (command == "reset-wifi") {
    Serial.println("Restting Wi-Fi setting...");
    deviceConfig.resetSettings();
  } else {
    Serial.print("Unknown command: ");
    Serial.println(command);
    Serial.println("Type 'help' for available commands.");
  }
}

void loop() {
  // if (!mqttClient.connected()) {
  //   connectMQTT();
  // }

//  mqttClient.loop();
//  mqttClient.publish("atom-lite/test", "hello");
//  Serial.println("MQTT: published hello");

  handleSerialCommand();

  delay(10000);
}
