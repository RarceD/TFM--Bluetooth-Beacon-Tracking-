#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEAddress.h>
#include <BLEUUID.h>

#define LED_GREEN 15
#define LED_RED 13
#define LED_ORANGE_UP 12
#define LED_ORANGE_DOWN 14

int scanTime = 2; //In seconds
BLEScan* pBLEScan;
bool match;
unsigned long schedule = 0;
long timerMatch = 0;
long timerScan = 0;
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //Serial.printf("Advertised Device: %s ", advertisedDevice.toString().c_str());
      int rssi = advertisedDevice.getRSSI();
      if (advertisedDevice.haveServiceUUID()) {
        //Serial.printf("Name -> %s", advertisedDevice.getServiceUUID().toString().c_str());
      }
      //Serial.printf(", rssi -> %i \n", rssi); advertisedDevice.getName()
      const char * macAdress = advertisedDevice.getAddress().toString().c_str();
      char * knownAdress = "c4:64:e3:f9:35:b3";
      Serial.printf("Adress: %s // RSSI: %d \n\n", macAdress, advertisedDevice.getRSSI());
      //Serial.printf("Searching for: %s \n", knownAdress);
      int val = 0;
      for (int i = 0; i < 10; i++ ) {
        if (knownAdress[i] == macAdress[i]) {
          val++;
        }
      }
      if (val > 8) {
        match = true;
        timerMatch = millis();
        digitalWrite(LED_ORANGE_UP, HIGH);
        digitalWrite(LED_ORANGE_DOWN, HIGH);
      }
    }

};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  timerScan = millis();
  schedule = millis();
  digitalWrite(LED_RED, LOW);
}

void loop() {

  if (millis() - schedule > 2000) {
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());
    pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
    schedule = millis();
  }
  if (match && millis() - timerMatch >= 2000) {
    match = false;
    timerMatch = millis();
    digitalWrite(LED_ORANGE_UP, LOW);
    digitalWrite(LED_ORANGE_DOWN, LOW);
  }
  if (millis() - timerScan >= 5000) {
    static bool ledBlink;
    timerScan = millis();
    if (ledBlink) {
      digitalWrite(LED_GREEN, HIGH);
    } else {
      digitalWrite(LED_GREEN, LOW);
    }
    ledBlink = !ledBlink;
  }
}
