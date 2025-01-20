#include "DFRobot_OxygenSensor.h"

/*
 * I2C slave Address, The default is ADDRESS_3.
 * ADDRESS_0   0x70  i2c device address.
 * ADDRESS_1   0x71
 * ADDRESS_2   0x72
 * ADDRESS_3   0x73
 */

#define Oxygen_IICAddress ADDRESS_3  // set address
#define COLLECT_NUMBER 10            // collect number, the collection range is 1-100.
DFRobot_OxygenSensor oxygen;         // create an object

float oxygen_data;

void oxygenSensorInit() {
  if (!oxygen.begin(Oxygen_IICAddress)) {
    Serial.println("Oxygen Sensor I2C device number error !");
    delay(1000);
  } else {
    Serial.println("Oxygen sensor I2C connect success !");
  }
}

void readOxygen() {
  oxygen_data = oxygen.getOxygenData(COLLECT_NUMBER);  // read the sensor value
    // Serial.print(" oxygen concentration is ");
    // Serial.print(oxygenData);
    // Serial.println(" %vol");
}
