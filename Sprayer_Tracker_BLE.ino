#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// BLE UUIDs
#define SERVICE_UUID         "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID  "abcd1234-ab12-cd34-ef56-abcdef123456" // UUID Flow Rate, Totalizer, Lat, Long GPS
#define CHAR_UUID_BATT_FUEL  "abcd5678-ab12-cd34-ef56-abcdef123456" // UUID Battery, Fuel

BLECharacteristic *pCharacteristic;
BLECharacteristic *pCharBattFuel;

bool deviceConnected = false;

// Dummy data
uint16_t flowrate = 0;
uint32_t totalizer = 0;
float latitude = -6.200000;
float longitude = 106.816600;

uint16_t batteryVoltage = 3700;
uint8_t fuelPercent = 80;

// BLE callbacks
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  }
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

// HEX payload print
void printPayloadHex(const uint8_t *payload, size_t length) {
  Serial.print("Payload HEX [");
  Serial.print(length);
  Serial.print(" bytes]: ");
  for (size_t i = 0; i < length; i++) {
    if (payload[i] < 0x10) Serial.print("0");
    Serial.print(payload[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

// Task 1: send flowrate + GPS
void task_flow_gps_sender(void *pvParameters) {
  while (true) {
    // Dummy data update
    flowrate = random(500, 2000);
    totalizer += flowrate;
    latitude += 0.00004;
    longitude += 0.00006;

    // Format payload (14 bytes)
    uint8_t payload[14];
    payload[0] = (flowrate >> 8) & 0xFF;
    payload[1] = flowrate & 0xFF;
    payload[2] = (totalizer >> 24) & 0xFF;
    payload[3] = (totalizer >> 16) & 0xFF;
    payload[4] = (totalizer >> 8) & 0xFF;
    payload[5] = totalizer & 0xFF;

    uint8_t *latPtr = (uint8_t*)&latitude;
    payload[6]  = latPtr[3];
    payload[7]  = latPtr[2];
    payload[8]  = latPtr[1];
    payload[9]  = latPtr[0];

    uint8_t *lonPtr = (uint8_t*)&longitude;
    payload[10] = lonPtr[3];
    payload[11] = lonPtr[2];
    payload[12] = lonPtr[1];
    payload[13] = lonPtr[0];

    if (deviceConnected) {
      pCharacteristic->setValue(payload, sizeof(payload));
      pCharacteristic->notify();
      printPayloadHex(payload, sizeof(payload));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// Task 2: send battery + fuel
void task_batt_fuel_sender(void *pvParameters) {
  while (true) {
    batteryVoltage = random(3600, 4200);
    fuelPercent = random(10, 100);

    uint8_t payload[3];
    payload[0] = (batteryVoltage >> 8) & 0xFF;
    payload[1] = batteryVoltage & 0xFF;
    payload[2] = fuelPercent;

    if (deviceConnected) {
      pCharBattFuel->setValue(payload, sizeof(payload));
      pCharBattFuel->notify();
      printPayloadHex(payload, sizeof(payload));

      Serial.print("Battery: ");
      Serial.print(batteryVoltage);
      Serial.print(" mV | Fuel: ");
      Serial.print(fuelPercent);
      Serial.println(" %");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup() {
  Serial.begin(115200);

  // BLE Init
  BLEDevice::init("FlowBLE");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharBattFuel = pService->createCharacteristic(
    CHAR_UUID_BATT_FUEL,
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pService->start();
  pServer->getAdvertising()->start();

  Serial.println("BLE server ready (ESP32-C6 with FreeRTOS), waiting for client...");

  // Create two task
  xTaskCreatePinnedToCore(task_flow_gps_sender, "FlowGPS", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(task_batt_fuel_sender, "BattFuel", 2048, NULL, 1, NULL, 0);
}

void loop() {
  // Empty loop because use freertos TASK
}
