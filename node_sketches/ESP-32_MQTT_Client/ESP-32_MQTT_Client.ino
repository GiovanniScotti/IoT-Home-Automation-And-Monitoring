// **********************************************************************
// This is the sketch for the ESP-32 board.
// Supported sensors:
// - Photoresistor as light sensor
// Supported actuators:
// - Relay module used as a remote switch
// **********************************************************************

// ***** IMPORT LIBRARIES *****
#include "WiFi.h"
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

// LED ports definitions
#define LED_RED       25  // GPIO 34
#define LED_YELLOW    26  // GPIO 35
#define LED_GREEN     32  // GPIO 32

#define LIGHT_ADC     33
#define SWITCH_PIN    27

// Subscription wait time in milliseconds
#define WAIT_TIME   5000

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USERNAME, PASSWORD);
Adafruit_MQTT_Publish light = Adafruit_MQTT_Publish(&mqtt, "espnode2/light");
Adafruit_MQTT_Subscribe toggle_switch = Adafruit_MQTT_Subscribe(&mqtt, "espnode2/toggle_sw");

int sw_status = 0;  // Toggle switch status

// ************************************************
// ***              SETUP FUNCTION              ***
// ************************************************

void setup() {

  pinMode(LED_RED, OUTPUT);     // Red LED - GPIO
  pinMode(LED_YELLOW, OUTPUT);  // Yellow LED - GPIO
  pinMode(LED_GREEN, OUTPUT);   // Green LED - GPIO

  pinMode(SWITCH_PIN, OUTPUT);      // Switch (relay) control
  digitalWrite(SWITCH_PIN, LOW);
  
  #ifdef DEBUG
    Serial.begin(115200);
    delay(100);                                 // Small delay to set up serial connection
    while(Serial.available()>0) Serial.read();  // Flush the buffer
    Serial.println("");
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);
  #endif

  ledRed();

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  while(WiFi.status() != WL_CONNECTED) {
    #ifndef DEBUG
      Serial.print(".");
    #endif
    delay(500);
  }
  
  #ifdef DEBUG
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  #endif

  mqtt.subscribe(&toggle_switch);
  // Connect to MQTT broker
  MQTT_connect();
  
  delay(500);
}


void loop() {
  
  if(WiFi.status() != WL_CONNECTED) {
    ledRed();
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    
    while(WiFi.status() != WL_CONNECTED) {
      #ifndef DEBUG
        Serial.print(".");
      #endif
      delay(500);
    }
  }
  
  MQTT_connect();
  delay(100);

  // Read from 12-bit ADC
  float l = (analogRead(LIGHT_ADC) / 4095.0f) * 100.0f;

  // Publish light intensity percentage
  light.publish(l);

  #ifdef DEBUG
    Serial.print("Light: ");
    Serial.print(l);
    Serial.println("%");
  #endif

  #ifdef DEBUG
    Serial.println("Publish operation completed.");
    Serial.println("Starting listening to subscriptions...");
  #endif

  Adafruit_MQTT_Subscribe *subscription;
  sw_status;
  while ((subscription = mqtt.readSubscription(WAIT_TIME))) {
    if (subscription == &toggle_switch) {
      sw_status = atoi((char *)toggle_switch.lastread);
      toggleSwitch();
      
      #ifdef DEBUG
        Serial.print(F("Relay switch: "));
        Serial.println(sw_status);
      #endif
    }
  }
}

// ***** Connect to MQTT service provided by the MQTT broker *****
void MQTT_connect() {
  int8_t err_code;
  
  if(mqtt.connected()) {
    ledGreen();
    return;
  }

  ledYellow();

  #ifdef DEBUG
    Serial.print("Connecting to MQTT... ");
  #endif
  
  while ((err_code = mqtt.connect()) != 0) { // Connect returns 0 on success
    #ifdef DEBUG
      Serial.println(mqtt.connectErrorString(err_code));
      Serial.println("Retrying MQTT connection in 1 second...");
    #endif
    
    mqtt.disconnect();
    delay(1000);
  }
    
  #ifdef DEBUG
    Serial.println("MQTT connected!");
  #endif

  ledGreen();
}

// ***** (RELAY) SWITCH CONTROL *****
void toggleSwitch() {
  if(sw_status) {
    digitalWrite(SWITCH_PIN, HIGH);
  }
  else {
    digitalWrite(SWITCH_PIN, LOW);
  }
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

