#include <HardwareSerial.h>
#include <Arduino.h>
#include "tbm.h"


HardwareSerial mySerial(1);  // Use UART1

void receivingESP_setup()
    Serial.begin(115200);
    mySerial.begin(9600, SERIAL_8N1, -1, 16); // RX on GPIO16, no TX needed

    Serial.println("ESP32 UART Receiver Started");
} 

void receivingESP_loop() {
    if (mySerial.available()) {
        String received = mySerial.readStringUntil('\n');
        Serial.print("Received: ");
        Serial.println(received);
        String temp = received[0:10] 
        String gas = received [11:20]
    }
}
