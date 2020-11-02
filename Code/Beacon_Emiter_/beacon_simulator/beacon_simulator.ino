
/*
   Create a BLE server that will send periodic iBeacon frames.
   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create advertising data
   3. Start advertising.
   4. wait
   5. Stop advertising.
   6. deep sleep
   
*/
#include "sys/time.h"

#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEBeacon.h"
#include "esp_sleep.h"

#define GPIO_DEEP_SLEEP_DURATION 5      // sleep x seconds and then wake up
RTC_DATA_ATTR static time_t last;        // remember last boot in RTC Memory
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory

#ifdef __cplusplus
extern "C"
{
#endif

  uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
BLEAdvertising *pAdvertising;
struct timeval now;

#define BEACON_UUID "8ec76ea3-eb0a-48da-9866-a0be0000a0be"
#define LED_GREEN 15
#define LED_RED 13
#define LED_ORANGE_UP 12
#define LED_ORANGE_DOWN 14
void setBeacon()
{
  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
  // oBeacon.setMajor((bootcount & 0xFFFF0000) >> 16);
  // oBeacon.setMinor(bootcount & 0xFFFF);
  oBeacon.setMajor(0xF11FAAAA);
  oBeacon.setMinor(0x11FF);
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();

  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04

  std::string strServiceData = "";

  strServiceData += (char)26;   // Len
  strServiceData += (char)0xFF; // Type
  strServiceData += oBeacon.getData();
  oAdvertisementData.addData(strServiceData);
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
}

void setup()
{
    for (uint8_t i = 0; i<4;i++){
  pinMode(12+i, OUTPUT);
  }
  digitalWrite(LED_RED, HIGH);
  Serial.begin(115200);
  Serial.println(millis());
  // Create the BLE Device
  BLEDevice::init("");
  pAdvertising = BLEDevice::getAdvertising();
  setBeacon();
  // Start advertising
  pAdvertising->start();
  delay(50);
  pAdvertising->stop();
  Serial.printf("sleep: %i \n", millis());
  digitalWrite(LED_RED, LOW);

  esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION);
}

void loop()
{
}