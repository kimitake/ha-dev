#include <Arduino.h>
#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

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

void loop() {
}
