#include <Arduino.h>
#include <WiFi.h>
#include <HADeviceConfig.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

HADeviceConfig deviceConfig;

void connectMQTT() {
    Serial.println("Connecting to MQTT...");

    mqttClient.setServer(MQTT_HOST, MQTT_PORT);

    while (!mqttClient.connected()) {
        if (mqttClient.connect("atom-lite", MQTT_USER, MQTT_PASSWORD)) {
            Serial.println("MQTT connected!");
        } else {
            Serial.print("MQTT connection failed, state=");
            Serial.println(mqttClient.state());
            delay(2000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    if (!deviceConfig.connectWiFi("Atom-Lite-Setup")) {
        ESP.restart();
    }

    connectMQTT();
}

void loop() {
    if (!mqttClient.connected()) {
        connectMQTT();
    }

    mqttClient.loop();

    mqttClient.publish("atom-lite/test", "hello");

    Serial.println("MQTT: published hello");

    delay(10000);
}
