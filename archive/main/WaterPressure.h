/************************************************************
  Calibration: connect the 3 pin wire to the ESP-32 (VCC, GND and Signal)
  without connecting the sensor to the water pipe and run the program
  for once. Mark down the LOWEST voltage value through the serial
  monitor and revise the "OffSet" value to complete the calibration.

  After the calibration the sensor is ready for measuring!
**************************************************************/

#define REFERENCE_VOLTAGE       3.3      // set reference voltage
#define ANALOG_RESOLUTION       4095      // set analog resolution 12-bit
#define CALIBRATION_MULTIPLIER  250

const float  OffSet = 0.483 ;

float readWaterPressure(int WATER_PRESSURE_SENSOR_PIN){
  float V, P;
  V = analogRead(WATER_PRESSURE_SENSOR_PIN) * REFERENCE_VOLTAGE / ANALOG_RESOLUTION;     // Sensor output voltage
  P = (V - OffSet) * CALIBRATION_MULTIPLIER;                                             // Calculate water pressure

  // Serial.print("Voltage:");
  // Serial.print(V, 3);
  // Serial.println("V");

  // Serial.print(" Pressure:");
  // Serial.print(P, 1);
  // Serial.println(" KPa");
  // Serial.println();

  return P;
}