// Define a calibration offset variable
int h2CalibrationOffset = 0;  // You can adjust this offset as needed

int readH2() {
  int readADC = analogRead(H2_SENSOR_PIN);  // read sensor value
  int hydrogenData = map(readADC, 0, 4095, 0, 100);
  
  // Apply the calibration offset
  hydrogenData += h2CalibrationOffset;

  // Ensure the data stays within the expected range
  if (hydrogenData < 0) hydrogenData = 0;
  if (hydrogenData > 100) hydrogenData = 100;

  return hydrogenData;
}
