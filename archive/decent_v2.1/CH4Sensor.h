#include <SoftwareSerial.h>

#define CH4_SENSOR_TX_PIN 26
#define CH4_SENSOR_RX_PIN 25

EspSoftwareSerial::UART CH4Serial;

uint8_t response[64];
uint8_t itt = 0;
uint8_t responseIndex = 0;
const uint8_t zeroAdjustCommand[] = { 0x11, 0x01, 0x03, 0xEB };
const uint8_t zeroPointCommand[] = { 0x11, 0x04, 0x4B, 0x00, 0x00, 0x00, 0xA0 };
const uint8_t middlePointCommand[] = { 0x11, 0x04, 0x4E, 0x00, 0x13, 0x88, 0x4A };
const uint8_t factoryResetCommand[] = { 0x11, 0x02, 0x4D, 0x00, 0xA0 };

// CH4 initialize
void CH4Init() {
  CH4Serial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, CH4_SENSOR_TX_PIN, CH4_SENSOR_RX_PIN);
  delay(1000);
}

// read CH4 sensor data
void readCH4() {
  CH4Serial.write(0x11);
  CH4Serial.write(0x01);
  CH4Serial.write(0x01);
  CH4Serial.write(0xED);
  itt = 0;
  for (int i = 0; i <= 5; i++) {
    while (CH4Serial.available()) {
      uint8_t temp = CH4Serial.read();
      if (temp != 0XFF) {
        response[itt] = temp;
        itt++;
      }
      if (itt >= 7) {
        uint8_t ack = response[0];
        uint8_t responseLb = response[1];
        uint8_t responseCmd = response[2];
        uint8_t df1 = response[3];  // DF1 byte
        uint8_t df2 = response[4];  // DF2 byte
        uint8_t st1 = response[5];
        uint8_t st2 = response[6];

        // Calculate gas concentration and apply calibration offset
        float gasConcentration = (df1 * 256 + df2) / 100.0;

        itt = 0;
        CH4SensorData = int(gasConcentration);
        break;
      }
    }
  }
}

bool validateChecksum(uint8_t* data, uint8_t length) {
  uint8_t calculatedChecksum = 0;
  for (uint8_t i = 0; i < length - 1; i++) {
    calculatedChecksum += data[i];
  }
  calculatedChecksum = -calculatedChecksum;  // Two's complement
  return (calculatedChecksum == data[length - 1]);
}

bool sensorResponse() {
  while (CH4Serial.available()) {
    uint8_t byteReceived = CH4Serial.read();
    if (byteReceived != 0xFF) {
      response[responseIndex++] = byteReceived;
      Serial.print(byteReceived, HEX);
      Serial.print(" ");
    }
    if (responseIndex >= 4) {
      Serial.println("\nComplete Response Received:");
      uint8_t ack = response[0];
      uint8_t length = response[1];
      uint8_t cmd = response[2];
      uint8_t checksum = response[3];

      // Debugging output
      Serial.print("ACK: ");
      Serial.println(ack, HEX);
      Serial.print("Length: ");
      Serial.println(length);
      Serial.print("Command: ");
      Serial.println(cmd, HEX);
      Serial.print("Checksum: ");
      Serial.println(checksum, HEX);

      if (ack == 0x16 && cmd == 0x03 && validateChecksum(response, 4)) {
        Serial.println("Zero Adjustment Successful!");
        responseIndex = 0;
        return true;
      } else if (ack == 0x16 && cmd == 0x4B && validateChecksum(response, 4)) {
        Serial.println("Zero Point Calibration Successful!");
        responseIndex = 0;
        return true;
      } else if (ack == 0x16 && cmd == 0x4E && validateChecksum(response, 4)) {
        Serial.println("Middle Point Calibration Successful!");
        responseIndex = 0;
        return true;
      } else if (ack == 0x16 && cmd == 0x4D && validateChecksum(response, 4)) {
        Serial.println("Reset Factory Calibration Successful!");
        responseIndex = 0;
        return true;
      } else {
        Serial.println("Invalid Response or Checksum Error!");
        responseIndex = 0;
        return false;
      }
    }
  }
}

// CH4 calibration function
void CH4Calibration(int command) {
  switch (command) {
    case 1:
      CH4Serial.write(zeroAdjustCommand, sizeof(zeroAdjustCommand));
      for (int i = 0; i <= 5; i++) {
        if (sensorResponse()) {
          break;
        }
        delay(500);
      }
      break;

    case 2:
      CH4Serial.write(zeroPointCommand, sizeof(zeroPointCommand));
      for (int i = 0; i <= 5; i++) {
        if (sensorResponse()) {
          break;
        }
        delay(500);
      }
      break;

    case 3:
      CH4Serial.write(middlePointCommand, sizeof(middlePointCommand));
      for (int i = 0; i <= 5; i++) {
        if (sensorResponse()) {
          break;
        }
        delay(500);
      }
      break;

    case 4:
      CH4Serial.write(factoryResetCommand, sizeof(factoryResetCommand));
      for (int i = 0; i <= 5; i++) {
        if (sensorResponse()) {
          break;
        }
        delay(500);
      }
      break;

    default:
      break;
  }
}
