#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("Hello from M5Stack ATOM Lite!");
}

void loop() {
    Serial.println("Running...");
    delay(1000);
}
