#include "cozir.h"
#include <SoftwareSerial.h>

#define CO2_SENSOR1_RX_PIN 4
#define CO2_SENSOR1_TX_PIN 23

#define CO2_SENSOR2_RX_PIN 18
#define CO2_SENSOR2_TX_PIN 19

EspSoftwareSerial::UART co2_sensor1;  // CO2 sensor 1 UART
EspSoftwareSerial::UART co2_sensor2;  // CO2 sensor 2 UART

COZIR czr1(&co2_sensor1);
COZIR czr2(&co2_sensor2);

void setup() {
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

void loop() {
  uint32_t c1 = czr1.CO2();
  Serial.print("CO2 =\t");
  Serial.println(c1);
  delay(1000);

  uint32_t c2 = czr2.CO2();
  Serial.print("CO2 =\t");
  Serial.println(c2);
  delay(1000);
}