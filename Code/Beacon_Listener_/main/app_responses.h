#include <PubSubClient.h>
#include <ArduinoJson.h>

#define MAX_JSON_SIZE 512
bool send_stop_program;

WiFiClient wifiClient;
PubSubClient clientMQTT(wifiClient);

void reconnect();

void init_mqtt();
void callback(char *topic, byte *payload, unsigned int length);

void reconnect()
{
    while (!clientMQTT.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random clientMQTT ID
        String clientId = "VTest-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (clientMQTT.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD))
        {
            Serial.println("connected");
            String topic = "beacon_master/+/app";
            clientMQTT.subscribe(topic.c_str());
            clientMQTT.publish("beacon_master", "beacon_master");
            Serial.println(topic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(clientMQTT.state());
            Serial.println(" try again in 5 seconds");
            delay(3000);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    LOGLN(length);

    if (!length)
        return;
    //Parse the huge topic
    char json[MAX_JSON_SIZE];
    DynamicJsonBuffer jsonBuffer(MAX_JSON_SIZE);
    strcpy(json, (char *)payload); //one alternative is using c fucntion to convert
    //Parse the huge topic
    JsonObject &parsed = jsonBuffer.parseObject(json);
    int identifier = parsed["id"].as<int>(); // This is for the sendding function app
    //Obtein the topic
    String sTopic = String(topic);
    LOGLN(sTopic);

    // Then I identify the topic related with
    if (sTopic.indexOf("config") != -1) //done
    {
        if (parsed["login"].success())
        {
            int beacon_number = parsed["number_beacons"]; // 3
            LOGLN(beacon_number);
            for (int i = 0; i < beacon_number; i++)
            {
                const char *login_0 = parsed["login"][i]; // "f3:34:23:54"
                LOGLN(login_0);
            }
        }
        if (parsed["scan_frecuency"].success())
        {
            int scan_frecuency = parsed["scan_frecuency"]; // 3
            LOGLN(scan_frecuency);
        }
    }
}
void init_mqtt()
{
    clientMQTT.setServer(mqtt_server, MQTT_PORT);
    clientMQTT.setCallback(callback);
    reconnect();
}