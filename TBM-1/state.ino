#include <Arduino.h>
#include "tbm.h"

// ---------------------------------------------------------
//  Setup for state machine
// ---------------------------------------------------------
void state_setup() {
  Serial.println("state_setup: Entering CONFIG");
  systemData.state = STATE_CONFIG;
}

// ---------------------------------------------------------
//  Check if all outputs are physically off
// ---------------------------------------------------------
bool checkStopped() {
  if (digitalRead(MOTORCTRL_PIN) != LOW)  return false;
  if (digitalRead(PUMPCTRL_PIN)  != LOW)  return false;
  if (digitalRead(BENTCTRL_PIN)  != LOW)  return false;
  return true;
}
// ---------------------------------------------------------
//  Shuts off TBM motor, estop, bentonite pump, and water pump 
// ---------------------------------------------------------
void stoppingTBM() { 
  digitalWrite(ESTOPCTRL_PIN, HIGH);
  digitalWrite(MOTORCTRL_PIN, LOW);
  digitalWrite(PUMPCTRL_PIN, LOW);
  digitalWrite(BENTCTRL_PIN, LOW);
}
// ---------------------------------------------------------
//  Finite state machine for TBM 
//  (Transitions between CONFIG, RUNNING, ERROR, STOP)
// ---------------------------------------------------------
void state_loop() {
  switch(systemData.state) {
    case STATE_CONFIG:
      Serial.println("STATE_CONFIG: Checking if system is stopped...");
      systemData.state = STATE_RUNNING;
      break;

    case STATE_RUNNING:
      if (systemData.motor_temp.value >= maxTemp || systemData.estop_button.value == 1) {
        stoppingTBM(); 
        systemData.state = STATE_STOP;
        break;  
      } 
      digitalWrite(ESTOPCTRL_PIN, LOW);
      digitalWrite(MOTORCTRL_PIN, HIGH);
      digitalWrite(PUMPCTRL_PIN, HIGH);
      digitalWrite(BENTCTRL_PIN, HIGH);
      Serial.printf("RUNNING: Motor temp = %d C\n", systemData.motor_temp.value);
      break;

    case STATE_STOP:
      Serial.println("STATE_STOP: Checking if physically off...");
      int i = 0;
      while (!checkStopped() || i != 100) {
        stoppingTBM(); 
        i++;  
      } 
      if (!checkStopped()) { 
        Serial.println("TRIED 100 TIMES TO STOP TBM - PULL THE PLUG!"); 
      }
      if (systemData.estop_button.value == 0 && systemData.motor_temp.value < maxTemp) {
        systemData.state = STATE_CONFIG;
      }
      break;
  }
}
