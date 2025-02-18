/*
@Project Name      : DeCent
@Version           : v1.1
@Target Controller : ESP32
@Modifed Date      : June 28, 2024

#Current Sensor Conditions
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

unsigned long timepoint = 0;

void setup() {
  Serial.begin(115200);
  pinConfiguration();
  co2Init();
  oxygenSensorInit();
  methaneSensorInit();
  configPH();
  configBLE();
}

void loop() {
  if (calibration_mode == false) {
    if (millis() - timepoint > 3000) {
      readSensorData();
      printSensorData();
      timepoint = millis();
    }
  }
  // Calibration process
  while (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    if (data == "calibration") {
      Serial.println("Enter calibration command...");
      calibration_mode = true;
    } else {
      if (calibration_mode == true) {
        if (data == "enterph1") {
          ph1.calibration(voltage1, temperature, "ENTERPH");
        } else if (data == "calph1") {
          ph1.calibration(voltage1, temperature, "CALPH");
        } else if (data == "exitph1") {
          ph1.calibration(voltage1, temperature, "EXITPH");
          calibration_mode = false;
        } else if (data == "enterph2") {
          ph1.calibration(voltage2, temperature, "ENTERPH");
        } else if (data == "calph2") {
          ph1.calibration(voltage2, temperature, "CALPH");
        } else if (data == "exitph2") {
          ph1.calibration(voltage2, temperature, "EXITPH");
          calibration_mode = false;
        }
      }
    }
  }
}

void readSensorData() {
  // read other sensors
  readCO2();
  readMethane();   // methane
  readHydrogen();  // hydrogen
  readOxygen();    // oxygen
  temperature = readTemperature();

  // Read pH sensor data
  voltage1 = analogRead(PH_SENSOR2_PIN) / ESPADC * ESPVOLTAGE;
  voltage2 = analogRead(PH_SENSOR1_PIN) / ESPADC * ESPVOLTAGE;
  phValue1 = ph1.readPH(voltage1, temperature);
  phValue2 = ph2.readPH(voltage2, temperature);

  // match conditions
  // check alarm status
  if (phValue1 == phValue2) {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("PH ALARM");
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }

  // check electrovalve status
  if (valve1 == false) {
    if (phValue1 < 6.5 || phValue1 > 9.0) {
      valve1 = true;
      digitalWrite(ELECTROVALVE1, HIGH);
      Serial.println("EV1 ON");
    }
  } else {
    if (millis() - last_valve1_update > ph_valve_on_time) {
      valve1 = false;
      digitalWrite(ELECTROVALVE1, LOW);
      Serial.println("EV1 OFF");
      last_valve1_update = millis();
    }
  }

  // check CH4 status
  if (pump1 == false) {
    int ch4_data = readMethane();
    if (ch4_data > 95) {
      pump1 = true;
      digitalWrite(LIQUID_PUMP1, HIGH);
      Serial.println("LIQUID_PUMP1 ON");
    }
  } else {
    if (millis() - last_pump1_update > pump1_on_time) {
      pump1 = false;
      digitalWrite(LIQUID_PUMP1, LOW);
      Serial.println("LIQUID_PUMP1 OFF");
      last_pump1_update = millis();
    }
  }

  // check ch4 another status
  if (pump2 == false) {
    int ch4_data = readMethane();
    if (ch4_data < 95) {
      pump2 = true;
      digitalWrite(LIQUID_PUMP2, HIGH);
      Serial.println("LIQUID_PUMP2 ON");
    }
  } else {
    if (millis() - last_pump2_update > pump2_on_time) {
      pump2 = false;
      digitalWrite(LIQUID_PUMP2, LOW);
      Serial.println("LIQUID_PUMP2 OFF");
      last_pump2_update = millis();
    }
  }
}

void printSensorData() {
  Serial.print("pH 1: ");
  Serial.print(phValue1);
  Serial.print(" | pH 2: ");
  Serial.print(phValue2);
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
  Serial.print(temperature);
  Serial.print(" C");
  Serial.print(" | Methane: ");
  Serial.print(readMethane());
  Serial.print(" %");
  Serial.print(" | Water Pressure 1: ");
  Serial.print(readWaterPressure(WATER_PRESSURE_SENSOR1_PIN));
  Serial.print(" | Water Pressure 2: ");
  Serial.print(readWaterPressure(WATER_PRESSURE_SENSOR2_PIN));
  Serial.println();
}