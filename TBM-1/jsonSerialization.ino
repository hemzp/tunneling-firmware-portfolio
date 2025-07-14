#include <Arduino.h>
#include <ArduinoJson.h>
#include "tbm.h"

// ---------------------------------------------------------
//  Initialize all fields of systemData
// ---------------------------------------------------------
void initSystemData() {
  systemData.state         = STATE_STOP;
  systemData.global_time   = 0;

  systemData.motor_temp    = { true, 0, 0 };
  systemData.flow_temp     = { true, 0, 0 };
  systemData.flow_in       = { true, 0, 0 };
  systemData.flow_out      = { true, 0, 0 };
  systemData.motor_power   = { true, 0, 0 };
  systemData.pump_power    = { true, 0, 0 };
  systemData.bentonite_power  = { true, 0, 0 };
  systemData.estop_button  = { true, 0, 0 };
  systemData.gas_sensor = { true, 0, 0}; 
}

// ---------------------------------------------------------
//  Build JSON string from systemData
// ---------------------------------------------------------
String constructJsonPayload() {
  StaticJsonDocument<512> doc;

  // Use helper to convert enum to string
  doc["state"]       = stateToString(systemData.state);
  doc["global_time"] = systemData.global_time;

  JsonObject motorTempObj = doc.createNestedObject("motor_temp");
  motorTempObj["active"]    = systemData.motor_temp.active;
  motorTempObj["value"]     = systemData.motor_temp.value;
  motorTempObj["timestamp"] = systemData.motor_temp.timestamp;

  // Repeat pattern for other sensors:
  JsonObject flowTempObj = doc.createNestedObject("flow_temp");
  flowTempObj["active"]    = systemData.flow_temp.active;
  flowTempObj["value"]     = systemData.flow_temp.value;
  flowTempObj["timestamp"] = systemData.flow_temp.timestamp;

  // ... flow_in, flow_out, motor_power, pump_power, etc.
  JsonObject gasSensorObj = doc.createNestedObject("gas_sensor"); 
  gasSensorObj["active"] = systemData.gas_sensor.active; 
  gasSensorObj["value"]     = systemData.gas_sensor.value;
  gasSensorObj["timestamp"] = systemData.gas_sensor.timestamp;

  // Done, serialize
  String output;
  serializeJson(doc, output);
  return output;
}

// ---------------------------------------------------------
//  Example send function (currently just prints to Serial)
// ---------------------------------------------------------
void sendJsonPayload(const String &payload) {
  Serial.println(payload);
}

// ---------------------------------------------------------
//  JSON_loop: read the constructed JSON and send it out
// ---------------------------------------------------------
void JSON_loop() {
  // Typically you'd do readSensors() & updateSystemState() 
  // in your main loop or state loop. 
  // But if you want to handle them here, you can — be consistent.

  String payload = constructJsonPayload();
  sendJsonPayload(payload);
}
