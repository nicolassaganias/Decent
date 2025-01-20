#include "cozir.h"
#include <SoftwareSerial.h>

EspSoftwareSerial::UART co2_sensor1;  // CO2 sensor 1 UART
EspSoftwareSerial::UART co2_sensor2;  // CO2 sensor 2 UART

COZIR czr1(&co2_sensor1);
COZIR czr2(&co2_sensor2);

uint32_t co2_data1;
uint32_t co2_data2;

// Define scaling factors
const float scaling_factor1 = 0.1;  // Example scaling factor for sensor 1
const float scaling_factor2 = 0.1;  // Example scaling factor for sensor 2

// Optional: Define baseline values if zero calibration is needed
const int baseline1 = 0;  // Adjust if there's a baseline offset for sensor 1
const int baseline2 = 0;  // Adjust if there's a baseline offset for sensor 2

void co2Init() {
  co2_sensor1.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, CO2_SENSOR1_TX_PIN, CO2_SENSOR1_RX_PIN);  // Start serial communications with sensor 1
  co2_sensor2.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, CO2_SENSOR2_TX_PIN, CO2_SENSOR2_RX_PIN);  // Start serial communications with sensor 2
  czr1.init();
  czr2.init();
  delay(1000);

  // set to polling explicitly.
  czr1.setOperatingMode(CZR_POLLING);
  czr2.setOperatingMode(CZR_POLLING);
  delay(1000);
}

void readCO2() {
  co2_data1 = czr1.CO2();
  delay(300);
  co2_data2 = czr2.CO2();

  // Apply scaling factors and zero calibration
  co2_data1 = (co2_data1 - baseline1) * scaling_factor1;
  co2_data2 = (co2_data2 - baseline2) * scaling_factor2;
}
