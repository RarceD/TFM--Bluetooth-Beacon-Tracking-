#include <esp_now.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEAddress.h>
#include <BLEUUID.h>

#define DEBUGGING
#ifdef DEBUGGING
#define LOGLN(x) Serial.println(x)
#define LOG(x) Serial.print(x)
#else
#define LOG(x)
#define LOGLN(x)
#endif
// 7C:9E:BD:03:31:14 mac of the listenner
uint8_t broadcastAddress[] = {0x3C, 0x71, 0xBF, 0x04, 0x30, 0xB0};
// uint8_t broadcastAddress[] = {0x84, 0x0D, 0x8E, 0x38, 0xED, 0x54};
// 84:0D:8E:38:ED:54
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 5        /* Time ESP32 will go to sleep (in seconds) */
int scanTime = 2;              //In seconds
BLEScan *pBLEScan;
bool match;
unsigned long schedule = 0;
long timerMatch = 0;
long timerScan = 0;
RTC_DATA_ATTR int bootCount = 0;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    // Serial.printf("Advertised Device: %s ", advertisedDevice.toString().c_str());
    int rssi = advertisedDevice.getRSSI();
    // if (advertisedDevice.haveServiceUUID())
    // {
    //   // Serial.printf("Name -> %s", advertisedDevice.getServiceUUID().toString().c_str());
    // }
    //Serial.printf(", rssi -> %i \n", rssi); advertisedDevice.getName()
    const char *macAdress = advertisedDevice.getAddress().toString().c_str();
    //c4:64:e3:f9:35:b3 black
    //e6:13:a7:0b:4f:b2 white
    char *knownAdress = "c4:64:e3:f9:35:b3";
    char *knownAdress_2 = "e6:13:a7:0b:4f:b2";
    Serial.printf(" Adress: %s // RSSI: %d \n", macAdress, advertisedDevice.getRSSI());
    //Serial.printf("Searching for: %s \n", knownAdress);
    int val1 = 0;
    int val2 = 0;

    for (int i = 0; i < 10; i++)
    {
      if (knownAdress[i] == macAdress[i])
        val1++;
      if (knownAdress_2[i] == macAdress[i])
        val2++;
    }
    // if (val1 > 8 || val2 > 8)
    // {
    char snum[25];
    //I calculate the aproximate distance:
    double distance = 0.0012f * rssi * rssi - 0.0395f * (-rssi) + 0.1818;
    itoa(rssi, snum, 10);

    match = true;
    timerMatch = millis();
    // digitalWrite(LED_ORANGE_UP, HIGH);
    // digitalWrite(LED_ORANGE_DOWN, HIGH);

    // const size_t capacity = JSON_ARRAY_SIZE(2) + 3 * JSON_OBJECT_SIZE(2);
    // DynamicJsonBuffer jsonBuffer(capacity);
    // JsonObject &root = jsonBuffer.createObject();
    // root["esp"] = "A2";
    // JsonArray &beacon = root.createNestedArray("beacon");

    // JsonObject &beacon_0 = beacon.createNestedObject();
    // beacon_0["uuid"] = macAdress;
    // beacon_0["distance"] = rssi;

    if (strcmp(macAdress, "") != 0)
    {
      char JSONmessageBuffer[100];
      // root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      // clientMQTT.publish("master_beacon", JSONmessageBuffer);
      LOGLN("I post  \n");
    }
    // }
    // clientMQTT.publish("master_beacon", "test");
  }
};

typedef struct Beacon_Information
{
  char uuid[20];
  char beacon[20];
  int rssi;
} Beacon_Information;

void setup()
{
  // Init Serial Monitor
  Serial.begin(115200);
  Serial.println(millis());

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                 " Seconds");
  Serial.println("Going to sleep now");
  Serial.flush();

  BLEDevice::init("");
  BLEScan *pBLEScan;
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
  // digitalWrite(LED_ORANGE_UP, HIGH);
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  LOGLN("Devices found: ");
  LOGLN(foundDevices.getCount());
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  // digitalWrite(LED_ORANGE_UP, LOW);
  esp_bt_controller_deinit();

  Beacon_Information myData;
  strcpy(myData.beacon, "c4:64:e3:f9:35:b3");
  strcpy(myData.uuid, "c4:64:e3:f9:35:b3");
  myData.rssi = -62;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }
  Serial.println(millis());

  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop()
{
  // Set values to send
  delay(2000);
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}