#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

void INA219Init() {
  for (int i = 0; i <= 5; i++) {
    if(ina219.begin()){
      break;
    }else {
      Serial.println("Failed to find INA219 chip");
    }
    delay(500);
  }
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();
}

float readINA219(float &current, float &voltage) {
  float totalCurrent = 0;
  float totalVoltage = 0;
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;

  // Take 10 samples
  for (int i = 0; i < 10; i++) {
    shuntvoltage = ina219.getShuntVoltage_mV();
    busvoltage = ina219.getBusVoltage_V();
    current_mA = ina219.getCurrent_mA();

    totalVoltage += busvoltage + (shuntvoltage * 0.001);
    totalCurrent += current_mA * 0.001;
    delay(100); // Wait for 100ms between samples
  }

  // Calculate averages
  voltage = totalVoltage / 10;
  current = totalCurrent / 10;
}
