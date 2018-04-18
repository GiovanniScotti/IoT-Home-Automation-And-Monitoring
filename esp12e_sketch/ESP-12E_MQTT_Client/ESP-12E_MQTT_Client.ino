// ***** IMPORT LIBRARIES *****
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// ***** Enable debug over serial communication
#define DEBUG

// Access point settings
#define WLAN_SSID   ""
#define WLAN_PASS   ""

// MQTT broker settings
#define HOST        "192.168.1.50"
#define PORT        1883
#define USERNAME    "admin"
#define PASSWORD    "hamrpi"

// Seconds to sleep
#define SLEEP_TIME  5

// Number of attempts to get connection before entering sleep mode
#define CONN_ATTEMPTS 200
#define MQTT_ATTEMPTS 10

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USERNAME, PASSWORD);
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, "espnode1/temperature");


// TODO: add new publications

void MQTT_connect();

void setup() {
  // Enable on-board ESP8266 wifi module to connect
  // to the Access Point specified by WLAN settings
  WiFi.mode(WIFI_STA);

  #ifdef DEBUG
    Serial.begin(115200);
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);
  #endif

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  int i;
  for (i = 0; i < CONN_ATTEMPTS; i++) {
    if(WiFi.status() == WL_CONNECTED) break;
    delay(100);

    #ifdef DEBUG
      Serial.print(".");
    #endif
  }
  
  if(i == CONN_ATTEMPTS) {
    #ifdef DEBUG
      Serial.println("TIMOUT!");
    #endif
    
    // TODO: and enter sleep mode
    
  }
  
  #ifdef DEBUG
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  #endif

  MQTT_connect();
  delay(100);
  
  temperature.publish("20.0");

  #ifdef DEBUG
    Serial.println("deep sleep");
  #endif
  
  delay(3000);
}

void deepSleep()
{
  //https://github.com/esp8266/Arduino/issues/644
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);
  ESP.deepSleep(SLEEP_TIME * 1000000/*, WAKE_RF_DISABLED*/);
}

void loop() 
{
  temperature.publish("20.0");

  // ***** DEBUG
  Serial.println("deep sleep");
  delay(3000);
  
}

void MQTT_connect() {
  int8_t err_code;
  
  if(mqtt.connected()) {
    return;
  }

  #ifdef DEBUG
    Serial.print("Connecting to MQTT... ");
  #endif
  
  uint8_t retries = MQTT_ATTEMPTS;
  while ((err_code = mqtt.connect()) != 0) { // Connect returns 0 on success
    #ifdef DEBUG
      Serial.println(mqtt.connectErrorString(err_code));
      Serial.println("Retrying MQTT connection in 1 second...");
    #endif
    
    mqtt.disconnect();
    delay(1000);
    retries--;
    if(retries == 0)
      deepSleep();
  }

  #ifdef DEBUG
    Serial.println("MQTT Connected!");
  #endif
}
