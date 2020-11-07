#define DEBUGGING
#ifdef DEBUGGING
#define LOGLN(x) Serial.println(x)
#define LOG(x) Serial.print(x)
#else
#define LOG(x)
#define LOGLN(x) 
#endif

const char *mqtt_server = "broker.mqttdashboard.com";

#define MQTT_USER "asd"
#define MQTT_PASSWORD "fds"
#define MQTT_PORT 1883

#define UUID "abcab9a1-8b22-40b0-b4bf-fb8e182f7508"

#define UUID_LEN 37
#define LED 2
#define BUTTON_RESET 4

