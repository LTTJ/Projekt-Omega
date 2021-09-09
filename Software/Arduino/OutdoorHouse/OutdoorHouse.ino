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


#define DHTPIN D4      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
uint32_t lastDHTCheck;

WiFiClient client;
MqttClient mqttClient(client);

const char DHT_HUMID_TOPIC[] = "outdoor/dht/humid";
const char DHT_TEMP_TOPIC[] = "outdoor/dht/temp";

const uint8_t RECORD_COUNT = 10;
uint8_t tempCount = 0;
uint8_t humidCount = 0;
double tempSum = 0;
double humidSum = 0;

void setup() {
  Serial.begin(115200);

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
  wifiManager.autoConnect("MQTTWifiManager");
  Serial.println("connected...yeey :)");


  mqttClient.setId("OutdoorHouse");
  mqttClient.setUsernamePassword(username, password);

  // String willPayload = "oh no!";
  // bool willRetain = true;
  // int willQos = 1;

  // mqttClient.beginWill(willTopic, willPayload.length(), willRetain, willQos);
  // mqttClient.print(willPayload);
  // mqttClient.endWill();


  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1)
      ;
  } else {
    Serial.println("MQTT connected!");
  }
}

void loop() {
  mqttClient.poll();
  if (millis() - lastDHTCheck >= delayMS) {
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
    } else {
      Serial.print(F("Temperature: "));
      Serial.print(event.temperature);
      Serial.println(F("°C"));

      tempSum += (event.temperature / float(RECORD_COUNT));
      tempCount++;
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);


    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));

    } else {
      Serial.print(F("Humidity: "));
      Serial.print(event.relative_humidity);
      Serial.println(F("%"));
      humidSum += (event.relative_humidity / float(RECORD_COUNT));
      humidCount++;
    }


    if(tempCount >= RECORD_COUNT){
      mqttClient.beginMessage(DHT_TEMP_TOPIC);
      mqttClient.print(tempSum);
      mqttClient.endMessage();
      tempCount = 0;
      tempSum = 0;      
    }
  
    if(humidCount >= RECORD_COUNT){
      mqttClient.beginMessage(DHT_HUMID_TOPIC);
      mqttClient.print(humidSum);
      mqttClient.endMessage();
      humidCount = 0;
      humidSum = 0;      
    }

    lastDHTCheck = millis();
  }
}
