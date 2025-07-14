#include <Arduino.h>
#include "tbm.h"
#include <HardwareSerial.h>

void setup() {
    Serial.begin(9600);    // Debugging
    Serial2.begin(115200, SERIAL_8N1, 16, 17); // TX = GPIO 16, RX = GPIO 17
}
 
void loop() {
    if (Serial2.available()) {
        String receivedData = Serial2.readStringUntil('\n'); // Read temperature as string
        double temperature = receivedData.toDouble(); // Convert to double
 
        Serial.print("Received Temperature: ");
        Serial.println(temperature, 5);
    }
}
