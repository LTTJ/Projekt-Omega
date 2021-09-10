#include "secrets.h"
#include "Definitions.h"
// OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_WEMOS.h>

// MQTT
#include <ArduinoMqttClient.h>

// WIFI
#include <ESP8266WiFi.h>  //https://github.com/esp8266/Arduino
#include <WiFiManager.h>

// CLOCK
#include "Clock.h"
#include <WiFiUdp.h>
#include <TimeLib.h>

// DHT
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "Menu.h"

// https://forum.arduino.cc/t/single-line-define-to-disable-code/636044/2
#define DEBUG true
#define DEBUG_SERIAL \
  if (DEBUG) Serial

#define DEBUG_DHT false
#define DEBUG_DHT_SERIAL \
  if (DEBUG_DHT) Serial

#define DEBUG_MQTT false
#define DEBUG_MQTT_SERIAL \
  if (DEBUG_MQTT) Serial


Adafruit_SSD1306_WEMOS display1(OLED_RESET);
Adafruit_SSD1306_WEMOS display2(OLED_RESET);

Menu menu;


DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
uint32_t lastDHTCheck;


WiFiClient client;
WiFiUDP Udp;
MqttClient mqttClient(client);

Clock c;

// volatile bool switchFlag = false;

ICACHE_RAM_ATTR void SWITCHHandler(void) {
  // switchFlag = true;
}


// volatile int count = 0;
volatile int lastCLK = 0;

volatile bool countChanged = false;


// data
time_t prevDisplay = 0;  // when the digital clock was displayed

float outdoorTempPrev = 0;
float outdoorTemp = 0;
float outdoorHumidPrev = 0;
float outdoorHumid = 0;

float indoorTemp = 0;
float indoorTempPrev = 0;
float indoorHumid = 0;
float indoorHumidPrev = 0;

//The interrupt handlers
ICACHE_RAM_ATTR void ClockChanged(void) {
  int clkValue = digitalRead(CLK);  //Read the CLK pin level
  int dtValue = digitalRead(DT);    //Read the DT pin level
  if (lastCLK != clkValue) {
    lastCLK = clkValue;
    //  count += (clkValue != dtValue ? 1 : -1);  //CLK and inconsistent DT + 1, otherwise - 1
    if (clkValue != dtValue) {
      menu.increment();
    } else {
      menu.decrement();
    }
  }
  countChanged = true;
}
volatile bool modeChanged = false;
ICACHE_RAM_ATTR void SWChanged(void) {
  uint8_t oldMode = menu.getSelectedMode();
  menu.select();
  modeChanged = (oldMode != menu.getSelectedMode());
}


void setup() {
#if DEBUG_DHT == true || DEBUG == true || DEBUG_MQTT == true
  Serial.begin(115200);
#endif

  pinMode(SWITCHPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SWITCHPIN), SWITCHHandler, CHANGE);

  pinMode(SW, INPUT_PULLUP);
  //digitalWrite(SW, HIGH);
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT);
  attachInterrupt(digitalPinToInterrupt(CLK), ClockChanged, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW), SWChanged, FALLING);


  display1.begin(SSD1306_SWITCHCAPVCC, address1);
  display2.begin(SSD1306_SWITCHCAPVCC, address2);


  display1.setTextColor(WHITE);
  display1.clearDisplay();
  display2.setTextColor(WHITE);
  display2.clearDisplay();

  // Temp and Humidity Sensor init
  dht.begin();
  DEBUG_SERIAL.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  DEBUG_DHT_SERIAL.println(F("------------------------------------"));
  DEBUG_DHT_SERIAL.println(F("Temperature Sensor"));
  DEBUG_DHT_SERIAL.print(F("Sensor Type: "));
  DEBUG_DHT_SERIAL.println(sensor.name);
  DEBUG_DHT_SERIAL.print(F("Driver Ver:  "));
  DEBUG_DHT_SERIAL.println(sensor.version);
  DEBUG_DHT_SERIAL.print(F("Unique ID:   "));
  DEBUG_DHT_SERIAL.println(sensor.sensor_id);
  DEBUG_DHT_SERIAL.print(F("Max Value:   "));
  DEBUG_DHT_SERIAL.print(sensor.max_value);
  DEBUG_DHT_SERIAL.println(F("°C"));
  DEBUG_DHT_SERIAL.print(F("Min Value:   "));
  DEBUG_DHT_SERIAL.print(sensor.min_value);
  DEBUG_DHT_SERIAL.println(F("°C"));
  DEBUG_DHT_SERIAL.print(F("Resolution:  "));
  DEBUG_DHT_SERIAL.print(sensor.resolution);
  DEBUG_DHT_SERIAL.println(F("°C"));
  DEBUG_DHT_SERIAL.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  DEBUG_DHT_SERIAL.println(F("Humidity Sensor"));
  DEBUG_DHT_SERIAL.print(F("Sensor Type: "));
  DEBUG_DHT_SERIAL.println(sensor.name);
  DEBUG_DHT_SERIAL.print(F("Driver Ver:  "));
  DEBUG_DHT_SERIAL.println(sensor.version);
  DEBUG_DHT_SERIAL.print(F("Unique ID:   "));
  DEBUG_DHT_SERIAL.println(sensor.sensor_id);
  DEBUG_DHT_SERIAL.print(F("Max Value:   "));
  DEBUG_DHT_SERIAL.print(sensor.max_value);
  DEBUG_DHT_SERIAL.println(F("%"));
  DEBUG_DHT_SERIAL.print(F("Min Value:   "));
  DEBUG_DHT_SERIAL.print(sensor.min_value);
  DEBUG_DHT_SERIAL.println(F("%"));
  DEBUG_DHT_SERIAL.print(F("Resolution:  "));
  DEBUG_DHT_SERIAL.print(sensor.resolution);
  DEBUG_DHT_SERIAL.println(F("%"));
  DEBUG_DHT_SERIAL.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;

  WiFiManager wifiManager;
  wifiManager.setWiFiAutoReconnect(true);
  DEBUG_SERIAL.println("starting connection");
  display1.clearDisplay();
  display1.println("connecting...");
  display1.display();
  wifiManager.autoConnect("IndoorHouse");
  DEBUG_SERIAL.println("connected...yeey :)");
  display1.clearDisplay();
  display1.println("connected!");
  display1.display();

  mqttClient.setId("IndoorHouse");
  mqttClient.setUsernamePassword(username, password);

  // String willPayload = "oh no!";
  // bool willRetain = true;
  // int willQos = 1;

  // mqttClient.beginWill(willTopic, willPayload.length(), willRetain, willQos);
  // mqttClient.print(willPayload);
  // mqttClient.endWill();


  if (!mqttClient.connect(broker, port)) {
    DEBUG_MQTT_SERIAL.print("MQTT connection failed! Error code = ");
    DEBUG_MQTT_SERIAL.println(mqttClient.connectError());
    while (1)
      ;
  }
  mqttClient.subscribe(OUTDOOR_TEMP_TOPIC);
  mqttClient.subscribe(OUTDOOR_HUMID_TOPIC);



  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
}

void loop() {

  handleMQTT();

  //if(menu.hasChanged()){
  menu.show(&display1);
  // }

  switch (menu.getSelectedMode()) {
    case INDOOR_DATA_MODE:
      // get new data from the sensor periodically
      getDHTData();

      // display the indoor values
      if (indoorTempPrev != indoorTemp || indoorHumidPrev != indoorHumid || modeChanged) {
        display2.clearDisplay();
        display2.setCursor(0, 5);
        display2.print("Indoor");
        display2.setCursor(0, 15);
        display2.print(F("Tmp: "));
        display2.print(indoorTemp);
        // display2.println(F("°C"));

        display2.setCursor(0, 25);
        display2.print(F("Hum: "));
        display2.print(indoorHumid);
        display2.display();
        // display2.println(F("%"));
        indoorHumidPrev = indoorHumid;
        indoorTempPrev = indoorTemp;

        modeChanged = false;
      }
      break;
    case OUTDOOR_DATA_MODE:
      if (outdoorTempPrev != outdoorTemp || outdoorHumidPrev != outdoorHumid || modeChanged) {
        display2.clearDisplay();
        display2.setCursor(0, 5);
        display2.print("Outdoor");
        display2.setCursor(0, 15);
        display2.print(F("Tmp: "));
        display2.print(outdoorTemp);
        // display2.println(F("°C"));

        display2.setCursor(0, 25);

        display2.print(F("Hum: "));
        display2.print(outdoorHumid);
        //  display2.println(F("%"));

        display2.display();
        outdoorTempPrev = outdoorTemp;
        outdoorHumidPrev = outdoorHumid;
        modeChanged = false;
      }
      break;
    case DIGITAL_CLOCK:
      if (timeStatus() != timeNotSet) {
        if (now() != prevDisplay || modeChanged) {  //update the display only if time has changed
          prevDisplay = now();
          display2.clearDisplay();
          c.displayDigital(&display2, hour(), minute(), second());
          display2.display();
          modeChanged = false;
        }
      }
      break;
    case ANALOG_CLOCK:
    default:
      if (timeStatus() != timeNotSet) {
        if (now() != prevDisplay || modeChanged) {  //update the display only if time has changed
          prevDisplay = now();
          display2.clearDisplay();
          c.displayAnalog(&display2, hour(), minute(), second());
          display2.display();
          modeChanged = false;
        }
      }
  }



  if (menu.hasChanged()) {
    menu.show(&display1);
  }
}

void handleMQTT() {
  int messageSize = mqttClient.parseMessage();
  if (messageSize) {
    // we received a message, print out the topic and contents
    String topic = mqttClient.messageTopic();
    DEBUG_MQTT_SERIAL.print("Received a message with topic '");
    DEBUG_MQTT_SERIAL.print(topic);
    DEBUG_MQTT_SERIAL.print("', length ");
    DEBUG_MQTT_SERIAL.print(messageSize);
    DEBUG_MQTT_SERIAL.println(" bytes:");

    // use the Stream interface to print the contents
    String value = "";
    while (mqttClient.available()) {
      value += (char)mqttClient.read();
    }
    DEBUG_MQTT_SERIAL.println(value);
    DEBUG_MQTT_SERIAL.println();

    if (topic.equals(OUTDOOR_TEMP_TOPIC)) {
      outdoorTempPrev = outdoorTemp;
      outdoorTemp = value.toFloat();
    } else if (topic.equals(OUTDOOR_HUMID_TOPIC)) {
      outdoorHumidPrev = outdoorHumid;
      outdoorHumid = value.toFloat();
    }
  }
}

void getDHTData() {
  if (millis() - delayMS > lastDHTCheck) {
    indoorTempPrev = indoorTemp;
    sensors_event_t event;
    dht.temperature().getEvent(&event);

    if (isnan(event.temperature)) {
      DEBUG_DHT_SERIAL.println(F("Error reading temperature!"));
    } else {
      DEBUG_DHT_SERIAL.print(F("Temperature: "));
      DEBUG_DHT_SERIAL.print(event.temperature);
      DEBUG_DHT_SERIAL.println(F("°C"));
      indoorTemp = event.temperature;
    }

    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      DEBUG_DHT_SERIAL.println(F("Error reading humidity!"));
    } else {
      DEBUG_DHT_SERIAL.print(F("Humidity: "));
      DEBUG_DHT_SERIAL.print(event.relative_humidity);
      DEBUG_DHT_SERIAL.println(F("%"));
      indoorHumid = event.relative_humidity;
    }
    lastDHTCheck = millis();
  }
}


// NTP STUFF
time_t getNtpTime() {
  IPAddress ntpServerIP;  // NTP server's ip address

  while (Udp.parsePacket() > 0)
    ;  // discard any previously received packets
  DEBUG_SERIAL.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  DEBUG_SERIAL.print(ntpServerName);
  DEBUG_SERIAL.print(": ");
  DEBUG_SERIAL.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      DEBUG_SERIAL.println("Receive NTP Response");
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
  DEBUG_SERIAL.println("No NTP Response :-(");
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
