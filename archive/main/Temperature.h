#include <OneWire.h>
OneWire ds(DS18S20_TEMPERATURE_SENSOR_PIN);  // Set temperature sensor on a digital pin

float temperature_data;

// read temperature sensor and process the reading data
float readTemperature() {
  byte data[12];  // store data
  byte addr[8];   // store address

  // check errors
  if (!ds.search(addr)) {  // check sensor address status
    ds.reset_search();     // no more sensors on chain, reset search
    return -1000;          // return error
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {     // check CRC
    Serial.println("CRC is not valid!");       // print CRC error
    return -1000;                              // return error
  }

  if (addr[0] != 0x10 && addr[0] != 0x28) {     // check address recognization
    Serial.print("Device is not recognized");   // print recognization error
    return -1000;                               // return error
  }

  ds.reset();         // reset and try
  ds.select(addr);    // select address
  ds.write(0x44, 1);  // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);  // Read Scratchpad

  for (int i = 0; i < 9; i++) {  // we need 9 bytes
    data[i] = ds.read();
  }
  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB);  //using two's compliment
  float temp_data = tempRead / 16;
  return temp_data;
}