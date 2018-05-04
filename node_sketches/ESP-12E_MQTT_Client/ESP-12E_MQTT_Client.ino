// **********************************************************************
// This is the sketch for the first ESP-12E board.
// Supported sensors:
// - DHT11 as temperature and humidity sensor
// - Photoresistor as light sensor
// **********************************************************************

// ***** IMPORT LIBRARIES *****
#include <ESP8266WiFi.h>
#include "DHT.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// ***** Enable debug over serial communication
// Decomment to enable serial debug
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
#define SLEEP_TIME  10

// Number of attempts to get connection before entering sleep mode
#define CONN_ATTEMPTS 200
#define MQTT_ATTEMPTS 10

// Type of humidity/temperature sensor
#define DHT_TYPE    DHT11   // DHT 11
#define DHT_PIN     13      // DHT sensor attached to GPIO 13

// LED ports definitions
#define LED_RED     5
#define LED_GREEN   12
#define LED_YELLOW  4

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USERNAME, PASSWORD);
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, "espnode1/temperature");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, "espnode1/humidity");
Adafruit_MQTT_Publish light = Adafruit_MQTT_Publish(&mqtt, "espnode1/light");

// Instantiate DHT sensor
DHT dht(DHT_PIN, DHT_TYPE);



// ************************************************
// ***              SETUP FUNCTION              ***
// ************************************************

void setup() {
  
  pinMode(LED_RED, OUTPUT);     // Red LED - GPIO 5
  pinMode(LED_YELLOW, OUTPUT);  // Blue LED - GPIO 4
  pinMode(LED_GREEN, OUTPUT);   // Green LED - GPIO 12

  ledRed();
  
  WiFi.forceSleepWake();
  delay(1);
  // Enable on-board ESP8266 wifi module to connect
  // to the Access Point specified by WLAN settings
  WiFi.mode(WIFI_STA);
  delay(500);                                   // Small delay to let the wifi module set up

  #ifdef DEBUG
    Serial.begin(9600);
    delay(100);                                 // Small delay to set up serial connection
    Serial.println("");
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);
  #endif

  // Read from DHT sensor
  dht.begin();
  float t = dht.readTemperature();  // As celsius
  float h = dht.readHumidity();
  float l = (analogRead(A0) / 1023.0f) * 100.0f;

  if(isnan(t)) {
    #ifdef DEBUG
      Serial.println("Failed to read temperature from DHT sensor!");
    #endif
    t = NAN;
  }

  if(isnan(h)) {
    #ifdef DEBUG
      Serial.println("Failed to read humidity from DHT sensor!");
    #endif
    h = NAN;
  }

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  int i;
  for(i = 0; i < CONN_ATTEMPTS; i++) {
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

  // Wifi connection established, MQTT disconnected
  ledYellow();

  MQTT_connect();
  delay(500);

  // Wifi connection established, MQTT connected
  ledGreen();

  #ifdef DEBUG
    Serial.println("Publishing data...");
  #endif

  // Publish temperature value
  temperature.publish(t);
  humidity.publish(h);
  light.publish(l);

  #ifdef DEBUG
    Serial.print("Temperature: ");
    Serial.println(t);
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.println("%");
    Serial.print("Light: ");
    Serial.print(l);
    Serial.println("%");
  #endif

  delay(2000);

  #ifdef DEBUG
    Serial.println("Publish operation completed.");
    Serial.println("Entering deep sleep state...");
  #endif

  enterDeepSleep();
}

void enterDeepSleep()
{
  ledOff();                 // Turn leds off
  
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
  while((err_code = mqtt.connect()) != 0) { // Connect returns 0 on success
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

// ***** LED CONTROL FUNCTIONS *****

void ledOff() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
}

void ledRed() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
}

void ledYellow() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_GREEN, LOW);
}

void ledGreen() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, HIGH);
}

