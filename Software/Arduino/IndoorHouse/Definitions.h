#ifndef DEFINITIONS
#define DEFINITIONS

//-------- PINS ---------//

#define DHTPIN D4  // Digital pin connected to the DHT sensor

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
const int DISPLAY_WIDTH = 64;
const int DISPLAY_HEIGHT = 48;

// DISPLAY MODES
const uint8_t INDOOR_DATA_MODE = 0;
const uint8_t OUTDOOR_DATA_MODE = 1;
const uint8_t ANALOG_CLOCK = 2;
const uint8_t DIGITAL_CLOCK = 3;
const uint8_t MIXED_CLOCK = 4;
const uint8_t SPACEX = 5;
const uint8_t MODE_COUNT = 6;
// Auflistung der Strings, welche im Menü angezeigt werden.
// Reihenfolge muss mit den Mode-Values übereinstimmen!
const static char *MODES[MODE_COUNT] = { "Indoor", "Outdoor", "Analog", "Digital", "Mixed", "SpaceX" };

// UDP TIME
const static char ntpServerName[] = "de.pool.ntp.org";
const static int timeZone = 2;
const static unsigned int localPort = 8888;

// NTP
const static int NTP_PACKET_SIZE = 48;
static byte packetBuffer[NTP_PACKET_SIZE];

// MQTT
const char OUTDOOR_TEMP_TOPIC[] = "Projekt-Omega/outdoor/dht/temp";
const char OUTDOOR_HUMID_TOPIC[] = "Projekt-Omega/outdoor/dht/humid";
const char DISPLAY_SELECTION_TOPIC[] = "Projekt-Omega/indoor/displayMode";

const char API_SPACEX[] = "api/spaceX";
const char API_SPACEX_NAME[] = "api/spaceX/name";
const char API_SPACEX_LAUNCHDATE[] = "api/spaceX/launchDate";

#endif