#include "DFRobot_ESP_PH.h"
#include <EEPROM.h>

DFRobot_ESP_PH ph1;
DFRobot_ESP_PH ph2;

#define ESPADC 4096.0    // the esp Analog Digital Conversion value
#define ESPVOLTAGE 3300  // the esp voltage supply value
#define PH_PIN1 15       // the esp gpio data pin number for the first pH sensor
#define PH_PIN2 32       // the esp gpio data pin number for the second pH sensor

float voltage1, voltage2, phValue1, phValue2, temperature = 25;
bool calibration_mode = false;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(64);  // increase EEPROM size if necessary to store more calibration data
  ph1.begin();
  ph2.begin();
}

void loop() {
  if (calibration_mode == false) {
    static unsigned long timepoint = millis();
    if (millis() - timepoint > 1000U) {
      timepoint = millis();

      // Read voltage from the first pH sensor
      voltage1 = analogRead(PH_PIN1) / ESPADC * ESPVOLTAGE;
      Serial.print("voltage1: ");
      Serial.println(voltage1, 4);

      // Read voltage from the second pH sensor
      voltage2 = analogRead(PH_PIN2) / ESPADC * ESPVOLTAGE;
      Serial.print("voltage2: ");
      Serial.println(voltage2, 4);

      // Print temperature
      Serial.print("temperature: ");
      Serial.print(temperature, 1);
      Serial.println("°C");

      // Convert voltage to pH with temperature compensation
      phValue1 = ph1.readPH(voltage1, temperature);
      Serial.print("pH1: ");
      Serial.println(phValue1, 4);

      phValue2 = ph2.readPH(voltage2, temperature);
      Serial.print("pH2: ");
      Serial.println(phValue2, 4);
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

float readTemperature() {
  // Add your code here to get the temperature from your temperature sensor
}
