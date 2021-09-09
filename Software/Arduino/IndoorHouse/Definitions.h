//-------- PINS ---------//

#define DHTPIN D4      // Digital pin connected to the DHT sensor

#define CLK D7
#define DT D3
#define SW D6

#define SWITCHPIN D5

#define BUZZER D8

//-------- DEFINITIONS ---------//

// DHT
#define DHTTYPE DHT11  // DHT 11

// OLED
#define OLED_RESET 0
const int address1 = 0x3C;
const int address2 = 0x3D;

// DISPLAY MODES
const uint8_t INDOOR_DATA_MODE = 0;
const uint8_t COUNTER_MODE = 1;
const uint8_t OUTDOOR_DATA_MODE = 2;
const uint8_t MODE_COUNT = 3;

// UDP TIME
const static char ntpServerName[] = "de.pool.ntp.org";
const static int timeZone = 2;
const static unsigned int localPort = 8888;

// MQTT
const char OUTDOOR_TEMP_TOPIC []= "outdoor/dht/temp";
const char OUTDOOR_HUMID_TOPIC[] = "outdoor/dht/humid";
