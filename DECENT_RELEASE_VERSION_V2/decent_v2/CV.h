// current and voltage sensor calibration offset variable
double currentCalibrationOffset = 0.0;
double voltageCalibrationOffset = 0.0;
double voltageScalingFactor = 11.0;

// current sensor
int mVperAmp = 185;  // 5A version of the ACS712 (use 100 for 20A Module, 66 for 30A Module)
double voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

float getVPP() {
  float result;
  int readValue;
  int maxValue = 0;
  int minValue = 4096;

  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) {
    readValue = analogRead(CURRENT_SENSOR_PIN);
    if (readValue > maxValue) {
      maxValue = readValue;
    }
    if (readValue < minValue) {
      minValue = readValue;
    }
  }
  result = ((maxValue - minValue) * 3.3) / 4096.0;
  return result;
}

double readCurrent() {
  voltage = getVPP();
  VRMS = (voltage / 2.0) * 0.707;  // root 2 is 0.707
  AmpsRMS = ((VRMS * 1000) / mVperAmp) + currentCalibrationOffset;
 // Serial.print("Amps: ");
 // Serial.println(AmpsRMS);
  return AmpsRMS;
}

void calibrateCurrentSensor() {
  Serial.println("Starting current sensor calibration...");
  double noLoadCurrent = 0.0;

  for (int i = 0; i < 10; i++) {
    noLoadCurrent += readCurrent();
    delay(500);
  }
  noLoadCurrent /= 10.0;
  if (noLoadCurrent > 0) {
    currentCalibrationOffset -= noLoadCurrent;
  } else if (noLoadCurrent < 0) {
    currentCalibrationOffset -= noLoadCurrent;
  }

  Serial.print("Current sensor calibration complete. Offset: ");
  Serial.println(currentCalibrationOffset);
}

double readVoltage() {
  int rawValue = analogRead(VOLTAGE_SENSOR_PIN);
  double rawVoltage = (rawValue * 3.3) / 4096.0;
  double sensorVoltage = rawVoltage * voltageScalingFactor;
  sensorVoltage += voltageCalibrationOffset;

 // Serial.print("Voltage: ");
 // Serial.println(sensorVoltage);
  return sensorVoltage;
}

void calibrateVoltageSensor() {
  Serial.println("Starting voltage sensor calibration...");
  double actualVoltage = 0.0;
  double measuredVoltage = 0.0;

  for (int i = 0; i < 10; i++) {
    measuredVoltage += readVoltage();
    delay(500);
  }

  measuredVoltage /= 10.0;
  voltageCalibrationOffset = actualVoltage - measuredVoltage;
  Serial.print("Voltage sensor calibration complete. Offset: ");
  Serial.println(voltageCalibrationOffset);
}
