#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEAddress.h>
#include <BLEUUID.h>
#include <string>
#define LED_GREEN 15
#define LED_RED 13
#define LED_ORANGE_UP 12
#define LED_ORANGE_DOWN 14
// Update these with values suitable for your network.
#define mqtt_port 1883
#define MQTT_USER "d"
#define MQTT_PASSWORD "d"
#define MQTT_SERIAL_PUBLISH_CH "master_beacon"
#define MQTT_SERIAL_RECEIVER_CH "master_beaconn"
// #define DEBUG
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
#define TIME_TO_SLEEP 2        /* Time ESP32 will go to sleep (in seconds) */

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    int rssi = advertisedDevice.getRSSI();
    const char *macAdress = advertisedDevice.getAddress().toString().c_str();
    //c4:64:e3:f9:35:b3 black
    //e6:13:a7:0b:4f:b2 white
    // Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    // Serial.printf("Adress: %s // RSSI: %d \n\n", macAdress, advertisedDevice.getRSSI());
    Serial.printf("Name -> %s\n", advertisedDevice.toString().c_str());

    if (advertisedDevice.haveManufacturerData())
    {
      Serial.println("yas");
      const char *data = advertisedDevice.getManufacturerData().c_str();
      Serial.println(advertisedDevice.getManufacturerData().length());
      for (int i = 0; i < 10; i++)
      {
        Serial.write(data[i]);
      }
      // Serial.println(advertisedDevice.getManufacturerData().c_str());
    }
    char *knownAdress = "71:a4:50:f9:8e:1b";

    int val = 0;

    for (int i = 0; i < 10; i++)
      if (knownAdress[i] == macAdress[i])
        val++;

    if (val > 8)
    {
      match = true;
      timerMatch = millis();
      digitalWrite(LED_ORANGE_UP, HIGH);
      digitalWrite(LED_ORANGE_DOWN, HIGH);
    }
  }
};

void setup()
{
  Serial.begin(115200);
  Serial.println(millis());
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  DPRINT("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  digitalWrite(14, HIGH);
  digitalWrite(15, HIGH);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
  timerScan = millis();
  schedule = millis();

  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  DPRINT("Devices found: ");
  DPRINT(foundDevices.getCount());
  // pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  DPRINT("TO SLEEP ");
  Serial.println(millis());
  digitalWrite(LED_RED, LOW);
  esp_deep_sleep_start();
}

void loop()
{
}
