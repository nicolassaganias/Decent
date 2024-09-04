EspSoftwareSerial::UART methaneSensor;

void methaneSensorInit() {
  methaneSensor.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, CH4_SENSOR_TX_PIN, CH4_SENSOR_RX_PIN);
  if (!methaneSensor) {
    Serial.println("Methane sensor port initialization failed!");
  } else {
    Serial.println("Methane sensor port initialization successfull!");
  }
}

int readMethane(){
  if (methaneSensor.available()) {

    // Read the incoming data
    char receive_data = methaneSensor.read();
    // Serial.print("Methane HEX: 0x");
    // Serial.println(receive_data, HEX);

    // convert HEX data into integer
    int sensor_data = strtol(&receive_data, NULL, 16);
   // Serial.print("Methane % : ");
    //Serial.println(sensor_data);
    return sensor_data;
  }
}
