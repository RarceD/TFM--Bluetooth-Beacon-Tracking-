#include "header.h"
#include "wifi_config.h"
#include "app_responses.h"
// #include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEAddress.h>
#include <BLEUUID.h>

// #include <PubSubClient.h>
// #include <ArduinoJson.h>

#define LED_GREEN 25
#define LED_RED 26
#define LED_ORANGE_UP 27
#define LED_ORANGE_DOWN 13

#define mqtt_port 1883
#define MQTT_USER "d"
#define MQTT_PASSWORD "d"
#define MQTT_SERIAL_PUBLISH_CH "master_beacon"
#define MQTT_SERIAL_RECEIVER_CH "master_beaconn"
#define DEBUG
#ifdef DEBUG
#define DPRINT(x) Serial.println(x)
#else
#define DPRINT(x)
#endif

int scanTime = 2; //In seconds
BLEScan *pBLEScan;
bool match;
unsigned long schedule = 0;
long timerMatch = 0;
long timerScan = 0;
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 1        /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    DPRINT("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    DPRINT("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    DPRINT("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    DPRINT("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    DPRINT("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }
}

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
      digitalWrite(LED_ORANGE_UP, HIGH);
      digitalWrite(LED_ORANGE_DOWN, HIGH);

      const size_t capacity = JSON_ARRAY_SIZE(2) + 3 * JSON_OBJECT_SIZE(2);
      DynamicJsonBuffer jsonBuffer(capacity);
      JsonObject &root = jsonBuffer.createObject();
      root["esp"] = "A2";
      JsonArray &beacon = root.createNestedArray("beacon");

      JsonObject &beacon_0 = beacon.createNestedObject();
      beacon_0["uuid"] = macAdress;
      beacon_0["distance"] = rssi;

      if (strcmp(macAdress, "") != 0)
      {
        char JSONmessageBuffer[100];
        root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
        clientMQTT.publish("master_beacon", JSONmessageBuffer);
        DPRINT("I post  \n");
      }
    // }
    // clientMQTT.publish("master_beacon", "test");
  }
};

void setup()
{
  Serial.begin(115200);
  //I initialize the reset pin to the wifi:
  pinMode(BUTTON_RESET, INPUT);
  Serial.println(ESP.getFlashChipSize());
  //Start and led red on
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  //Connect to wifi or try it at least:
  auto_wifi_connect();
  //Connect to mqtt and publish
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, HIGH);
  init_mqtt();
  //Connected to broker and led green on
  pinMode(LED_ORANGE_UP, OUTPUT);
  digitalWrite(LED_ORANGE_UP, LOW);

  pinMode(LED_ORANGE_DOWN, OUTPUT);
  digitalWrite(LED_ORANGE_DOWN, HIGH);
  uint32_t repeat_ble = millis();
  uint32_t repeat_mqtt = millis();

  while (true)
  {
    //Handler timers to publish:
    if (millis()-repeat_mqtt >=13000)
    {
      digitalWrite(LED, HIGH);
      // uint32_t isrCount = 0, isrTime = 0;
      // // Read the interrupt count and time
      // portENTER_CRITICAL(&timerMux);
      // isrCount = isrCounter;
      // isrTime = lastIsrAt;
      // portEXIT_CRITICAL(&timerMux);
      // Serial.print("Counter ISR:");
      // Serial.println(isrCount);
      // Serial.println(sys.selector_auto);
      // if (isrCount % 21 == 0)
      // {
        // publish_counter++;
        // char json[100];
        // DynamicJsonBuffer jsonBuffer(32);
        // JsonObject &root = jsonBuffer.createObject();
        // root.set("publish", publish_counter);
        // root.set("heap", esp_get_free_heap_size());
        // root.set("heap_min", esp_get_minimum_free_heap_size());
        // root.set("uptime", isrTime);
        // root.printTo(json);
        // client.publish((String(sys.uuid_) + "/uptime").c_str(), (const uint8_t *)json, strlen(json), false);
        // Serial.println("Publish");
        Serial.println(esp_get_free_heap_size());
        LOGLN("mqtt task");
        // clientMQTT.publish("master_beacon", "{test:true}");
        repeat_mqtt = millis();
      
      
    }
    if (millis() - repeat_ble >= 10000)
    {
      BLEDevice::init("");
      BLEScan *pBLEScan;
      pBLEScan = BLEDevice::getScan(); //create new scan
      pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
      pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
      pBLEScan->setInterval(100);
      pBLEScan->setWindow(99); // less or equal setInterval value
      digitalWrite(LED_ORANGE_UP, HIGH);
      BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
      DPRINT("Devices found: ");
      DPRINT(foundDevices.getCount());
      pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
      digitalWrite(LED_ORANGE_UP, LOW);
      esp_bt_controller_deinit();
      repeat_ble = millis();
    }
  }
}

void loop()
{
}
