/************************************************************
  Calibration: connect the 3 pin wire to the ESP-32 (VCC, GND and Signal)
  without connecting the sensor to the water pipe and run the program
  for once. Mark down the LOWEST voltage value through the serial
  monitor and revise the "OffSet" value to complete the calibration.

  After the calibration the sensor is ready for measuring!
**************************************************************/

#define REFERENCE_VOLTAGE 3.3   // set reference voltage
#define ANALOG_RESOLUTION 4095  // set analog resolution 12-bit

#define WP1_CALIBRATION_MULTIPLIER 250  // water pressure sensor 1 calibration multiplier
#define WP2_CALIBRATION_MULTIPLIER 250  // water pressure sensor 2 calibration multiplier

const float WP1_OffSet = 0.233;  // water pressure sensor 1 offset value
const float WP2_OffSet = 0.232;  // water pressure sensor 2 offset value

// water pressure sensor 1
float readWP1() {
  float V, P, mbar;
  V = analogRead(WATER_PRESSURE_SENSOR1_PIN) * REFERENCE_VOLTAGE / ANALOG_RESOLUTION;  // Sensor output voltage
  P = (V - WP1_OffSet) * WP1_CALIBRATION_MULTIPLIER;                                   // Calculate water pressure

  // Serial.print("Voltage:");
  // Serial.print(V, 3);
  // Serial.println("V");

  // Serial.print(" Pressure:");
  // Serial.print(P, 1);
  // Serial.println(" KPa");
  // Serial.println();

  mbar = P * 10.0;  // converted kPa to mbar
  return mbar;
}


// water pressure sensor 2
float readWP2() {
  float V, P, mbar;
  V = analogRead(WATER_PRESSURE_SENSOR2_PIN) * REFERENCE_VOLTAGE / ANALOG_RESOLUTION;  // Sensor output voltage
  P = (V - WP2_OffSet) * WP2_CALIBRATION_MULTIPLIER;                                   // Calculate water pressure

  // Serial.print("Voltage:");
  // Serial.print(V, 3);
  // Serial.println("V");

  // Serial.print(" Pressure:");
  // Serial.print(P, 1);
  // Serial.println(" KPa");
  // Serial.println();

  mbar = P * 10.0;  // converted kPa to mbar
  return mbar;
}
