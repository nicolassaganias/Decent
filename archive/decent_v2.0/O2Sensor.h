#include "DFRobot_OxygenSensor.h"
/*
   I2C slave Address, The default is ADDRESS 3
   ADDRESS_0   0x70  i2c device address
   ADDRESS_1   0x71
   ADDRESS_2   0x72
   ADDRESS_3   0x73
*/

#define Oxygen_IICAddress ADDRESS_3  // set address
#define COLLECT_NUMBER 10            // collect number, the collection range is 1-100

DFRobot_OxygenSensor oxygen;  // create an object

// Calibration offset for oxygen data
float o2CalibrationOffset = 0.0;  // Adjust this value to calibrate readings

void O2Init() {
  if (!oxygen.begin(Oxygen_IICAddress)) {
    Serial.println("Oxygen Sensor I2C device number error !");
    delay(1000);
  } else {
    Serial.println("Oxygen sensor I2C connect success !");
  }
}

float readO2() {
  float oxygenData = oxygen.getOxygenData(COLLECT_NUMBER);

  // Apply calibration offset
  oxygenData += o2CalibrationOffset;
  // Clamp the oxygen data to reasonable bounds (e.g., 0 to 100%)
  // if (oxygenData < 0.0) oxygenData = 0.0;
  // if (oxygenData > 100.0) oxygenData = 100.0;

  return oxygenData;
}
