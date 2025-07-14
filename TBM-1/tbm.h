#ifndef TBM_H
#define TBM_H

#include <Arduino.h>

// -------------------------------------------------------------------
//    PIN DEFINITIONS 
// -------------------------------------------------------------------

#define ESTOPCTRL_PIN    12
#define MOTORCTRL_PIN    13
#define PUMPCTRL_PIN     21
#define BENTCTRL_PIN     25

#define MOTORSENSE_PIN 22
#define PUMPSENSE_PIN 23
#define ESTOPSENSE_PIN 33
#define MOTOR_TEMP_PIN 34
#define PUMP_TEMP_PIN 35
#define GASSENSE_PIN 32
//#define FLOW_IN_PIN 12
//#define FLOW_OUT_PIN 13
// SPI for thermocouple
#define DO   18 
#define CS   19
#define CLK  5

#define SENSOR_API "/sensor"
#define CONFIG_API "/config"


const char* STOP = "stop";
const char* START = "start";
const char* RUNNING = "running";
const char* ERROR = "error";

typedef enum { 
    NO_MESSAGE = 0, 
    MESSAGE_TBM_INIT = 1,  
    TBM_START = 2,  
    TBM_STOP = 3,  
    TBM_ERROR = 4,  
    TBM_DATA = 5,  
} MessageID; 

MessageID incomingMessage = NO_MESSAGE; 
    
// -------------------------------------------------------------------
//    CONSTANTS/THRESHOLDS
// -------------------------------------------------------------------
static const float AREF           = 3.3f;
static const int   ADC_RESOLUTION = 4095;
static const float SHUNT_RESISTOR = 150.0f;
static const float maxTemp        = 50.0f; // Example threshold

// -------------------------------------------------------------------
//    STATE MACHINE
// -------------------------------------------------------------------
enum TBMState {
  STATE_CONFIG,
  STATE_RUNNING,
  STATE_STOP,
  STATE_ERROR
};

// -------------------------------------------------------------------
//    SENSOR & SYSTEM DATA STRUCTS
// -------------------------------------------------------------------
struct Sensor {
  bool         active;
  int          value;
  unsigned long timestamp;
};

struct sys_json {
  TBMState     state;
  Sensor       motor_temp;
  Sensor       flow_temp;
  Sensor       flow_in;
  Sensor       flow_out;
  Sensor       motor_power;
  Sensor       pump_power;
  Sensor       bentonite_power;
  Sensor       estop_button;
  Sensor       gas_sensor
  unsigned long global_time;
};

// -------------------------------------------------------------------
//    GLOBALS (declared extern here, defined in tbm1.ino)
// -------------------------------------------------------------------
extern sys_json  systemData;
extern TBMState  currentState;

// -------------------------------------------------------------------
//    FUNCTION PROTOTYPES
// -------------------------------------------------------------------
// eStop
// void eStop_setup();
// void eStop_loop();

// // JSON & system data
// void initSystemData();
// String constructJsonPayload();
// void sendJsonPayload(const String &payload);
// void JSON_loop();

// // Sensors
// void readSensors();
// float get_voltage(int raw_adc);
// float get_temperature(float voltage);
// float get_flowRate(float voltage);

// // State machine
// void state_setup();
// bool checkStopped();
// void updateSystemState();
// void state_loop();

// -------------------------------------------------------------------
//    HELPER: Convert TBMState enum -> string for JSON, etc.
// -------------------------------------------------------------------
inline const char* stateToString(TBMState s) {
  switch(s) {
    case STATE_CONFIG:  return "config";
    case STATE_RUNNING: return "running";
    case STATE_ERROR:   return "error";
    case STATE_STOP:    return "stop";
    default:            return "unknown";
  }
}

#endif // TBM_H
