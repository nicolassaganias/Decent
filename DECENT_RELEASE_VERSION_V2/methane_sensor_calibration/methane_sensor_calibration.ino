#include <SoftwareSerial.h>

// Define the pins for RX and TX
#define CH4_SENSOR_TX_PIN 26
#define CH4_SENSOR_RX_PIN 25

EspSoftwareSerial::UART methanSerial;  // Create a SoftwareSerial object

uint8_t response[64];
uint8_t responseIndex = 0;
unsigned long lastCommandTime = 0;

// Zero adjustment command (defined as per datasheet)
const uint8_t zeroAdjustCommand[] = { 0x11, 0x01, 0x03, 0xEB };

void setup() {
  Serial.begin(115200);                                                                             // Start the hardware serial communication
  methanSerial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, CH4_SENSOR_TX_PIN, CH4_SENSOR_RX_PIN);  // Start the software serial communication

  delay(5000);  // Allow time for sensor stabilization
  Serial.println("Serial communication started. Initializing Zero Calibration...");

  // Send the initial zero adjustment command
  sendZeroAdjustmentCommand();
  for (int i = 0; i <= 5; i++) {
    if(processSensorResponse()){
      break;
    }
    delay(500);
  }
}

void loop() {
  if (Serial.available() > 0) {
    if (Serial.read() == 'r') {
      Serial.println("Restarting...");
      delay(500);
      ESP.restart();
    }
  }
  // Send the zero adjustment command every 5 seconds
  //if (millis() - lastCommandTime > 5000) {
  //  sendZeroAdjustmentCommand();
  //   lastCommandTime = millis();
  // }

  // Process the sensor's response
  //processSensorResponse();
}

// Function to send the zero adjustment command
void sendZeroAdjustmentCommand() {
  Serial.println("Sending Zero Adjustment Command...");
  methanSerial.write(zeroAdjustCommand, sizeof(zeroAdjustCommand));
}

// Function to process sensor responses
bool processSensorResponse() {
  while (methanSerial.available()) {
    uint8_t byteReceived = methanSerial.read();

    // Avoid storing invalid data
    if (byteReceived != 0xFF) {
      response[responseIndex++] = byteReceived;
      Serial.print(byteReceived, HEX);
      Serial.print(" ");
    }

    // Check if the full response is received (response size: 4 bytes for zero adjustment)
    if (responseIndex >= 4) {
      Serial.println("\nComplete Response Received:");

      // Parse and debug the response
      uint8_t ack = response[0];       // Acknowledgment byte
      uint8_t length = response[1];    // Length byte
      uint8_t cmd = response[2];       // Command echo
      uint8_t checksum = response[3];  // Checksum byte

      // Debugging output
      Serial.print("ACK: ");
      Serial.println(ack, HEX);
      Serial.print("Length: ");
      Serial.println(length);
      Serial.print("Command: ");
      Serial.println(cmd, HEX);
      Serial.print("Checksum: ");
      Serial.println(checksum, HEX);

      // Validate response
      if (ack == 0x16 && cmd == 0x03 && validateChecksum(response, 4)) {
        Serial.println("Zero Adjustment Successful!");
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

// Function to validate the checksum
bool validateChecksum(uint8_t* data, uint8_t length) {
  uint8_t calculatedChecksum = 0;
  for (uint8_t i = 0; i < length - 1; i++) {
    calculatedChecksum += data[i];
  }
  calculatedChecksum = -calculatedChecksum;  // Two's complement
  return (calculatedChecksum == data[length - 1]);
}
