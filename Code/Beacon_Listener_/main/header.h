#define DEBUGGING
#ifdef DEBUGGING
#define LOGLN(x) Serial.println(x)
#define LOG(x) Serial.print(x)
#else
#define LOG(x)
#define LOGLN(x) 
#endif


static uint8_t cmd_read_date[] = {0x02, 0xfe, 'R', 'E', 'A', 'D', ' ', 'D', 'A', 'T', 'E', 0x23, 0x03, 0, 0};
static uint8_t cmd_read_time[] = {0x02, 0xfe, 'R', 'E', 'A', 'D', ' ', 'T', 'I', 'M', 'E', 0x23, 0x03, 0, 0};
static uint8_t cmd_read_flag[] = {0x02, 0xfe, 'R', 'E', 'A', 'D', ' ', 'F', 'L', 'A', 'G', 'S', 0x23, 0x03, 0, 0};
static uint8_t cmd_write_flag[] = {0x02, 0xfe, 'W', 'R', 'I', 'T', 'E', ' ', 'F', 'L', 'A', 'G', 'S', 0x23, 0x03, 0, 0};
static uint8_t cmd_ok[] = {0x02, 0xfe, 'S', 'E', 'T', ' ', 'O', 'A', 'S', 'I', 'S', 0x23, ' ', ' ', 0x23, 'O', 'K', 0x23, 0, 0};
static uint8_t cmd_read_line[] = {0x02, 0xfe, 'R', 'E', 'A', 'D', ' ', 'L', 'I', 'N', 'E', 0x23, ' ', ' ', ' ', 0x23, 0x03, 0, 0};
static uint8_t cmd_read_page[] = {0x02, 0xfe, 'R', 'E', 'A', 'D', ' ', 'P', 'A', 'G', 'E', 0x23, ' ', ' ', ' ', 0x23, 0x03, 0, 0};
static uint8_t cmd_read_book[] = {0x02, 0xfe, 'R', 'E', 'A', 'D', ' ', 'B', 'O', 'O', 'K', 0x23, ' ', ' ', ' ', 0x23, 0x03, 0, 0};
static uint8_t cmd_nok[] = {0x02, 0xfe, 'S', 'E', 'T', ' ', 'O', 'A', 'S', 'I', 'S', 0x23, ' ', ' ', 0x23, 'N', 'O', 'K', 0x23, 0, 0};
static uint8_t cmd_com_error[] = {0x02, 0xfe, 'S', 'E', 'T', ' ', 'O', 'A', 'S', 'I', 'S', 0x23, ' ', ' ', 0x23, 'C', 'O', 'M', 0x23, 0, 0};
static uint8_t cmd_stop_manprg[] = {0x02, 0xfe, 'S', 'T', 'O', 'P', ' ', 'M', 'A', 'N', 'P', 'R', 'G', 0x23, ' ', 0x23, 0x03, 0, 0};
static uint8_t cmd_start_manprg[] = {0x02, 0xfe, 'S', 'T', 'A', 'R', 'T', ' ', 'M', 'A', 'N', 'P', 'R', 'G', 0x23, ' ', 0x23, 0x03, 0, 0};
static uint8_t cmd_write_data[] = {0x02, 0xfe, 'W', 'R', 'I', 'T', 'E', ' ', 'D', 'A', 'T', 'A', 0x23, ' ', ' ', ' ', 0x23, ' ', ' ', 0x23, 0x03, 0, 0};
static uint8_t cmd_start_manvalv[] = {0x02, 0xfe, 'S', 'T', 'A', 'R', 'T', ' ', 'M', 'A', 'N', 'V', 'A', 'L', 'V', 0x23, ' ', ' ', 0x23, ' ', ' ', ' ', ' ', 0x23, 0x03, 0, 0};
static uint8_t cmd_stop_manvalv[] = {0x02, 0xfe, 'S', 'T', 'O', 'P', ' ', 'M', 'A', 'N', 'V', 'A', 'L', 'V', 0x23, ' ', ' ', 0x23, 0x03, 0, 0};
static uint8_t cmd_set_time[] = {0x02, 0xfe, 'S', 'E', 'T', ' ', 'T', 'I', 'M', 'E', 0x23, '0', '9', '0', '5', '0', '0', 0x23, '0', '3', 0x23, 0x03, 0, 0};
static uint8_t reset_unit[] = {0x02, 0xfe, 'R', 'E', 'S', 'E', 'T', ' ', 'U', 'N', 'I', 'T', 0x23, 0x03, 0, 0};

// const char *ssid = "Vyrsa";
// const char *password = "Pass3232A1";
// const char *ssid = "RUBEN14";
// const char *password = "9472302440";
const char *mqtt_server = "broker.hivemq.com";
// const char *mqtt_server = "mqtt.vyr-hydro-plus.com";

#define MQTT_USER "hydroplus"
#define MQTT_PASSWORD "hGFrTxCvDmBb"
#define MQTT_PORT 1883

// #define SUSCRIBE_MANVALVE "abcab9a1-8b22-40b0-b4bf-fb8e182f7508/manvalve/app"
#define UUID "abcab9a1-8b22-40b0-b4bf-fb8e182f7508"
// #define UUID "VYR_OASIS_A2"

#define UUID_LEN 37
#define LED 2
#define BUTTON_RESET 4

typedef struct
{
    char uuid_[UUID_LEN];
    bool led_off;
} Entity;

typedef struct
{
  uint8_t interval;
  uint8_t startDay;
  uint8_t wateringDay;
  uint16_t waterPercent;
  uint8_t start[6][2];
  uint16_t irrigTime[128];
} program;

typedef struct //If I recevived a stop command in the web I have to know what to stop
{
  bool valves[14];
  bool programs[6];
} active_to_stop;

typedef struct
{
  bool manual_web;
  bool active[10];
  uint8_t number_timers;
  uint8_t valve_number[10];
  uint32_t millix[10];
  uint32_t times[10];
} stopManualWeb;

Entity sys;
program prog[6];
active_to_stop active;
stopManualWeb stop_man_web; //MAX 10 valves open at the same time
