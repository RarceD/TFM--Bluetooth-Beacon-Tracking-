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

int scanTime = 2; //In seconds
BLEScan *pBLEScan;
bool match;
unsigned long schedule = 0;
long timerMatch = 0;
long timerScan = 0;
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    //Serial.printf("Advertised Device: %s ", advertisedDevice.toString().c_str());
    int rssi = advertisedDevice.getRSSI();
    if (advertisedDevice.haveServiceUUID())
    {
      // Serial.printf("Name -> %s", advertisedDevice.getServiceUUID().toString().c_str());
    }
    //Serial.printf(", rssi -> %i \n", rssi); advertisedDevice.getName()
    const char *macAdress = advertisedDevice.getAddress().toString().c_str();
    //c4:64:e3:f9:35:b3 black
    //e6:13:a7:0b:4f:b2 white
    char *knownAdress = "c4:64:e3:f9:35:b3";
    Serial.printf(" Adress: %s // RSSI: %d \n", macAdress, advertisedDevice.getRSSI());
    //Serial.printf("Searching for: %s \n", knownAdress);
    int val = 0;
    for (int i = 0; i < 10; i++)
      if (knownAdress[i] == macAdress[i])
        val++;
    if (val > 8)
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
      root["esp"] = "A1";
      JsonArray &beacon = root.createNestedArray("beacon");

      JsonObject &beacon_0 = beacon.createNestedObject();
      beacon_0["uuid"] = knownAdress;
      beacon_0["distance"] = rssi;

      // JsonObject &beacon_1 = beacon.createNestedObject();
      // beacon_1["uuid"] = "test";
      // beacon_1["distance"] = rssi;

      Serial.println();
      root.prettyPrintTo(Serial);
      char JSONmessageBuffer[100];
      root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      client.publish("master_beacon", JSONmessageBuffer);
    }
  }
};

void setup()
{
  Serial.begin(115200);
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
}

void loop()
{

  if (millis() - schedule > 2000)
  {
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());
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
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish("hora_de_siluetas_r", "dick");
      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("-------new message from broker-----");
  Serial.print("channel:");
  Serial.println(topic);
  Serial.print("data:");
  Serial.write(payload, length);
  Serial.println();
}

void publishSerialData(char *serialData)
{
  if (!client.connected())
  {
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}
