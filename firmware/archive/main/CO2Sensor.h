#include "cozir.h"
#include <SoftwareSerial.h>

EspSoftwareSerial::UART co2_sensor1;  // CO2 sensor 1 UART
EspSoftwareSerial::UART co2_sensor2;  // CO2 sensor 2 UART

COZIR czr1(&co2_sensor1);
COZIR czr2(&co2_sensor2);

uint32_t co2_data1;
uint32_t co2_data2;

void co2Init() {
  co2_sensor1.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, CO2_SENSOR1_TX_PIN, CO2_SENSOR1_RX_PIN);  // Start serial communications with sensor 1
  co2_sensor2.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, CO2_SENSOR2_TX_PIN, CO2_SENSOR2_RX_PIN);  // Start serial communications with sensor 2
  czr1.init();
  czr2.init();
  delay(1000);

  Serial.begin(115200);
  Serial.print("COZIR_LIB_VERSION: ");
  Serial.println(COZIR_LIB_VERSION);
  Serial.println();

  // set to polling explicitly.
  czr1.setOperatingMode(CZR_POLLING);
  czr2.setOperatingMode(CZR_POLLING);
  delay(1000);
}

void readCO2() {
  co2_data1 = czr1.CO2();
  delay(1000);
  co2_data2 = czr2.CO2();
}