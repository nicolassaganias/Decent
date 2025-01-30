/*
 * Calibration Process for pH Sensors
 *
 * The calibration process involves the following steps:
 *
 * 1. **Entering Calibration Mode:**
 *    - The user sends a command to enter calibration mode (e.g., "enterph1" or "enterph2").
 *    - This initializes the calibration procedure for the respective pH sensor.
 * 
 * 2. **Reading the pH Sensor Data:**
 *    - The pH voltage is read from the sensor using `analogRead()`.
 *    - This voltage is then converted to a corresponding pH value using the pH sensor's library functions.
 * 
 * 3. **Calibration Command:**
 *    - The user sends a command (e.g., "calph1" or "calph2") to calibrate the sensor.
 *    - The current temperature is checked; if valid, it is used for calibration. If not, a default temperature (25°C) is applied.
 *    - The sensor calibration function is called with the current pH voltage and temperature.
 * 
 * 4. **Exiting Calibration Mode:**
 *    - The user sends a command to exit calibration mode (e.g., "exitph1" or "exitph2").
 *    - The last recorded pH voltage and temperature are used to finalize the calibration.
 *    - A confirmation message is printed to indicate that the calibration has been completed.
 *
 * The calibration commands are handled in the `handlepHCommand()` function, which checks for user input and directs
 * it to the appropriate calibration functions for the specific pH sensor.
 */

#include "DFRobot_ESP_PH.h"
#include <EEPROM.h>
#include <Adafruit_ADS1X15.h>

DFRobot_ESP_PH ph1;  // pH sensor 1
DFRobot_ESP_PH ph2;  // pH sensor 2
Adafruit_ADS1115 ads;

#define ESP_ADC_RANGE 4096.0       // the esp Analog Digital Conversion value
#define ESP_REF_VOLTAGE 3300       // the esp voltage supply value
#define PH_DEFAULT_TEMPERATURE 25  // default temperature

#define ENTER "ENTERPH"  // enter pH calibration mode
#define CAL "CALPH"      // send command to calibrate pH mode
#define EXIT "EXITPH"    //exit pH calibration mode

float pH1Temperature, pH2Temperature;  // temperature values for the both pH sensor
float returnpHVoltage;                 // return ph calculated voltage

void phInit() {
  EEPROM.begin(64);  // begin eeprom
  ph1.begin();       // initialize the pH sensor 1
  ph2.begin();       // initialize the pH sensor 2
  delay(500);
  for (int i = 0; i <= 5; i++) {
    if (ads.begin()) {
      Serial.println("ADS initializtion done!");
      break;
    } else {
      Serial.println("Failed to initialize ADS.");
    }
    delay(500);
  }
}

float readph1voltage() {
  int16_t adc0 = ads.readADC_SingleEnded(0);
  float volts0 = ads.computeVolts(adc0);
  return volts0;
}

float readph2voltage() {
  int16_t adc1 = ads.readADC_SingleEnded(1);
  float volts1 = ads.computeVolts(adc1);
  return volts1;
}

// read pH 1 sensor data
float readpHSens1() {
  float pHVoltage, pHValue;
  pHVoltage = readph1voltage();
  pHValue = ph1.readPH(pHVoltage, PH_DEFAULT_TEMPERATURE);
  ph1.calibration(pHVoltage, PH_DEFAULT_TEMPERATURE);
  return pHValue;
}

// read pH 2 sensor data
float readpHSens2() {
  float pHVoltage, pHValue;
  pHVoltage = readph2voltage();
  pHValue = ph2.readPH(pHVoltage, PH_DEFAULT_TEMPERATURE);
  ph2.calibration(pHVoltage, PH_DEFAULT_TEMPERATURE);
  return pHValue;
}

void calibratepHSensor1(String calibrationData) {
  if (calibrationData == "enterph1") {
    ph1.calibration(3.2, 25.0, ENTER);
  } else if (calibrationData == "calph1") {
    // change the temperature value, if the current temperature is available
    if (temperature > 0) {
      pH1Temperature = temperature;
    } else {
      pH1Temperature = PH_DEFAULT_TEMPERATURE;
    }
    float pHVoltage;
    pHVoltage = readph1voltage();
    ph1.calibration(pHVoltage, pH1Temperature, CAL);
    returnpHVoltage = pHVoltage;
  } else if (calibrationData == "exitph1") {
    ph1.calibration(returnpHVoltage, pH1Temperature, EXIT);
    Serial.println("Calibration completed!");
  }
}

void calibratepHSensor2(String calibrationData) {
  if (calibrationData == "enterph2") {
    ph2.calibration(3.2, 25.0, ENTER);
  } else if (calibrationData == "calph2") {
    // change the temperature value, if the current temperature is available
    if (temperature > 0) {
      pH2Temperature = temperature;
    } else {
      pH2Temperature = PH_DEFAULT_TEMPERATURE;
    }
    float pHVoltage;
    pHVoltage = readph2voltage();
    ph2.calibration(pHVoltage, pH2Temperature, CAL);
    returnpHVoltage = pHVoltage;
  } else if (calibrationData == "exitph2") {
    ph2.calibration(returnpHVoltage, pH2Temperature, EXIT);
    Serial.println("Calibration completed!");
  }
}
