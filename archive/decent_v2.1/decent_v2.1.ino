#include "Settings.h"
#include "CO2Sensor.h"
#include "O2Sensor.h"
#include "H2Sensor.h"
#include "Temperature.h"
#include "WaterPressure.h"
#include "pHSensor.h"
#include "CH4Sensor.h"
#include "ESP32BLEServer.h"
#include "CV.h"

void checkCalibrationCommand();
unsigned long intervalCheckedTime = 0;
uint32_t time_s = 0;
uint32_t checked_time = 0;
bool time_2s;
bool time_3s;

void setup() {
  Serial.begin(115200);
  Serial.println("Sensors are initializing, please wait....");
  setPinsMode();
  O2Init();        // initialize the oxygen sensor
  CO2Sens1Init();  // initialize the CO2 sensor 1
  CO2Sens2Init();  // initialize the CO2 sensor 2
  phInit();        // initialize the pH sensor
  CH4Init();       // initialize the methane sensor
  INA219Init();
  delay(1000);
  BLEServerInit();  // BLE server initialize
  delay(1000);
  Serial.println("Initializing done!");
  checkCalibrationCommand();
}

void loop() {
  if (millis() - intervalCheckedTime >= 1000) {
    time_s++;
    intervalCheckedTime = millis();
    if (time_s % 2 == 0) {
      time_2s = true;
    }
    if (time_3s % 3 == 0) {
      time_3s = true;
    }
    if (time_s >= 60) {
      time_s = 0;
    }
  }

  if (time_2s) {
    time_2s = false;
    CO2Sensor1Data = readCO2Sens1();
    CO2Sensor2Data = readCO2Sens2();
    readCH4();
    O2SensorData = readO2();
    H2SensorData = readH2();
    pHSensor1Data = readpHSens1();
    pHSensor2Data = readpHSens2();
    TempSensorData = readTemp();
    WPSensor1Data = readWP1();
    WPSensor2Data = readWP2();
    readINA219(measuredCurrent, measuredVoltage);

    Serial.println("---------------------");
    Serial.print("CO1: " + String(CO2Sensor1Data) + "\t");
    Serial.print("CO2: " + String(CO2Sensor2Data) + "\t");
    Serial.print("CH4: " + String(CH4SensorData) + "\t");
    Serial.print("O2: " + String(O2SensorData) + "\t");
    Serial.print("H2: " + String(H2SensorData) + "\t");
    Serial.print("pH1: " + String(pHSensor1Data) + "\t");
    Serial.print("pH2: " + String(pHSensor2Data) + "\t");
    Serial.print("Temperature: " + String(TempSensorData) + "\t");
    Serial.print("Current: " + String(measuredCurrent) + "\t");
    Serial.print("Voltage: " + String(measuredVoltage) + "\t");
    Serial.print("Pressure1: " + String(WPSensor1Data) + "\t");
    Serial.println("Pressure2: " + String(WPSensor2Data));  // End the line
    Serial.println("---------------------");

    // check if the mobile app is connected or not
    if (deviceConnected) {
      switch (sentDataCount) {
        case 0:
          sendDataToApp("co1: " + String(CO2Sensor1Data));
          sentDataCount++;
          break;
        case 1:
          sendDataToApp("co2: " + String(CO2Sensor2Data));
          sentDataCount++;
          break;
        case 2:
          sendDataToApp("ch4: " + String(CH4SensorData));
          sentDataCount++;
          break;
        case 3:
          sendDataToApp("o2: " + String(O2SensorData));
          sentDataCount++;
          break;
        case 4:
          sendDataToApp("h2: " + String(H2SensorData));
          sentDataCount++;
          break;
        case 5:
          sendDataToApp("ph1: " + String(pHSensor1Data));
          sentDataCount++;
          break;
        case 6:
          sendDataToApp("ph2: " + String(pHSensor2Data));
          sentDataCount++;
          break;
        case 7:
          sendDataToApp("temp: " + String(TempSensorData));
          sentDataCount++;
          break;
        case 8:
          sendDataToApp("pressure1: " + String(WPSensor1Data));
          sentDataCount++;
          break;
        case 9:
          sendDataToApp("pressure2: " + String(WPSensor2Data));
          sentDataCount++;
          break;
        case 10:
          sendDataToApp("current: " + String(measuredCurrent));
          sentDataCount++;
          break;
        case 11:
          sendDataToApp("voltage: " + String(measuredVoltage));
          sentDataCount = 0;
          break;
        default:
          break;
      }
    }

    // check alarm
    if (pHSensor1Data == pHSensor2Data) {
      Serial.println("Alarm ON");
      digitalWrite(RELAY_PIN, HIGH);
    } else {
      Serial.println("Alarm OFF");
      digitalWrite(RELAY_PIN, LOW);
    }
  }

  // check conditions while the mode is auto
  if (!manualMode) {
    // liquid pump 1
    if (pHSensor1Data > 0 && (pHSensor1Data < LOWER_PH_THRESHOLD || pHSensor1Data > UPPER_PH_THRESHOLD) && !liquidPump1On) {
      liquidPump1On = true;
      Serial.println("PUMP 1 : 1");
      digitalWrite(LIQUID_PUMP1, HIGH);
      checked_time = time_s;
      pumpCharacteristic->setValue(String("p1:1").c_str());
      pumpCharacteristic->notify();
    }

    if (liquidPump1On) {
      if (time_s - checked_time >= LIQUID_PUMP_ON_DURATION) {
        liquidPump1On = false;
        checked_time = 0;
        Serial.println("PUMP 1 : 0");
        digitalWrite(LIQUID_PUMP1, LOW);
        pumpCharacteristic->setValue(String("p1:0").c_str());
        pumpCharacteristic->notify();
      }
    }

    // liquid pump 2
    if (CH4SensorData > CH4_THRESHOLD && !liquidPump2On) {
      liquidPump2On = true;
      checked_time = time_s;
      Serial.println("PUMP 2 : 1");
      digitalWrite(LIQUID_PUMP2, HIGH);
      pumpCharacteristic->setValue(String("p2:1").c_str());
      pumpCharacteristic->notify();
    }

    if (liquidPump2On) {
      if (time_s - checked_time >= LIQUID_PUMP_ON_DURATION) {
        liquidPump2On = false;
        checked_time = 0;
        Serial.println("PUMP 1 : 0");
        digitalWrite(LIQUID_PUMP2, LOW);
        pumpCharacteristic->setValue(String("p2:0").c_str());
        pumpCharacteristic->notify();
      }
    }

    // gas pump 1
    if (CH4SensorData > 0 && CH4SensorData < CH4_THRESHOLD && !gasPump1On) {
      gasPump1On = true;
      checked_time = time_s;
      Serial.println("PUMP 4 : 1");
      digitalWrite(GAS_PUMP1, HIGH);
      pumpCharacteristic->setValue(String("p4:1").c_str());
      pumpCharacteristic->notify();
    }

    if (gasPump1On) {
      if (time_s - checked_time >= GAS_PUMP_ON_DURATION) {
        gasPump1On = false;
        checked_time = 0;
        Serial.println("PUMP 4 : 0");
        digitalWrite(GAS_PUMP1, LOW);
        pumpCharacteristic->setValue(String("p4:0").c_str());
        pumpCharacteristic->notify();
      }
    }

    if (time_3s) {
      time_3s = false;
      digitalWrite(LIQUID_PUMP3, !digitalRead(LIQUID_PUMP3));
      digitalWrite(GAS_PUMP2, !digitalRead(GAS_PUMP2));
    }
  }
}

void checkCalibrationCommand() {
  Serial.println("Type 'calibration' within 10 seconds to enter calibration mode...");
  unsigned long startTime = millis();
  bool calibrationMode = false;

  while (millis() - startTime < 10000) {
    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');
      command.trim();
      if (command.equalsIgnoreCase("calibration")) {
        calibrationMode = true;
        break;
      }
    }
  }

  if (calibrationMode) {
    Serial.println(calibrationInfo);
    while (1) {  // Infinite loop for calibration mode
      if (Serial.available() > 0) {
        String calibrationCommand = Serial.readStringUntil('\n');
        calibrationCommand.trim();
        if (calibrationCommand.equalsIgnoreCase("exit")) {
          Serial.println(calibrationInfo);
          break;
        } else if (calibrationCommand.equalsIgnoreCase("enterph1") || calibrationCommand.equalsIgnoreCase("calph1") || calibrationCommand.equalsIgnoreCase("exitph1")) {
          calibratepHSensor1(calibrationCommand);
        } else if (calibrationCommand.equalsIgnoreCase("enterph2") || calibrationCommand.equalsIgnoreCase("calph2") || calibrationCommand.equalsIgnoreCase("exitph2")) {
          calibratepHSensor2(calibrationCommand);
        } else if (calibrationCommand.equalsIgnoreCase("zeroAdjust") || calibrationCommand.equalsIgnoreCase("zeroPoint") || calibrationCommand.equalsIgnoreCase("middlePoint") || calibrationCommand.equalsIgnoreCase("factoryReset")) {
          if (calibrationCommand.equalsIgnoreCase("zeroAdjust")) {
            CH4Calibration(CMD_ZERO_ADJUSTMENT);
          } else if (calibrationCommand.equalsIgnoreCase("zeroPoint")) {
            CH4Calibration(CMD_ZERO_POINT);
          } else if (calibrationCommand.equalsIgnoreCase("middlePoint")) {
            CH4Calibration(CMD_MIDDLE_POINT);
          } else if (calibrationCommand.equalsIgnoreCase("factoryReset")) {
            CH4Calibration(CMD_FACTORY_RESET);
          }
        } else {
          Serial.println("Unknown calibration command.");
        }
      }
    }
  } else {
    Serial.println("No calibration command received. Proceeding to normal operation...");
  }
}
