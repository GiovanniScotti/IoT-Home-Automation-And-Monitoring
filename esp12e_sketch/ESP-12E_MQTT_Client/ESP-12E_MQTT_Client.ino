// ***** IMPORT LIBRARIES *****
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// ***** Enable debug over serial communication
#define DEBUG

// Access point settings
#define WLAN_SSID   "ASUS-Network"
#define WLAN_PASS   ""

// MQTT broker settings
#define HOST        "192.168.1.50"
#define PORT        1883
#define USERNAME    "admin"
#define PASSWORD    "hamrpi"

// Seconds to sleep
#define SLEEP_TIME  10

// Number of attempts to get connection before entering sleep mode
#define CONN_ATTEMPTS 200
#define MQTT_ATTEMPTS 10

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USERNAME, PASSWORD);
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, "espnode1/temperature");

// TODO: add new publications


void MQTT_connect();

// ************************************************
// ***              SETUP FUNCTION              ***
// ************************************************

void setup() {
  WiFi.forceSleepWake();
  delay(1);
  // Enable on-board ESP8266 wifi module to connect
  // to the Access Point specified by WLAN settings
  WiFi.mode(WIFI_STA);
  delay(1000);

  #ifdef DEBUG
    Serial.begin(9600);
    delay(100);                                 // Small delay to set up serial connection
    while(Serial.available()>0) Serial.read();  // Flush the buffer
    Serial.println("");
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
      Serial.println("Time is out. Entering deep sleep state...");
    #endif

    enterDeepSleep();
  }
  
  #ifdef DEBUG
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  #endif

  MQTT_connect();
  delay(100);

  #ifdef DEBUG
    Serial.println("Publishing temperature...");
  #endif

  // Publish temperature value
  temperature.publish("20.0");

  delay(2000);

  #ifdef DEBUG
    Serial.println("Publish operation completed.");
    Serial.println("Entering deep sleep state...");
  #endif

  enterDeepSleep();
}

void enterDeepSleep()
{
  WiFi.mode(WIFI_OFF);      // Turn wifi off
  WiFi.forceSleepBegin();
  delay(1);
  ESP.deepSleep(SLEEP_TIME * 1000000/*, WAKE_RF_DISABLED*/);
}

void loop() {
  // loop function is not required since deep sleep is enabled.
  // Every time the ESP-12E board restarts, the setup function is executed
  // and deep sleep state is entered.
}

// ***** Connect to MQTT service provided by the MQTT broker *****
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
      enterDeepSleep();
  }

  #ifdef DEBUG
    Serial.println("MQTT connected!");
  #endif
}
