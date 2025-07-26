#include <Arduino.h>
#include <Wire.h>
#include <VL53L1X.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "12345678-1234-5678-1234-56789abcdef1"
#define CMD_CHARACTERISTIC_UUID "12345678-1234-5678-1234-56789abcdef2"

BLECharacteristic *pCmdCharacteristic = nullptr;
BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;
bool deviceConnected = false;

VL53L1X sensor;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Verbinding gemaakt!");
        pServer->getAdvertising()->stop();
    }
    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Verbinding verbroken");
        pServer->getAdvertising()->start();
    }
};

class CommandCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        String value = pCharacteristic->getValue();
        if (value == "start") {
            Serial.println("Startcommando ontvangen!");
            const int numMeasurements = 10;
            uint16_t sum = 0;
            int validMeasurements = 0;

            for (int i = 0; i < numMeasurements; i++) {
                while (!sensor.dataReady()) delay(1);
                uint16_t distance = sensor.read();
                if (!sensor.timeoutOccurred()) {
                    sum += distance;
                    validMeasurements++;
                }
            }

            if (validMeasurements > 0) {
                float gemiddelde = (float)sum / validMeasurements;
                Serial.print("Gemiddelde afstand: ");
                Serial.print(gemiddelde);
                Serial.println(" mm");
                String result = String((int)gemiddelde);
                ::pCharacteristic->setValue(result.c_str());
                ::pCharacteristic->notify();
                Serial.println("Notificatie verzonden: " + result);
            } else {
                ::pCharacteristic->setValue("0");
                ::pCharacteristic->notify();
                Serial.println("No valid measurements, sent 0");
            }
        }
    }
};

void setup() {
    Serial.begin(115200);
    Wire.begin();
    sensor.setTimeout(500);
    if (!sensor.init()) {
        Serial.println("VL53L1X sensor niet gevonden!");
        while (1);
    }
    sensor.setDistanceMode(VL53L1X::Long);
    sensor.setMeasurementTimingBudget(100000);
    sensor.startContinuous(100);
    BLEDevice::init("ESP32-C6 Ultrasoon");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pCmdCharacteristic = pService->createCharacteristic(
        CMD_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    pCmdCharacteristic->setCallbacks(new CommandCallback());
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();
    Serial.println("BLE Sensor gestart!");
}

void loop() {}