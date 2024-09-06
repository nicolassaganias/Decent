 int hydrogen_data;
 
 void readHydrogen(){
  int read_adc = analogRead(H2_SENSOR_PIN);      // read sensor value
  hydrogen_data = map(read_adc, 0, 4095, 0, 100);
  delay(100);
}