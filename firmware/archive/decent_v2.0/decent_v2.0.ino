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

void setup() {
  Serial.begin(115200);
  setPinsMode();
  O2Init();        // initialize the oxygen sensor
  INA219Init();
  CO2Sens1Init();  // initialize the CO2 sensor 1
  CO2Sens2Init();  // initialize the CO2 sensor 2
  phInit();        // initialize the pH sensor
  CH4Init();       // initialize the methane sensor
  delay(2000);
  BLEServerInit();  // BLE server initialize
  delay(1000);
  
  // CH4 sensor calibration
  // CH4Calibration(CMD_ZERO_ADJUSTMENT);
  // CH4Calibration(CMD_ZERO_POINT);
  // CH4Calibration(CMD_MIDDLE_POINT);
  // CH4Calibration(CMD_FACTORY_RESET);

  // pH sensor calibration
  // calibratepHSensor1("enterph1");
  // calibratepHSensor1("calph1");
  // calibratepHSensor1("exitph1");

  // calibratepHSensor2("enterph2");
  // calibratepHSensor2("calph2");
  // calibratepHSensor2("exitph2");
}
void loop() {
  if (millis() - sendIntervalTime > 2000) {
    // read the sensors data
    CO2Sensor1Data = readCO2Sens1();
    CO2Sensor2Data = readCO2Sens2();
    CH4SensorData = readCH4();
    O2SensorData = readO2();
    H2SensorData = readH2();
    pHSensor1Data = readpHSens1();
    pHSensor2Data = readpHSens2();
    TempSensorData = readTemp();
    WPSensor1Data = readWP1();
    WPSensor2Data = readWP2();
    readINA219(measuredCurrent, measuredVoltage);

    // check if the mobile app is connected or not
    if (deviceConnected) {
      // start to send the data to the mobile app
      Serial.println("--- Sending data to the app ---");
      switch (sentDataCount) {
        case 0:
          sendDataToApp("co1: " + String(CO2Sensor1Data));
          Serial.println("CO1: " + String(CO2Sensor1Data));
          sentDataCount++;
          break;
        case 1:
          sendDataToApp("co2: " + String(CO2Sensor2Data));
          Serial.println("CO2: " + String(CO2Sensor2Data));
          sentDataCount++;
          break;
        case 2:
          sendDataToApp("ch4: " + String(CH4SensorData));
          Serial.println("CH4: " + String(CH4SensorData));
          sentDataCount++;
          break;
        case 3:
          sendDataToApp("o2: " + String(O2SensorData));
          Serial.println("O2: " + String(O2SensorData));
          sentDataCount++;
          break;
        case 4:
          sendDataToApp("h2: " + String(H2SensorData));
          Serial.println("H2: " + String(H2SensorData));
          sentDataCount++;
          break;
        case 5:
          sendDataToApp("ph1: " + String(pHSensor1Data));
          Serial.println("pH1: " + String(pHSensor1Data));
          sentDataCount++;
          break;
        case 6:
          sendDataToApp("ph2: " + String(pHSensor2Data));
          Serial.println("pH2: " + String(pHSensor2Data));
          sentDataCount++;
          break;
        case 7:
          sendDataToApp("temp: " + String(TempSensorData));
          Serial.println("temp: " + String(TempSensorData));
          sentDataCount++;
          break;
        case 8:
          sendDataToApp("pressure1: " + String(WPSensor1Data));
          Serial.println("pressure1: " + String(WPSensor1Data));
          sentDataCount++;
          break;
        case 9:
          sendDataToApp("pressure2: " + String(WPSensor2Data));
          Serial.println("pressure2: " + String(WPSensor2Data));
          sentDataCount++;
          break;
        case 10:
          sendDataToApp("rrent: " + String(measuredCurrent));
        Serial.println("current: " + String(measuredCurrent));
          sentDataCount++;
          break;
        case 11:
          sendDataToApp("voltage: " + String(measuredVoltage));
          Serial.println("voltage: " + String(measuredVoltage));
          sentDataCount = 0;
          break;
        default:
          break;
      }
      Serial.println("--- End sending data to the App ---");
      Serial.println();
      Serial.flush();
    }


    // check conditions while the mode is auto
    if (!currentMode) {
     //Serial.println("--- Checking the pumps status in auto mode ---");
      if (pHSensor1Data == pHSensor2Data) {
       // Serial.println("PUMP 1 : 1");
        digitalWrite(RELAY_PIN, HIGH);
      } else {
      //  Serial.println("PUMP 1 : 0");
        digitalWrite(RELAY_PIN, LOW);
      }

      // liquid pump 1
      unsigned long currentMillis = millis();
      if ((pHSensor1Data < LOWER_PH_THRESHOLD || pHSensor1Data > UPPER_PH_THRESHOLD) && !liquidPump1On) {
        liquidPump1On = true;
        liquidPump1StartTime = currentMillis;
       // Serial.println("PUMP 1 : 1");
        digitalWrite(LIQUID_PUMP1, HIGH);
        pumpCharacteristic->setValue(String("p1:1").c_str());
        pumpCharacteristic->notify();
      }
      if (liquidPump1On && (currentMillis - liquidPump1StartTime >= LIQUID_PUMP_ON_DURATION)) {
        liquidPump1On = false;
       // Serial.println("PUMP 1 : 0");
        digitalWrite(LIQUID_PUMP1, LOW);
        pumpCharacteristic->setValue(String("p1:0").c_str());
        pumpCharacteristic->notify();
      }

      // liquid pump 2
      if (CH4SensorData > CH4_THRESHOLD && !liquidPump2On) {
        liquidPump2On = true;
        liquidPump2StartTime = currentMillis;
       // Serial.println("PUMP 2 : 1");
        digitalWrite(LIQUID_PUMP2, HIGH);
        pumpCharacteristic->setValue(String("p2:1").c_str());
        pumpCharacteristic->notify();
      }
      if (liquidPump2On && (currentMillis - liquidPump2StartTime >= LIQUID_PUMP_ON_DURATION)) {
        liquidPump2On = false;
       // Serial.println("PUMP 1 : 0");
        digitalWrite(LIQUID_PUMP2, LOW);
        pumpCharacteristic->setValue(String("p2:0").c_str());
        pumpCharacteristic->notify();
      }

      // gas pump 1
      if (CH4SensorData < CH4_THRESHOLD && !gasPump1On) {
        gasPump1On = true;
        gasPump1StartTime = currentMillis;
       // Serial.println("PUMP 4 : 1");
        digitalWrite(GAS_PUMP1, HIGH);
        pumpCharacteristic->setValue(String("p4:1").c_str());
        pumpCharacteristic->notify();
      }
      if (gasPump1On && (currentMillis - gasPump1StartTime >= GAS_PUMP_ON_DURATION)) {
        gasPump1On = false;
       // Serial.println("PUMP 4 : 0");
        digitalWrite(GAS_PUMP1, LOW);
        pumpCharacteristic->setValue(String("p4:0").c_str());
        pumpCharacteristic->notify();
      }
     // Serial.println("=== End the Pumps status in Auto Mode ===");
     // Serial.flush();
    }
    digitalWrite(LIQUID_PUMP3, !digitalRead(LIQUID_PUMP3));
    digitalWrite(GAS_PUMP2, !digitalRead(GAS_PUMP2));
    sendIntervalTime = millis();
  }

  // pH and methane sensor calibration using serial command (optional method)
  if (Serial.available() > 0) {
    String response = Serial.readStringUntil('\n');
    if (response == "calibration") {
      Serial.println(calibrationInfo);
      while (1) {
        if (Serial.available() > 0) {
          String calbrationResponse = Serial.readStringUntil('\n');
          if (calbrationResponse == "exit") {
            Serial.println("Exit successfull!");
            break;
          } else if (calbrationResponse == "enterph1" || calbrationResponse == "calph1" || calbrationResponse == "exitph1") {
            calibratepHSensor1(calbrationResponse);
          } else if (calbrationResponse == "enterph2" || calbrationResponse == "calph2" || calbrationResponse == "exitph2") {
            calibratepHSensor2(calbrationResponse);
          } else if (calbrationResponse == "zeroAdjust" || calbrationResponse == "zeroPoint" || calbrationResponse == "middlePoint" || calbrationResponse == "factoryReset") {
            if (calbrationResponse == "zeroAdjust") {
              CH4Calibration(CMD_ZERO_ADJUSTMENT);
            } else if (calbrationResponse == "zeroPoint") {
              CH4Calibration(CMD_ZERO_POINT);
            } else if (calbrationResponse == "middlePoint") {
              CH4Calibration(CMD_MIDDLE_POINT);
            } else if (calbrationResponse == "factoryReset") {
              CH4Calibration(CMD_FACTORY_RESET);
            }
          } else {
            Serial.println("Unknown command");
          }
          delay(100);
        }
      }
    }
  }
}