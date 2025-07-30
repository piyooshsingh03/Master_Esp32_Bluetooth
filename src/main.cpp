#include <Arduino.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
bool connect_flag = false;

// Replace with your Slave's MAC address
uint8_t slaveAddress[] = {0xFC, 0xF5, 0xC4, 0x01, 0x57, 0x7E}; // <- Replace this

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_MASTER", true);  // true = master mode
  Serial.println("Master started.");
}

void loop() {
  if (!connect_flag) {
    Serial.print("Trying to connect to slave... ");
    if (SerialBT.connect(slaveAddress)) {
      Serial.println("Connected!");
      connect_flag = true;
    } else {
      Serial.println("Failed. Will retry in 2 seconds.");
      delay(1000);
      return;
    }
  }

  if (SerialBT.connected()) {
    SerialBT.println("CMD1:123");
    delay(500); // Slow down!
  } else {
    Serial.println("Disconnected. Trying to reconnect...");
    connect_flag = false;  // Try again
  }

  if (SerialBT.available()) {
    String received = SerialBT.readStringUntil('\n');
    int sepIndex = received.indexOf(':');
    if (sepIndex != -1) {
      String flag = received.substring(0, sepIndex);
      int slaveack = received.substring(sepIndex + 1).toInt();

      if (flag == "CMD1") {
        Serial.println("Motor speed set to " + String(slaveack));
      }
    }
  }
}
