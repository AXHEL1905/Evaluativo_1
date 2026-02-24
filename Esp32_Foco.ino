#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Pines LEDs
#define LED_ROJO 25
#define LED_AZUL 26
#define LED_VERDE 27

// Relevador
const int relayPin = 2;

// UUIDs
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Callbacks conexión BLE
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    // Solo verde encendido
    digitalWrite(LED_ROJO, LOW);
    digitalWrite(LED_AZUL, LOW);
    digitalWrite(LED_VERDE, HIGH);
    Serial.println("Conectado");
  }

  void onDisconnect(BLEServer* pServer) {
    // Solo rojo encendido
    digitalWrite(LED_ROJO, HIGH);
    digitalWrite(LED_AZUL, LOW);
    digitalWrite(LED_VERDE, LOW);

    BLEDevice::startAdvertising();
    Serial.println("Desconectado");
  }
};

// Recibir datos
class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    if (value.length() > 0) {
      if (value[0] == '1') digitalWrite(relayPin, LOW);
      if (value[0] == '0') digitalWrite(relayPin, HIGH);
    }
  }
};

void setup() {
  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);

  // Al encender: buscando / listo (solo azul)
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_AZUL, HIGH);
  digitalWrite(LED_VERDE, LOW);

  BLEDevice::init("ESP32_BLE_Axhel");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("ESP32 buscando conexion BLE...");
}

void loop() {
  delay(10);
}