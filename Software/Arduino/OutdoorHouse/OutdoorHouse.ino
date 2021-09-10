#include "secrets.h"

// MQTT
#include <ArduinoMqttClient.h>

// WIFI
#include <ESP8266WiFi.h>  //https://github.com/esp8266/Arduino
#include <WiFiManager.h>

// DHT
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DEBUG true
#define DEBUG_SERIAL \
  if (DEBUG) Serial

#define DEBUG_DHT false
#define DEBUG_DHT_SERIAL \
  if (DEBUG_DHT) Serial

#define DEBUG_MQTT false
#define DEBUG_MQTT_SERIAL \
  if (DEBUG_MQTT) Serial


#define DHTPIN D4      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
uint32_t lastDHTCheck;

WiFiClient client;
MqttClient mqttClient(client);

const char DHT_HUMID_TOPIC[] = "Projekt-Omega/outdoor/dht/humid";
const char DHT_TEMP_TOPIC[] = "Projekt-Omega/outdoor/dht/temp";

const uint8_t RECORD_COUNT = 10;
uint8_t tempCount = 0;
uint8_t humidCount = 0;
double tempSum = 0;
double humidSum = 0;

void setup() {
#if DEBUG_DHT == true || DEBUG == true || DEBUG_MQTT == true
  Serial.begin(115200);
#endif
  // Temp and Humidity Sensor init
  dht.begin();
  DEBUG_DHT_SERIAL.println(F("DHTxx Unified Sensor Example"));
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
  DEBUG_MQTT_SERIAL.println("starting connection");
  wifiManager.autoConnect("MQTTWifiManager");
  DEBUG_MQTT_SERIAL.println("connected...yeey :)");


  mqttClient.setId("OutdoorHouse");
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
  } else {
    DEBUG_MQTT_SERIAL.println("MQTT connected!");
  }
}

void loop() {
  mqttClient.poll();
  if (millis() - lastDHTCheck >= delayMS) {
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      DEBUG_DHT_SERIAL.println(F("Error reading temperature!"));
    } else {
      DEBUG_DHT_SERIAL.print(F("Temperature: "));
      DEBUG_DHT_SERIAL.print(event.temperature);
      DEBUG_DHT_SERIAL.println(F("°C"));

      tempSum += (event.temperature / float(RECORD_COUNT));
      tempCount++;
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);


    if (isnan(event.relative_humidity)) {
      DEBUG_DHT_SERIAL.println(F("Error reading humidity!"));

    } else {
      DEBUG_DHT_SERIAL.print(F("Humidity: "));
      DEBUG_DHT_SERIAL.print(event.relative_humidity);
      DEBUG_DHT_SERIAL.println(F("%"));
      humidSum += (event.relative_humidity / float(RECORD_COUNT));
      humidCount++;
    }


    if (tempCount >= RECORD_COUNT) {
      mqttClient.beginMessage(DHT_TEMP_TOPIC);
      mqttClient.print(tempSum);
      mqttClient.endMessage();
      DEBUG_MQTT_SERIAL.print("Sending ");
      DEBUG_MQTT_SERIAL.print(tempSum);
      DEBUG_MQTT_SERIAL.print(" to ");
      DEBUG_MQTT_SERIAL.println(DHT_TEMP_TOPIC);
      tempCount = 0;
      tempSum = 0;
    }

    if (humidCount >= RECORD_COUNT) {
      mqttClient.beginMessage(DHT_HUMID_TOPIC);
      mqttClient.print(humidSum);
      mqttClient.endMessage();
      DEBUG_MQTT_SERIAL.print("Sending ");
      DEBUG_MQTT_SERIAL.print(humidSum);
      DEBUG_MQTT_SERIAL.print(" to ");
      DEBUG_MQTT_SERIAL.println(DHT_HUMID_TOPIC);
      humidCount = 0;
      humidSum = 0;
    }

    lastDHTCheck = millis();
  }
}
