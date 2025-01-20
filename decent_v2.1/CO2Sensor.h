#include "cozir.h"
#include <SoftwareSerial.h>

// probando con 100% el CO2 1(20% se satura en 2000) y el CO2 (100%) satura en 1000
#define CO2_SENSOR1_SCALING_FACTOR 0.1
#define CO2_SENSOR2_SCALING_FACTOR 0.1

#define CO2_SENSOR1_BASELINE 0
#define CO2_SENSOR2_BASELINE 0

EspSoftwareSerial::UART CO2Sensor1;  // CO2 sensor 1 UART
EspSoftwareSerial::UART CO2Sensor2;  // CO2 sensor 2 UART

COZIR czr1(&CO2Sensor1);
COZIR czr2(&CO2Sensor2);

void CO2Sens1Init() {
  CO2Sensor1.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, CO2_SENSOR1_TX_PIN, CO2_SENSOR1_RX_PIN);
  czr1.init();
  delay(1000);
  czr1.setOperatingMode(CZR_POLLING);  // set to polling explicitly.
  delay(1000);
}

void CO2Sens2Init() {
  CO2Sensor2.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, CO2_SENSOR2_TX_PIN, CO2_SENSOR2_RX_PIN);  // Start serial communications with sensor 2
  czr2.init();
  delay(1000);
  czr2.setOperatingMode(CZR_POLLING);  // set to polling explicitly.
  delay(1000);
}

int readCO2Sens1() {
  int CO2Data = czr1.CO2();
  delay(300);
  CO2Data = (CO2Data - CO2_SENSOR1_BASELINE) * CO2_SENSOR1_SCALING_FACTOR;  // Apply scaling factors and zero calibration
  return CO2Data;
}

int readCO2Sens2() {
  int CO2Data = czr2.CO2();
  delay(300);
  CO2Data = (CO2Data - CO2_SENSOR2_BASELINE) * CO2_SENSOR2_SCALING_FACTOR;  // Apply scaling factors and zero calibration
  return CO2Data;
}
