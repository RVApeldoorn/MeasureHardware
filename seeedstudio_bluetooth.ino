#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define TRIG_PIN 20
#define ECHO_PIN 18

BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;
bool deviceConnected = false;

// Unieke BLE-service en characteristic UUID's
#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "12345678-1234-5678-1234-56789abcdef1"

// Callback voor BLE-verbindingen
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    }
    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

// Functie om afstand te meten
float measureDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = duration * 0.034 / 2;
    return distance;
}

void setup() {
    Serial.begin(115200);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // BLE-apparaat initialiseren
    BLEDevice::init("ESP32-C6 Ultrasoon");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Service aanmaken
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );

    // BLE starten
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();

    Serial.println("BLE Sensor gestart!");
}

void loop() {
    if (deviceConnected) {
        float distance = measureDistance();
        Serial.print("Afstand: ");
        Serial.print(distance);
        Serial.println(" cm");

        // Data als string versturen
        String distanceStr = String(distance) + " cm";
        pCharacteristic->setValue(distanceStr.c_str());
        pCharacteristic->notify();  // Stuur update naar verbonden apparaat

        delay(1000);  // Even wachten voor de volgende meting
    }
}
