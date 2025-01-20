#include <SoftwareSerial.h>

// Define the pins for RX and TX
#define CH4_SENSOR_TX_PIN 26
#define CH4_SENSOR_RX_PIN 25

EspSoftwareSerial::UART methanSerial;  // Create a SoftwareSerial object

uint8_t response[64];
uint8_t itt = 0;
unsigned long time2 = 0;

// Send command to check measurement result
uint8_t ip = 0x11;
uint8_t lb = 0x01;
uint8_t cmd = 0x01;
uint8_t df = 0x00;  // No data field for this command
uint8_t cs = 0xED;

void setup() {
  Serial.begin(115200);                                                                             // Start the hardware serial communication
  methanSerial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, CH4_SENSOR_TX_PIN, CH4_SENSOR_RX_PIN);  // Start the software serial communication
  delay(5000);
  Serial.println("Serial communication started");

  // Send the command
  methanSerial.write(ip);
  methanSerial.write(lb);
  methanSerial.write(cmd);
  methanSerial.write(cs);
  itt = 0;
}

void loop() {
  if (millis() - time2 > 5000) {
    // Send the command
    methanSerial.write(ip);
    methanSerial.write(lb);
    methanSerial.write(cmd);
    methanSerial.write(cs);
    itt = 0;
    time2 = millis();
  }

  // Check if data is available from the sensor
  if (methanSerial.available()) {
    // Read the response
    uint8_t temp = methanSerial.read();
    if (temp != 0XFF) {
      response[itt] = temp;
      // Serial.print(temp, HEX);
      // Serial.print(" ");
      itt++;
    }

    // Once we have received the entire response (7 bytes)
    if (itt >= 7) {
      uint8_t ack = response[0];
      uint8_t responseLb = response[1];
      uint8_t responseCmd = response[2];
      uint8_t df1 = response[3];  // DF1 byte
      uint8_t df2 = response[4];  // DF2 byte
      uint8_t st1 = response[5];
      uint8_t st2 = response[6];

      // Calculate gas concentration
      int gasConcentration = (df1 * 256 + df2) / 100;

      // Print the data to the Serial Monitor
      Serial.print("Gas Concentration: ");
      Serial.print(gasConcentration);
      Serial.println("%");

      // Reset iterator for the next response
      itt = 0;
    }
  }
}
