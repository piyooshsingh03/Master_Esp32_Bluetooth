#include <Arduino.h>
#include <NimBLEDevice.h>
#include <nimconfig.h>
#define Service_1_UUID "374e714b-0838-4d2e-8ae3-fb28c585474b"
#define Charateristic_1_UUID "41a1a8a8-695c-44fd-aa60-5f258be92169"
static NimBLEServer* pServer;
static NimBLEService*pService;
static NimBLECharacteristic*pCharacteristic;
bool deviceConnected = false;
static int received;
uint8_t slaveflag=0;
int count=0;
int led=2;
class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        digitalWrite(led,HIGH);
         deviceConnected = true;
        Serial.printf("Client address: %s\n", connInfo.getAddress().toString().c_str());
         NimBLEDevice::startAdvertising();
        pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 180);
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        digitalWrite(led,LOW);
        Serial.printf("Client disconnected - start advertising\n");
        NimBLEDevice::startAdvertising();
    }

} serverCallbacks;

/** Handler class for characteristic actions */
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        uint32_t currentMillis= (millis()/1000);
        pCharacteristic->setValue(currentMillis);
        Serial.printf("millis - %d\n",currentMillis);
    }

    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        std::string value = pCharacteristic->getValue();
       String received = String(value.c_str()); 
    Serial.printf("As text: %s\n", received.c_str());
     int sepIndex = received.indexOf(','); 
    if (sepIndex != -1) {
        // Format = key:value
        String key = received.substring(0, sepIndex);
        uint8_t valStr = received.substring(sepIndex + 1).toInt();

        Serial.printf("Key: %s, Value(str): %d\n", key.c_str(), valStr);
    }
        // if (value.length() == sizeof(int)) {
    //     memcpy(&received, value.data(), sizeof(int));  // copy raw bytes into int
    //    Serial.printf("Receive data: %d\n", received);
    // } 
    }
} chrCallbacks;

void setup(void) {
    Serial.begin(115200);
    Serial.printf("Starting NimBLE Server\n");
     pinMode(led,OUTPUT);
    /** Initialize NimBLE and set the device name */
    NimBLEDevice::init("NimBLE");
// creat server
    pServer = NimBLEDevice::createServer();
// Servercallback
    pServer->setCallbacks(&serverCallbacks);
// Creat service
    pService = pServer->createService(Service_1_UUID);
// Creat Characteristic
    pCharacteristic =
    pService->createCharacteristic(Charateristic_1_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
    pCharacteristic->setCallbacks(&chrCallbacks);

     pService->start();
    /** Create an advertising instance and add the services to the advertised data */
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName("NimBLE-Server");
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->enableScanResponse(true);
    pAdvertising->start();
    Serial.printf("Advertising Started\n");
}

void loop() {
    static unsigned long lastMillis = 0;

    if (millis() - lastMillis > 1000) { // every 2 seconds
        lastMillis = millis();
        count++; 
        char buffer[20]="Check Master,1";
        if(count%3==0)
        {
           pCharacteristic->setValue((uint8_t*)& buffer, strlen( buffer));
            pCharacteristic->notify();   
        }            
    }
}
