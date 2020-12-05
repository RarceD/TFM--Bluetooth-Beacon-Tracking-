#include <esp_now.h>
#include <WiFi.h>

// Structure example to receive data
// Must match the sender structure
typedef struct Beacon_Information
{
  char uuid[20];
  char beacon[20];
  int rssi;
} Beacon_Information;

// Create a struct_message called myData
Beacon_Information myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);

  Serial.print("UUID ");
  for (uint8_t index = 0; index < 20; index++)
    Serial.write(myData.uuid[index]);
  Serial.println();
  Serial.print("Beacon ");
  for (uint8_t index = 0; index < 20; index++)
    Serial.write(myData.beacon[index]);
  Serial.println();
  Serial.print("RSSI: ");
  Serial.println(myData.rssi);
  Serial.println();
}

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
  esp_now_register_recv_cb(OnDataRecv);
}