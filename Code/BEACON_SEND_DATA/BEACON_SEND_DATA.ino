#include <WiFi.h>
#include <PubSubClient.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEAddress.h>
#include <BLEUUID.h>
#include <ArduinoJson.h>

#define LED_GREEN 15
#define LED_RED 13
#define LED_ORANGE_UP 12
#define LED_ORANGE_DOWN 14
// Update these with values suitable for your network.
const char *ssid = "RUBEN14";
const char *password = "9472302440";
const char *mqtt_server = "broker.mqttdashboard.com";
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


/*
For no antenna these are the results:
  0.5 metros -40
  1 metro -50
  2 metros -60
  3 metros -68
  4 metros -77
  5 metros -85
  6 metros -89

  y = distance 
  x = abs(rssi)
  y = 0,0012x2 - 0,0395x + 0,1818

*/
WiFiClient wifiClient;
PubSubClient client(wifiClient);

int scanTime = 3; //In seconds
BLEScan *pBLEScan;
bool match;
unsigned long schedule = 0;
long timerMatch = 0;
long timerScan = 0;
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : DPRINT("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : DPRINT("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : DPRINT("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : DPRINT("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : DPRINT("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    //Serial.printf("Advertised Device: %s ", advertisedDevice.toString().c_str());
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
    //  Serial.printf(" Adress: %s // RSSI: %d \n", macAdress, advertisedDevice.getRSSI());
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
    if (val1 > 8 || val2>8)
    {
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

      // JsonObject &beacon_1 = beacon.createNestedObject();
      // beacon_1["uuid"] = "test";
      // beacon_1["distance"] = rssi;

      DPRINT();
      // root.prettyPrintTo(Serial);
      char JSONmessageBuffer[100];
      root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      client.publish("master_beacon", JSONmessageBuffer);
    }
  }
};

void setup()
{
  Serial.begin(115200);
  
  //Increment boot number and print it every reboot
  ++bootCount;
  DPRINT("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  DPRINT("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");

  Serial.setTimeout(500); // Set time out for
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
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
  pBLEScan->setWindow(99); // less or equal setInterval value
  timerScan = millis();
  schedule = millis();
  digitalWrite(LED_RED, LOW);


    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    DPRINT("Devices found: ");
    DPRINT(foundDevices.getCount());
    pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
    DPRINT("TO SLEEP ");

  esp_deep_sleep_start();

}

void loop()
{

  if (millis() - schedule > 2000)
  {
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Devices found: ");
    DPRINT(foundDevices.getCount());
    pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
    schedule = millis();
  }
  if (match && millis() - timerMatch >= 2000)
  {
    match = false;
    timerMatch = millis();
    digitalWrite(LED_ORANGE_UP, LOW);
    digitalWrite(LED_ORANGE_DOWN, LOW);
  }
  if (millis() - timerScan >= 5000)
  {
    static bool ledBlink;
    timerScan = millis();
    if (ledBlink)
    {
      digitalWrite(LED_GREEN, HIGH);
    }
    else
    {
      digitalWrite(LED_GREEN, LOW);
    }
    ledBlink = !ledBlink;
  }
  if (Serial.available() > 0)
  {
    char mun[501];
    memset(mun, 0, 501);
    Serial.readBytesUntil('\n', mun, 500);
    publishSerialData(mun);
  }
  client.loop();
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  DPRINT();
  DPRINT("Connecting to ");
  DPRINT(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    DPRINT(".");
  }
  randomSeed(micros());
  DPRINT("");
  DPRINT("WiFi connected");
  DPRINT("IP address: ");
  DPRINT(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    DPRINT("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      DPRINT("connected");
      //Once connected, publish an announcement...
      client.publish("hora_de_siluetas_r", "dick");
      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    }
    else
    {
      DPRINT("failed, rc=");
      DPRINT(client.state());
      DPRINT(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  DPRINT("-------new message from broker-----");
  DPRINT("channel:");
  DPRINT(topic);
  DPRINT("data:");
  Serial.write(payload, length);
  DPRINT();
}

void publishSerialData(char *serialData)
{
  if (!client.connected())
  {
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}
