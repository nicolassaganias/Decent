#include "DFRobot_ESP_PH.h"
#include <EEPROM.h>

DFRobot_ESP_PH ph1;
DFRobot_ESP_PH ph2;

#define ESPADC 4096.0    // the esp Analog Digital Conversion value
#define ESPVOLTAGE 3300  // the esp voltage supply value
float voltage1, voltage2, phValue1, phValue2;

void configPH() {
  EEPROM.begin(64);  // increase EEPROM size if necessary to store more calibration data
  ph1.begin();
  ph2.begin();
}