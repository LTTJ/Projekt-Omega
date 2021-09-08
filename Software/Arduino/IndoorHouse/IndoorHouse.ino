#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_WEMOS.h>

#include <ESP8266WiFi.h>  //https://github.com/esp8266/Arduino
#include <WiFiManager.h>

#include "Clock.h"
#include <WiFiUdp.h>
#include <TimeLib.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define OLED_RESET 0
Adafruit_SSD1306_WEMOS display1(OLED_RESET);
Adafruit_SSD1306_WEMOS display2(OLED_RESET);

const int address1 = 0x3C;
const int address2 = 0x3D;

#define DHTPIN D4      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 11


DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
uint32_t lastDHTCheck;


WiFiClient client;
WiFiUDP Udp;

Clock c;

const static char ntpServerName[] = "de.pool.ntp.org";
const static int timeZone = 2;

const static unsigned int localPort = 8888;


volatile bool switchFlag = false;
const int SWITCHPIN = D5;

const int CLK = D7;
const int DT = D3;
const int SW = D6;

volatile int count = 0;
volatile int lastCLK = 0;
volatile bool clkFlag = false;

void setup() {
  Serial.begin(115200);

  pinMode(SWITCHPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SWITCHPIN), SWITCHHandler, CHANGE);

  pinMode(SW, INPUT);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  attachInterrupt(digitalPinToInterrupt(CLK), CLKHandler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW), SWHandler, CHANGE);


  display1.begin(SSD1306_SWITCHCAPVCC, address1);
  display2.begin(SSD1306_SWITCHCAPVCC, address2);


  display1.setTextColor(WHITE);
  display1.clearDisplay();
  display2.setTextColor(WHITE);
  display2.clearDisplay();

  // Temp and Humidity Sensor init
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("°C"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("°C"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("%"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("%"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;

  WiFiManager wifiManager;
  wifiManager.setWiFiAutoReconnect(true);
  Serial.println("starting connection");
  display1.clearDisplay();
  display1.println("connecting...");
  display1.display();
  wifiManager.autoConnect("MQTTWifiManager");
  Serial.println("connected...yeey :)");
  display1.clearDisplay();
  display1.println("connected!");
  display1.display();

  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  display2.clearDisplay();
  display2.setCursor(0, 10);
  display2.print(F("count: "));
  display2.print(count);
  display2.display();
}



/*     
  Draw two eyes

  display1.clearDisplay();
  display1.drawCircle(32, 24, 23, WHITE);
  display1.fillCircle(24, 21, 4, WHITE);

  display1.drawLine(45,2 , 60, 24, WHITE);
  display1.display();


  display2.clearDisplay();
  display2.drawCircle(32, 24, 23, WHITE);
  display2.fillCircle(40, 20, 4, WHITE);

  display1.drawLine(4,24 , 19, 2, WHITE);
  display2.display();  
 */

time_t prevDisplay = 0;  // when the digital clock was displayed


void loop() {
  if (millis() - delayMS > lastDHTCheck) {
    display2.clearDisplay();
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    display2.setCursor(0, 5);
    if (isnan(event.temperature)) {
      //Serial.println(F("Error reading temperature!"));
      display2.println(F("Tmp Err"));
    } else {
      // Serial.print(F("Temperature: "));
      // Serial.print(event.temperature);
      // Serial.println(F("°C"));
      display2.print(F("Tmp: "));
      display2.print(event.temperature);
      // display2.println(F("°C"));
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    display2.setCursor(0, 15);

    if (isnan(event.relative_humidity)) {
      //Serial.println(F("Error reading humidity!"));
      display2.println(F("Hum Err"));
    } else {
      // Serial.print(F("Humidity: "));
      // Serial.print(event.relative_humidity);
      // Serial.println(F("%"));
      display2.print(F("Hum: "));
      display2.print(event.relative_humidity);
      //  display2.println(F("%"));
    }
    display2.display();
    lastDHTCheck = millis();
  }


  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) {  //update the display only if time has changed
      prevDisplay = now();


      display1.clearDisplay();
      c.displayAnalog(&display1, hour(), minute(), second());
      display1.display();
    }
  }

  if (switchFlag) {

    display1.clearDisplay();
    c.displayAnalog(&display1, hour(), minute(), second());

    if (digitalRead(SWITCHPIN)) {  // switch inverts display
      display1.invertDisplay(1);
    } else {
      display1.invertDisplay(0);
    }
    display1.display();

    switchFlag = false;
  }
  /*

  if (clkFlag) {
    display2.clearDisplay();
    display2.setCursor(0, 10);
    display2.print(F("count: "));
    display2.print(count);
    display2.display();
    clkFlag = false;
  }
  */
}



ICACHE_RAM_ATTR void SWITCHHandler(void) {
  switchFlag = true;
}

ICACHE_RAM_ATTR void CLKHandler(void) {
  int clkValue = digitalRead(CLK);  //Read the CLK pin level
  int dtValue = digitalRead(DT);    //Read the DT pin level
  if (lastCLK != clkValue) {
    lastCLK = clkValue;
    count += (clkValue != dtValue ? 1 : -1);  //CLK and inconsistent DT + 1, otherwise - 1
  }
  Serial.println("Encoder");
  Serial.print("count: ");
  Serial.println(count);
  clkFlag = true;
}


ICACHE_RAM_ATTR void SWHandler(void) {
  if (count != 0) {
    count = 0;
  }
  Serial.println("EncoderBtn");
}

// NTP STUFF


const static int NTP_PACKET_SIZE = 48;
static byte packetBuffer[NTP_PACKET_SIZE];

time_t getNtpTime() {
  IPAddress ntpServerIP;  // NTP server's ip address

  while (Udp.parsePacket() > 0)
    ;  // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0;  // return 0 if unable to get the time
}


// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;  // LI, Version, Mode
  packetBuffer[1] = 0;           // Stratum, or type of clock
  packetBuffer[2] = 6;           // Polling Interval
  packetBuffer[3] = 0xEC;        // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123);  //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
