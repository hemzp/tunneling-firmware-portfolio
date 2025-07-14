#include <Arduino.h>
#include "tbm.h"

#define DEBOUNCE_TIME 500 // in ms
int last_sample = 0; // time of last button sample

void eStop_loop() {
    int current_sample = millis();
    if (current_sample - last_sample > 500) {
      // Read the state of the emergency stop button
      int buttonState = digitalRead(ESTOPSENSE_PIN);
    
      if (buttonState == HIGH) {
          Serial.println("Emergency Stop Activated!");
          // TODO: add stop logic
          systemData.estop_button.value = 1;
      } else {
          systemData.estop_button.value = 0;
          //Serial.println("Estop button released. System Running Normally");
      }
    }
    
}
