// Hello world
// #include <Arduino.h>

// void setup() {
//     Serial.begin(115200);
//     delay(1000);

//     Serial.println("Hello from M5Stack ATOM Lite!");
// }

// void loop() {
//     Serial.println("Running...");
//     delay(1000);
// }

// Wi-Fi setting
// #include <Arduino.h>
// #include <WiFi.h>

// void setup() {
//     Serial.begin(115200);
//     delay(1000);

//     Serial.println("Connecting to Wi-Fi...");

//     WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }

//     Serial.println();
//     Serial.println("Wi-Fi connected!");
//     Serial.print("IP address: ");
//     Serial.println(WiFi.localIP());
// }

// void loop() {
// }

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void connectWiFi() {
    Serial.println("Connecting to Wi-Fi...");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

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

    connectWiFi();
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
