#include <BLEDevice.h>
#include <BLEServer.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"  // Service UUID
#define CHAR_UUID "4a37b01d-364e-44d9-9335-770e2025b29f"     // Characteristic UUID

BLEServer* pServer = NULL;                 // BLEServer object creation
BLECharacteristic* characteristic = NULL;  // All charateristic object creation
bool deviceConnected = false;              // Check BLE connection status
String receivedData;                       // Store received data
bool pairStatus = false;
int sentDataCount = 0;

// Callback function for checking the BLE is connected to anything or not
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {  // connect status
    deviceConnected = true;             // set connected status
    Serial.println("Connected.");
  };

  void onDisconnect(BLEServer* pServer) {  // disconnect status
    deviceConnected = false;               // set disconnected status
    pServer->startAdvertising();           // start advertising
    Serial.println("Disconnected.");
  }
};

// Function for sending data on desired characteristic
void sendDataToApp(String data) {
  characteristic->setValue(data.c_str());  // sending from esp32
  characteristic->notify();                // Notify the client for receiving data
}

// Callback function for receiving data from client and process it
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    receivedData = "";
    if (value.length() > 0) {
      for (int i = 0; i < value.length(); i++) {
        receivedData += value[i];
      }
    }
    if (receivedData == "pair") {
      sendDataToApp("s:pair");
      pairStatus = true;
    }
    if (receivedData == "stop") {
      sendDataToApp("stopped");
      pairStatus = false;
    }
    if (pairStatus == true) {
      if (receivedData == "od" && sentDataCount == 0) {
      sendDataToApp("co1: " + String(co2_data1));
        sentDataCount++;
      } else if (receivedData == "od" && sentDataCount == 1) {
      sendDataToApp("co2: " + String(co2_data2));
        sentDataCount++;
      } else if (receivedData == "od" && sentDataCount == 2) {
        sendDataToApp("ch4: " + String(readMethane()));
        sentDataCount++;
      } else if (receivedData == "od" && sentDataCount == 3) {
        sendDataToApp("o2: " + String(oxygen_data));
        sentDataCount++;
      } else if (receivedData == "od" && sentDataCount == 4) {
        sendDataToApp("h2: " + String(hydrogen_data));
        sentDataCount++;
      } else if (receivedData == "od" && sentDataCount == 5) {
       sendDataToApp("ph1: " + String(ph1_data));
        sentDataCount++;
      } else if (receivedData == "od" && sentDataCount == 6) {
        sendDataToApp("ph2: " + String(ph2_data));
        sentDataCount++;
      } else if (receivedData == "od" && sentDataCount == 7) {
        sendDataToApp("temp: " + String(readTemperature()));
        sentDataCount++;
      } else if (receivedData == "od" && sentDataCount == 8) {
        sendDataToApp("pressure1: " + String(readWaterPressure(WATER_PRESSURE_SENSOR1_PIN)));
        sentDataCount++;
      } else if (receivedData == "od" && sentDataCount >= 9) {
        sendDataToApp("pressure2: " + String(readWaterPressure(WATER_PRESSURE_SENSOR2_PIN)));
        sentDataCount = 0;
      }
    }
  }
};

// Configuration of BLE sever
void configBLE() {
  BLEDevice::init("DeCent");                                    // BLE Initialization
  pServer = BLEDevice::createServer();                          // Create a BLE Server
  pServer->setCallbacks(new MyServerCallbacks());               // Set the callback function for checking the device is connected or not
  BLEService* pService = pServer->createService(SERVICE_UUID);  // Create a service on the server

  // Create characteristic for send data to the mobile app
  characteristic = pService->createCharacteristic(
    CHAR_UUID, BLECharacteristic::PROPERTY_READ
                 | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  characteristic->setCallbacks(new MyCallbacks());
  pService->start();                                               // Start the server
  pServer->getAdvertising()->addServiceUUID(pService->getUUID());  // Set advertisment
  pServer->getAdvertising()->start();                              // Start advertisment
  delay(1000);                                                     // Wait a seconds
}
