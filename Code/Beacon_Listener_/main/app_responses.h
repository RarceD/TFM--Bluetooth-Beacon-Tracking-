#include <PubSubClient.h>
#include <ArduinoJson.h>


#define MAX_JSON_SIZE 512
bool send_stop_program;

WiFiClient wifiClient;
PubSubClient client(wifiClient);


void reconnect();

void init_mqtt();
void callback(char *topic, byte *payload, unsigned int length);

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "VTest-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD))
        {
            Serial.println("connected");
            String topic = String(sys.uuid_) + "/+/app";
            client.subscribe(topic.c_str());
            Serial.println(topic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(3000);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
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
    identifier = 0;
    // Then I identify the topic related with
    if (identifier >= 1)
    {
        if (sTopic.indexOf("manvalve") != -1) //done
        {
           
        }
    }

}
void init_mqtt()
{
    client.setServer(mqtt_server, MQTT_PORT);
    client.setCallback(callback);
    reconnect();
}