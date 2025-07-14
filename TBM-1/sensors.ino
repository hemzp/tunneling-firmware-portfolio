#include <Arduino.h>
#include "tbm.h"
#include <SPI.h> //enables communication between ESP32 and MAX31855 amp
#include "Adafruit_MAX31855.h" // need to install this package, depends on above as well

// ---------------------------------------------------------
//  Helpers for ADC, temperature, flow, etc.
// ---------------------------------------------------------

#define SHUNT_RESISTOR 150.0 // Resistor value in ohms

#define AREF 3.3             // ADC reference voltage for ESP32
#define ADC_RESOLUTION 4095  // 12-bit ADC resolution
// 
/*
#define DO   18 
#define CS   19
#define CLK  5

Set up pins in tbm.h
*/

Adafruit_MAX31855 thermocouple(CLK, CS, DO); //initialize thermocouple object
// define a new pin for chip select - two temp readings - refactor code above using SPI. 
void sensorPinSetup() {
  Serial.begin(9600);
  // no longer reading from pin for temperature
  //pinMode(FLOW_IN_PIN, INPUT); // Set the thermocouple pin as input
}
float get_voltage(int raw_adc) {
  return (raw_adc * AREF) / ADC_RESOLUTION;;  
}

// found this code online 
// https://learn.adafruit.com/calibrating-sensors/maxim-31855-linearization
// https://github.com/heypete/MAX31855-Linearization/tree/master
// returns the temperature read from the thermocouple amplifier
double get_temperature() {
  int i = 0; // Counter for arrays
  double internalTemp = thermocouple.readInternal(); // Read the internal temperature of the MAX31855.
  double rawTemp = thermocouple.readCelsius(); // Read the temperature of the thermocouple. This temp is compensated for cold junction temperature.
  double thermocoupleVoltage= 0;
  double internalVoltage = 0;
  double correctedTemp = 0;

  // Check to make sure thermocouple is working correctly.
  if (isnan(rawTemp)) {
    Serial.println("Something wrong with thermocouple!");
    return NAN;
  }
  else {
    // Steps 1 & 2. Subtract cold junction temperature from the raw thermocouple temperature.
    thermocoupleVoltage = (rawTemp - internalTemp)*0.041276;  // C * mv/C = mV
    // Step 3. Calculate the cold junction equivalent thermocouple voltage.
    if (internalTemp >= 0) { // For positive temperatures use appropriate NIST coefficients
      // Coefficients and equations available from http://srdata.nist.gov/its90/download/type_k.tab
      double c[] = {-0.176004136860E-01,  0.389212049750E-01,  0.185587700320E-04, -0.994575928740E-07,  0.318409457190E-09, -0.560728448890E-12,  0.560750590590E-15, -0.320207200030E-18,  0.971511471520E-22, -0.121047212750E-25};
      
      // Count the the number of coefficients. There are 10 coefficients for positive temperatures (plus three exponential coefficients),
      // but there are 11 coefficients for negative temperatures.
      int cLength = sizeof(c) / sizeof(c[0]);

      // Exponential coefficients. Only used for positive temperatures.
      double a0 =  0.118597600000E+00;
      double a1 = -0.118343200000E-03;
      double a2 =  0.126968600000E+03;

      // From NIST: E = sum(i=0 to n) c_i t^i + a0 exp(a1 (t - a2)^2), where E is the thermocouple voltage in mV and t is the temperature in degrees C.
      // In this case, E is the cold junction equivalent thermocouple voltage.
      // Alternative form: C0 + C1*internalTemp + C2*internalTemp^2 + C3*internalTemp^3 + ... + C10*internaltemp^10 + A0*e^(A1*(internalTemp - A2)^2)
      // This loop sums up the c_i t^i components.
      for (i = 0; i < cLength; i++) {
        internalVoltage += c[i] * pow(internalTemp, i);
      }
      // This section adds the a0 exp(a1 (t - a2)^2) components.
      internalVoltage += a0 * exp(a1 * pow((internalTemp - a2), 2));
    }
    else if (internalTemp < 0) { // for negative temperatures
      double c[] = {0.000000000000E+00,  0.394501280250E-01,  0.236223735980E-04, -0.328589067840E-06, -0.499048287770E-08, -0.675090591730E-10, -0.574103274280E-12, -0.310888728940E-14, -0.104516093650E-16, -0.198892668780E-19, -0.163226974860E-22};
      // Count the number of coefficients.
      int cLength = sizeof(c) / sizeof(c[0]);

      // Below 0 degrees Celsius, the NIST formula is simpler and has no exponential components: E = sum(i=0 to n) c_i t^i
      for (i = 0; i < cLength; i++) {
        internalVoltage += c[i] * pow(internalTemp, i) ;
      }
    }
    // Step 4. Add the cold junction equivalent thermocouple voltage calculated in step 3 to the thermocouple voltage calculated in step 2.
    double totalVoltage = thermocoupleVoltage + internalVoltage;

    // Step 5. Use the result of step 4 and the NIST voltage-to-temperature (inverse) coefficients to calculate the cold junction compensated, linearized temperature value.
    // The equation is in the form correctedTemp = d_0 + d_1*E + d_2*E^2 + ... + d_n*E^n, where E is the totalVoltage in mV and correctedTemp is in degrees C.
    // NIST uses different coefficients for different temperature subranges: (-200 to 0C), (0 to 500C) and (500 to 1372C).
    if (totalVoltage < 0) { // Temperature is between -200 and 0C.
      double d[] = {0.0000000E+00, 2.5173462E+01, -1.1662878E+00, -1.0833638E+00, -8.9773540E-01, -3.7342377E-01, -8.6632643E-02, -1.0450598E-02, -5.1920577E-04, 0.0000000E+00};
      int dLength = sizeof(d) / sizeof(d[0]);
      for (i = 0; i < dLength; i++) {
        correctedTemp += d[i] * pow(totalVoltage, i);
      }
    }
    else if (totalVoltage < 20.644) { // Temperature is between 0C and 500C.
      double d[] = {0.000000E+00, 2.508355E+01, 7.860106E-02, -2.503131E-01, 8.315270E-02, -1.228034E-02, 9.804036E-04, -4.413030E-05, 1.057734E-06, -1.052755E-08};
      int dLength = sizeof(d) / sizeof(d[0]);
      for (i = 0; i < dLength; i++) {
        correctedTemp += d[i] * pow(totalVoltage, i);
      }
    }
    else if (totalVoltage < 54.886 ) { // Temperature is between 500C and 1372C.
      double d[] = {-1.318058E+02, 4.830222E+01, -1.646031E+00, 5.464731E-02, -9.650715E-04, 8.802193E-06, -3.110810E-08, 0.000000E+00, 0.000000E+00, 0.000000E+00};
      int dLength = sizeof(d) / sizeof(d[0]);
      for (i = 0; i < dLength; i++) {
        correctedTemp += d[i] * pow(totalVoltage, i);
      }
    } else { // NIST only has data for K-type thermocouples from -200C to +1372C. If the temperature is not in that range, set temp to impossible value.
      // Error handling should be improved.
      Serial.print("Temperature is out of range. This should never happen.");
      correctedTemp = NAN;
    }
    return correctedTemp;
  }
}

float get_flowRate(float voltage){
  // Flow rate formula for liquid flow meter LVB-15-A Liquid flow meter [3.5 .. 50 l/min]

  // first must convert voltage to current
  float current_mA = (voltage / SHUNT_RESISTOR) * 1000;
  // formula
  return 3.125 * (current_mA - 4);
}

// the values for this are not the ones we shd be using. - will figure out values soon 
float get_gasConcentration(int value) { 
  const float A = 60.0; 
  const float V_REF = 3.3; 
  const float B = -1.0; 
  float voltage = (value / 4095.0) * V_REF; 
  return (A * pow((voltage / V_REF), B) / 100);
} 


// ---------------------------------------------------------
//  Read all sensors -> update systemData fields
// ---------------------------------------------------------
void readSensors() {
  // 1. Motor temperature
  double motorTempC      = get_temperature();

  systemData.motor_temp.value     = (int)motorTempC;
  systemData.motor_temp.timestamp = millis();

  // 2. Pump temperature => "flow_temp" in your struct
  int   raw_pump        = analogRead(PUMP_TEMP_PIN);
  float volt_pump       = get_voltage(raw_pump);
  float pumpTempC       = get_temperature();

  systemData.flow_temp.value      = (int)pumpTempC;
  systemData.flow_temp.timestamp  = millis();

  // 3. Flow in
  //int   raw_flowIn      = analogRead(FLOW_IN_PIN);
  //float volt_flowIn     = get_voltage(raw_flowIn);
  //float flowRateIn      = get_flowRate(volt_flowIn);
  float flowRateIn = 10;
  systemData.flow_in.value        = (int)flowRateIn;
  systemData.flow_in.timestamp    = millis();

  // 4. Flow out
  //int   raw_flowOut     = analogRead(FLOW_OUT_PIN);
  //float volt_flowOut    = get_voltage(raw_flowOut);
  //float flowRateOut     = get_flowRate(volt_flowOut);
  float flowRateOut = 10;
  systemData.flow_out.value       = (int)flowRateOut;
  systemData.flow_out.timestamp   = millis();

  // 5. Motor power sense (digital)
  int motorPow = digitalRead(MOTORSENSE_PIN);
  systemData.motor_power.value    = motorPow;
  systemData.motor_power.timestamp= millis();

  // 6. Pump power sense (digital)
  int pumpPow = digitalRead(PUMPSENSE_PIN);
  systemData.pump_power.value     = pumpPow;
  systemData.pump_power.timestamp = millis();

  // 7. Bentonite power sense (if no pin, set 0)
  systemData.bentonite_power.value     = 0;
  systemData.bentonite_power.timestamp = millis();

  // 8. E-stop sense
  int eStopVal = digitalRead(ESTOPSENSE_PIN);
  systemData.estop_button.value        = eStopVal;
  systemData.estop_button.timestamp    = millis();

  // 9. Gas sense 
  int gasValue = analogRead(GASSENSE_PIN); 
  systemData.gas_sensor.value = get_gasConcentration(gasValue); 
  systemData.gas_sensor.timestamp = millis(); 

  // 9. Update global_time
  systemData.global_time = millis();
}
