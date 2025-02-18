#include <Wire.h>
#include <Adafruit_INA219.h>
#include "DFRobot_OxygenSensor.h"
c  

// Initialize DFRobot Oxygen Sensor
DFRobot_OxygenSensor oxygen;  

#define Oxygen_IICAddress ADDRESS_3  // Default 0x73
#define COLLECT_NUMBER 10            // Number of samples for averaging

void setup() {
    Serial.begin(115200);

    // Initialize INA219 (Custom Pins: SDA=17, SCL=32)
    I2C_INA219.begin(17, 32, 100000);  
    if (!ina219.begin(&I2C_INA219)) {
        Serial.println("Failed to find INA219!");
        while (1);
    }
    Serial.println("INA219 Initialized!");

    // Initialize default I2C (Wire, SDA=21, SCL=22) for Oxygen Sensor
    Wire.begin(21, 22, 100000);  

    if (!oxygen.begin(Oxygen_IICAddress)) {  // Removed extra TwoWire argument
        Serial.println("Failed to find Oxygen Sensor!");
        while (1);
    }
    Serial.println("Oxygen Sensor Initialized!");
}

void loop() {
    // Read INA219 data
    float busVoltage = ina219.getBusVoltage_V();
    float shuntVoltage = ina219.getShuntVoltage_mV();
    float current_mA = ina219.getCurrent_mA();
    float power_mW = ina219.getPower_mW();

    Serial.print("INA219 - Bus Voltage: "); Serial.print(busVoltage); Serial.println(" V");
    Serial.print("INA219 - Shunt Voltage: "); Serial.print(shuntVoltage); Serial.println(" mV");
    Serial.print("INA219 - Current: "); Serial.print(current_mA); Serial.println(" mA");
    Serial.print("INA219 - Power: "); Serial.print(power_mW); Serial.println(" mW");
    
    // Read Oxygen Sensor data
    float oxygenData = oxygen.getOxygenData(COLLECT_NUMBER);
    Serial.print("Oxygen Concentration: "); Serial.print(oxygenData); Serial.println(" %");

    Serial.println("-----------------------------");
    delay(1000);  
}
