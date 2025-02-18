#include <SoftwareSerial.h>
EspSoftwareSerial::UART mySerial;

#define CO2_SENSOR1_RX_PIN 4
#define CO2_SENSOR1_TX_PIN 23
#define CO2_SENSOR2_RX_PIN 18
#define CO2_SENSOR2_TX_PIN 19

int co2 = 0;
double multiplier = 1;  // 1 for 2% =20000 PPM, 10 for 20% = 200,000 PPM
uint8_t buffer[25];  // Sufficient size to hold incoming data
uint8_t ind = 0;
uint8_t bufIndex = 0;

int fill_buffer();  // function prototypes here
int format_output();

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, CO2_SENSOR1_TX_PIN, CO2_SENSOR1_RX_PIN);  // Start serial communications with sensor
  mySerial.println("M 6");  // send Mode for Z and z outputs
  mySerial.println("K 1");  // set streaming mode
}

void loop() {
  if (fill_buffer() == 0) {  // function call that reads CO2 sensor and fills buffer
    format_output();
    Serial.print(" Raw PPM ");
    bufIndex = 8;  // In ASCII buffer, filtered value is offset from raw by 8
    format_output();
    Serial.println(" Filtered PPM\n\n");
  }
  delay(1000);
}

int fill_buffer(void) {
  // Clear the buffer
  memset(buffer, 0, sizeof(buffer));
  ind = 0;

  // Fill buffer with sensor ASCII data
  unsigned long startTime = millis();
  while (true) {
    if (mySerial.available()) {
      buffer[ind] = mySerial.read();
      if (buffer[ind] == 0x0A) {  // Check for end of line character
        break;
      }
      ind++;
      if (ind >= sizeof(buffer)) {  // Prevent buffer overflow
        return -1;  // Return error if buffer size exceeds limit
      }
    }
    if (millis() - startTime > 1000) {  // Timeout after 1 second
      return -1;  // Return error if data read times out
    }
  }

  // Ensure the buffer has valid data length
  if (ind < 16) {  // Minimum length of expected data
    return -1;  // Return error if data length is insufficient
  }

  // buffer now filled with sensor ASCII data
  // ind contains the number of characters loaded into buffer up to 0x0A = CR
  return 0;
}

int format_output(void) {
  if (ind < 16) {
    Serial.println("Insufficient data in buffer.");
    return -1;  // Return error if data length is insufficient
  }

  // Extract and convert ASCII characters to PPM value
  co2 = buffer[15 - bufIndex] - 0x30;
  co2 = co2 + ((buffer[14 - bufIndex] - 0x30) * 10);
  co2 += (buffer[13 - bufIndex] - 0x30) * 100;
  co2 += (buffer[12 - bufIndex] - 0x30) * 1000;
  co2 += (buffer[11 - bufIndex] - 0x30) * 10000;

  // Check if CO2 value is negative and ignore if so
  if (co2 < 0) {
   // Serial.println("Negative CO2 value detected. Ignoring...");
    return -1;  // Return error if the value is negative
  }

  Serial.print("\n CO2 = ");
  Serial.print(co2 * multiplier, 0);

  delay(200);
  return 0;
}
