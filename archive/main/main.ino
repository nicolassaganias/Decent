/*
@Project Name      : DeCent
@Version           : v1.0
@Target Controller : ESP32
@Modifed Date      : May 06, 2024

#Current Sensor Conditons
    1. if PHSensor1 == PHSensor2 then an alarm goes on
    2. if PHSensor1 < 6.5 OR > 9 turns on an electrovalve during "x" seconds
    3. if CH4 > 95% turns on an electrovalve during "x" seconds
    4. if CH4 < 95% turns on a pump during "x" seconds

*/

#include "Settings.h"
#include "CO2Sensor.h"
#include "O2Sensor.h"
#include "H2Sensor.h"
#include "Temperature.h"
#include "WaterPressure.h"
#include "pHSensor.h"
#include "CH4Sensor.h"
#include "ESP32BLEServer.h"

void setup() {
  pinConfiguration();
  co2Init();
  oxygenSensorInit();
  methaneSensorInit();
  configBLE();
}

void loop() {
  /* ----------- sensor conditions start ------------*/
  readpH1();
  readCO2();
  readMethane();
  readHydrogen();
  readOxygen();
  temperature_data = readTemperature();
  readpH2();

  // check alarm status
  if (ph1_data == ph2_data) {
    digitalWrite(RELAY_PIN, HIGH);
    //Serial.println("Relay ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    //Serial.println("Relay OFF");
  }

  // check electrovalve status
  if (valve1 == false) {
    if (ph1_data < 6.5 || ph1_data > 9.0) {
      valve1 = true;
      digitalWrite(ELECTROVALVE1, HIGH);
      // Serial.println("Electrovalve 1 ON");
    }
  } else {
    if (millis() - last_valve1_update > ph_valve_on_time) {
      valve1 = false;
      digitalWrite(ELECTROVALVE1, LOW);
      // Serial.println("Electrovalve 1 OFF");
      last_valve1_update = millis();
    }
  }

  // check CH4 status
  if (pump1 == false) {
    int ch4_data = readMethane();
    if (ch4_data > 95) {
      pump1 = true;
      digitalWrite(LIQUID_PUMP1, HIGH);
      // Serial.println("Liquid Pump 1 ON");
    }
  } else {
    if (millis() - last_pump1_update > pump1_on_time) {
      pump1 = false;
      digitalWrite(LIQUID_PUMP1, LOW);
      // Serial.println("Liquid Pump 1 OFF");
      last_pump1_update = millis();
    }
  }

  // check ch4 another status
  if (pump2 == false) {
    int ch4_data = readMethane();
    if (ch4_data < 95) {
      pump2 = true;
      digitalWrite(LIQUID_PUMP2, HIGH);
      // Serial.println("Liquid Pump 2 ON");
    }
  } else {
    if (millis() - last_pump2_update > pump2_on_time) {
      pump2 = false;
      digitalWrite(LIQUID_PUMP2, LOW);
      // Serial.println("Liquid Pump 2 OFF");
      last_pump2_update = millis();
    }
  }

  /* ----------- sensor conditions end ------------*/
  /* ----------- print sensors data start ----------*/
  Serial.print("pH 1: ");
  Serial.print(ph1_data);
  Serial.print(" | pH 2: ");
  Serial.print(ph2_data);
  Serial.print(" | CO2 1: ");
  Serial.print(co2_data1);
  Serial.print(" PPM");
  Serial.print(" | CO2 2: ");
  Serial.print(co2_data2);
  Serial.print(" PPM");
  Serial.print(" | H2: ");
  Serial.print(hydrogen_data);
   Serial.print(" %");
  Serial.print(" | O2: ");
  Serial.print(oxygen_data);
  Serial.print(" %");
  Serial.print(" | Temperature: ");
  Serial.print(temperature_data);
  Serial.print(" C");
  Serial.print(" | Methane: ");
  Serial.print(readMethane());
  Serial.print(" %");
  Serial.print(" | Water Pressure 1: ");
  Serial.print(readWaterPressure(WATER_PRESSURE_SENSOR1_PIN));
  Serial.print(" | Water Pressure 2: ");
  Serial.print(readWaterPressure(WATER_PRESSURE_SENSOR2_PIN));
  Serial.println();
  delay(1000);
  /* ----------- print sensors data end ----------*/
}