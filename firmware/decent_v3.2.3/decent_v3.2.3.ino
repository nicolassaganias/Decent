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
#include "SerialControl.h"

void checkCalibrationCommand();
unsigned long intervalCheckedTime = 0;
uint32_t time_s = 0;
uint32_t time_m = 0;
uint32_t checked_time = 0;
bool time_2s;
bool time_3s;
bool time_1m;
bool startCooldown = false;
unsigned long cooldownStartTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Sensors are initializing, please wait....");
  setPinsMode();
  inputString.reserve(20);  // Reserva memoria para evitar fragmentación
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
  unsigned long currentMillis = millis();

  // **Leer sensores cada 1s**
  if (currentMillis - lastSensorReadTime >= SENSOR_INTERVAL) {
    lastSensorReadTime = currentMillis;
    updateSensors();
  }

  // **Mostrar datos en Serial Monitor cada 2s**
  if (currentMillis - lastDisplayTime >= DISPLAY_INTERVAL) {
    lastDisplayTime = currentMillis;
    displaySensorData();
  }

  // **Alternar GP2 cada 10s**
  if (currentMillis - lastPumpToggleTime >= PUMP_TOGGLE_INTERVAL) {
    lastPumpToggleTime = currentMillis;
    toggleGP2();
  }
  Serial.println("");

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

  // check conditions while the mode is auto
  if (!manualMode) {  // automatic mode
    // liquid pump 1 (MP1) always on/off
    liquidPump1On = true;
    analogWrite(LIQUID_PUMP1, speedLP1);
    checked_time = time_s;
    pumpCharacteristic->setValue(String("p1:1").c_str());
    pumpCharacteristic->notify();

    if (pHSensor1Data > 0 &&
        (pHSensor1Data < LOWER_PH_THRESHOLD || pHSensor1Data > UPPER_PH_THRESHOLD) &&
        !liquidPump2On && !startCooldown) {

      if (abs(pHSensor1Data - pHSensor2Data) <= PH_DIFFERENCE_THRESHOLD) {
        liquidPump2On = true;
        lp2StartTime = currentMillis;  // 🔹 Guardar tiempo de activación
        analogWrite(LIQUID_PUMP2, speedLP2);
        pumpCharacteristic->setValue("p2:1");
        pumpCharacteristic->notify();
        Serial.println("✅ Liquid Pump 2 ON (pH dentro del umbral permitido)");
      } else {
        Serial.println("⚠️ Liquid Pump 2 NO encendida (Diferencia de pH demasiado alta)");
      }
    }

    // 🔹 Verificar si LP2 lleva encendido más de 10 segundos y apagarlo
    if (liquidPump2On && (currentMillis - lp2StartTime >= LP2_DURATION)) {
      liquidPump2On = false;
      analogWrite(LIQUID_PUMP2, 0);
      pumpCharacteristic->setValue("p2:0");
      pumpCharacteristic->notify();
      Serial.println("❌ Liquid Pump 2 OFF (Tiempo de operación cumplido)");

      // 🔹 Inicia el periodo de cooldown
      startCooldown = true;
      cooldownStartTime = currentMillis;
    }

    // 🔹 Cooldown de LP2 (10 minutos)
    if (startCooldown && (currentMillis - cooldownStartTime >= 30000)) {  // 10 min en ms = 600000
      startCooldown = false;
      Serial.println("🟢 Cooldown terminado. Liquid Pump 2 lista para ser reactivada.");
    }

    // liquid pump 3 (MP3) always on/OFF
    liquidPump3On = false;
    analogWrite(LIQUID_PUMP3, speedLP3);
    checked_time = time_s;
    pumpCharacteristic->setValue(String("p3:0").c_str());
    pumpCharacteristic->notify();

    // gasPump1 (MP4) depends on CH4 and Pressure levels
    if (CH4SensorData >= 0 && CH4SensorData < CH4_THRESHOLD && !gasPump1On) {
      gasPump1On = true;
      checked_time = time_s;
      analogWrite(GAS_PUMP1, speedGP1);
      pumpCharacteristic->setValue(String("p4:1").c_str());
      pumpCharacteristic->notify();
    } else if (CH4SensorData > 0 && CH4SensorData > CH4_THRESHOLD && !electrovalveOn) {
      electrovalveOn = true;
      digitalWrite(ELECTROVALVE, HIGH);
    }

    if (WPSensor2Data <= 0.1 && gasPump1On) {  // if pressure = 0
      gasPump1On = false;
      electrovalveOn = false;
      checked_time = time_s;
      analogWrite(GAS_PUMP1, 0);
      digitalWrite(ELECTROVALVE, LOW);
      pumpCharacteristic->setValue(String("p4:0").c_str());
      pumpCharacteristic->notify();
    }
  } else {  ////////////////////////// if Manual Mode ON /////////////////////////////
    // manually initialize pumps
    speedLP1 = 0;
    speedLP2 = 0;
    speedLP3 = 0;
    speedGP1 = 0;
    speedGP2 = 0;

    analogWrite(GAS_PUMP1, speedGP1);
    gasPump1On = (speedGP1 != 0);
    analogWrite(GAS_PUMP2, speedGP2);
    gasPump2On = (speedGP2 != 0);
    analogWrite(LIQUID_PUMP1, speedLP1);
    liquidPump1On = (speedLP1 != 0);
    analogWrite(LIQUID_PUMP2, speedLP2);
    liquidPump2On = (speedLP2 != 0);
    analogWrite(LIQUID_PUMP3, speedLP3);
    liquidPump3On = (speedLP3 != 0);

    digitalWrite(ELECTROVALVE, LOW);
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
    Serial.println("Enter commands (e.g., GP1,150 or LP2,200). Type 'exit' to save and continue.");

    while (1) {  // Bucle infinito hasta recibir "exit"
      if (Serial.available() > 0) {
        String calibrationCommand = Serial.readStringUntil('\n');
        calibrationCommand.trim();

        if (calibrationCommand.equalsIgnoreCase("exit")) {
          Serial.println("Calibration saved. Exiting...");
          break;  // Salir del bucle y continuar con setup()
        }
        else if (calibrationCommand.equalsIgnoreCase("enterph1") || calibrationCommand.equalsIgnoreCase("calph1") || calibrationCommand.equalsIgnoreCase("exitph1")) {
          calibratepHSensor1(calibrationCommand);
        }
        else if (calibrationCommand.equalsIgnoreCase("enterph2") || calibrationCommand.equalsIgnoreCase("calph2") || calibrationCommand.equalsIgnoreCase("exitph2")) {
          calibratepHSensor2(calibrationCommand);
        }
        else if (calibrationCommand.equalsIgnoreCase("zeroAdjust") || calibrationCommand.equalsIgnoreCase("zeroPoint") || calibrationCommand.equalsIgnoreCase("middlePoint") || calibrationCommand.equalsIgnoreCase("factoryReset")) {
          if (calibrationCommand.equalsIgnoreCase("zeroAdjust")) {
            CH4Calibration(CMD_ZERO_ADJUSTMENT);
          }
          else if (calibrationCommand.equalsIgnoreCase("zeroPoint")) {
            CH4Calibration(CMD_ZERO_POINT);
          }
          else if (calibrationCommand.equalsIgnoreCase("middlePoint")) {
            CH4Calibration(CMD_MIDDLE_POINT);
          }
          else if (calibrationCommand.equalsIgnoreCase("factoryReset")) {
            CH4Calibration(CMD_FACTORY_RESET);
          }
        }
        // Configuración de velocidades de bombas durante la calibración
        else if (calibrationCommand.startsWith("GP1,")) {
          int speed = calibrationCommand.substring(4).toInt();
          speedGP1 = constrain(speed, 0, 255);
          Serial.print("GAS_PUMP1 Speed set to: ");
          Serial.println(speedGP1);
        }
        else if (calibrationCommand.startsWith("GP2,")) {
          int speed = calibrationCommand.substring(4).toInt();
          speedGP2 = constrain(speed, 0, 255);
          Serial.print("GAS_PUMP2 Speed set to: ");
          Serial.println(speedGP2);
        }
        else if (calibrationCommand.startsWith("LP1,")) {
          int speed = calibrationCommand.substring(4).toInt();
          speedLP1 = constrain(speed, 0, 255);
          Serial.print("LIQUID_PUMP1 Speed set to: ");
          Serial.println(speedLP1);
        }
        else if (calibrationCommand.startsWith("LP2,")) {
          int speed = calibrationCommand.substring(4).toInt();
          speedLP2 = constrain(speed, 0, 255);
          Serial.print("LIQUID_PUMP2 Speed set to: ");
          Serial.println(speedLP2);
        }
        else if (calibrationCommand.startsWith("LP3,")) {
          int speed = calibrationCommand.substring(4).toInt();
          speedLP3 = constrain(speed, 0, 255);
          Serial.print("LIQUID_PUMP3 Speed set to: ");
          Serial.println(speedLP3);
        }
        else {
          Serial.println(" Unknown calibration command. Use GP1,150 or LP2,200.");
        }
      }
    }
  } else {
    Serial.println("No calibration command received. Proceeding to normal operation...");
  }
}

// **Lectura de sensores**
void updateSensors() {
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
}

// **Muestra datos en Serial Monitor**
void displaySensorData() {
  Serial.println("---------------------");
  Serial.print("CO1: " + String(CO2Sensor1Data) + "\t");
  Serial.print("CO2: " + String(CO2Sensor2Data) + "\t");
  Serial.print("CH4: " + String(CH4SensorData) + "\t");
  Serial.print("O2: " + String(O2SensorData) + "\t");
  Serial.print("H2: " + String(H2SensorData) + "\t");
  Serial.print("pH1: " + String(pHSensor1Data) + "\t");
  Serial.print("pH2: " + String(pHSensor2Data) + "\t");
  Serial.print("Temp: " + String(TempSensorData) + "\t");
  Serial.print("Current: " + String(measuredCurrent) + "\t");
  Serial.print("Voltage: " + String(measuredVoltage) + "\t");
  Serial.print("Pressure1: " + String(WPSensor1Data) + "\t");
  Serial.println("Pressure2: " + String(WPSensor2Data));
  Serial.println("---------------------");
}
void toggleGP2() {
  gasPump2On = !gasPump2On;  // Cambiar estado
  analogWrite(GAS_PUMP2, gasPump2On ? 140 : 0);  // ON a 140, OFF a 0

  Serial.print("🔄 Gas Pump 2 Alternando cada 10s: ");
  Serial.println(gasPump2On ? "ON" : "OFF");
}
