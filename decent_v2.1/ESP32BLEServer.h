#include "HardwareSerial.h"
#include "esp32-hal-gpio.h"
#include <BLEDevice.h>
#include <BLEServer.h>

// service uuid
#define SERVICE_UUID "4FAFC201-1FB5-459E-8FCC-C5C9C331914B"

// characteristic uuid
#define SENSOR_CHAR_UUID "4A37B01D-364E-44D9-9335-770E2025B29A"
#define MODE_CHAR_UUID "4A37B01D-364E-44D9-9335-770E2025B29B"
#define PUMP_CHAR_UUID "4A37B01D-364E-44D9-9335-770E2025B29C"

// create BLE instances
BLEServer* pServer = NULL;
BLECharacteristic* sensorCharacteristic = NULL;
BLECharacteristic* modeCharacteristic = NULL;
BLECharacteristic* pumpCharacteristic = NULL;

// global variables
unsigned long sendIntervalTime = 0;
int sentDataCount = 0;
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Connected.");
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    pServer->startAdvertising();
    Serial.println("Disconnected.");
    deviceConnected = false;
  }
};

// Callback function for handling mode
class ModeCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    Serial.print("Mode Characteristic Received: ");
    Serial.println(value.c_str());
    if (value.length() > 0) {
      String receivedMode;
      for (int i = 0; i < value.length(); i++) {
        receivedMode += value[i];
      }
      if (receivedMode == "auto") {
        Serial.println("received : Auto");
        manualMode = 0;  // auto mode
      } else if (receivedMode == "manual") {
        Serial.println("received : Manual");
        manualMode = 1;  // manual mode
        digitalWrite(LIQUID_PUMP1, LOW);
        digitalWrite(LIQUID_PUMP2, LOW);
        digitalWrite(LIQUID_PUMP3, LOW);
        digitalWrite(GAS_PUMP1, LOW);
        digitalWrite(GAS_PUMP2, LOW);
        digitalWrite(RELAY_PIN, LOW);
      } else {
        Serial.println("Invalid mode received: " + receivedMode);
      }
    }
  }
};

// Callback function for handling pump
class PumpCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    Serial.print("Pump characteristic received: ");
    Serial.println(value.c_str());
    if (value.length() > 0) {
      String command;
      for (int i = 0; i < value.length(); i++) {
        command += value[i];
      }
      // when current mode is manual
      if (manualMode) {
        Serial.println("### Manual Mode Command Received ###");
        if (command.startsWith("p1:")) {
          int pumpState = (command == "p1:1") ? HIGH : LOW;
          Serial.println("PUMP 1 : " + String(pumpState));
          digitalWrite(LIQUID_PUMP1, pumpState);
          pumpCharacteristic->setValue(command.c_str());
          pumpCharacteristic->notify();
        } else if (command.startsWith("p2:")) {
          int pumpState = (command == "p2:1") ? HIGH : LOW;
          Serial.println("PUMP 2 : " + String(pumpState));
          digitalWrite(LIQUID_PUMP2, pumpState);
          pumpCharacteristic->setValue(command.c_str());
          pumpCharacteristic->notify();
        } else if (command.startsWith("p3:")) {
          int pumpState = (command == "p3:1") ? HIGH : LOW;
          Serial.println("PUMP 3 : " + String(pumpState));
          digitalWrite(LIQUID_PUMP3, pumpState);
          pumpCharacteristic->setValue(command.c_str());
          pumpCharacteristic->notify();
        } else if (command.startsWith("p4:")) {
          int pumpState = (command == "p4:1") ? HIGH : LOW;
          Serial.println("PUMP 4 : " + String(pumpState));
          digitalWrite(GAS_PUMP1, pumpState);
          pumpCharacteristic->setValue(command.c_str());
          pumpCharacteristic->notify();
        } else if (command.startsWith("p5:")) {
          int pumpState = (command == "p5:1") ? HIGH : LOW;
          Serial.println("PUMP 5 : " + String(pumpState));
          digitalWrite(GAS_PUMP2, pumpState);
          pumpCharacteristic->setValue(command.c_str());
          pumpCharacteristic->notify();
        } else {
          Serial.println("Invalid Pump Command: " + command);
        }
        Serial.println();
        Serial.flush();
      }
    }
  }
};

// class SensorCallbacks : public BLECharacteristicCallbacks {
//   void onWrite(BLECharacteristic* pCharacteristic) {
//     std::string value = pCharacteristic->getValue();
//     String sensorData = "";
//     if (value.length() > 0) {
//       for (int i = 0; i < value.length(); i++) {
//         sensorData += value[i];
//       }
//     }
//   }
// };


// Function to sending data
void sendDataToApp(String data) {
  sensorCharacteristic->setValue(data.c_str());
  sensorCharacteristic->notify();
}

void BLEServerInit() {
  BLEDevice::init("DeCent");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService* pService = pServer->createService(SERVICE_UUID);

  // Create characteristic for SENSOR_CHAR_UUID
  sensorCharacteristic = pService->createCharacteristic(SENSOR_CHAR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  //sensorCharacteristic->setCallbacks(new SensorCallbacks());

  // Create characteristic for MODE_CHAR_UUID
  modeCharacteristic = pService->createCharacteristic(MODE_CHAR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  modeCharacteristic->setCallbacks(new ModeCharacteristicCallbacks());

  // Create characteristic for PUMP_CHAR_UUID
  pumpCharacteristic = pService->createCharacteristic(PUMP_CHAR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  pumpCharacteristic->setCallbacks(new PumpCharacteristicCallbacks());

  pService->start();
  pServer->getAdvertising()->addServiceUUID(pService->getUUID());
  pServer->getAdvertising()->start();
  delay(1000);
}
