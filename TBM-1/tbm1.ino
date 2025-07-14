#include <Arduino.h>
#include "tbm.h"
#include "sensors.ino"
#include "comms.ino"
#include "state.ino"
#include "eStop.ino"
#include "jsonSerialization.ino"
#include "receiving_ESP_comms.ino"

sys_json systemData;       

void setup() {
  Serial.begin(115200);
  
  pinMode(ESTOPCTRL_PIN, OUTPUT);
  pinMode(MOTORCTRL_PIN, OUTPUT);
  pinMode(PUMPCTRL_PIN, OUTPUT);
  pinMode(BENTCTRL_PIN, OUTPUT);

  pinMode(MOTORSENSE_PIN, INPUT);
  pinMode(PUMPSENSE_PIN, INPUT);
  pinMode(ESTOPSENSE_PIN, INPUT);
  pinMode(MOTOR_TEMP_PIN, INPUT);
  pinMode(PUMP_TEMP_PIN, INPUT);
  //pinMode(FLOW_IN_PIN,    INPUT);
  //pinMode(FLOW_OUT_PIN,   INPUT);



 
  initSystemData();


  state_setup();
  receivingESP_setup()

}

void loop(){

  eStop_loop();
  receivingESP_loop()


  readSensors();


  state_loop();


  JSON_loop();

  delay(1000); 
}
