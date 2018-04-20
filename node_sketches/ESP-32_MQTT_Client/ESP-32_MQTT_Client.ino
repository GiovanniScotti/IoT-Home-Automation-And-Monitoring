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

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USERNAME, PASSWORD);
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, "espnode2/temperature");

// TODO: add new publications


// ************************************************
// ***              SETUP FUNCTION              ***
// ************************************************

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    delay(100);                                 // Small delay to set up serial connection
    while(Serial.available()>0) Serial.read();  // Flush the buffer
    Serial.println("");
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);
  #endif

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

  MQTT_connect();
  delay(500);

  float t = 20;

  // Publish temperature value
  temperature.publish(t);

  #ifdef DEBUG
    Serial.print("Temperature: ");
    Serial.println(t);
  #endif

  delay(5000);

  #ifdef DEBUG
    Serial.println("Publish operation completed.");
  #endif
}


void loop() {
  // If wifi not connected connect
  // else if mqtt not connected connect
  // else publish and subscribe

  long random_num = random(-1, 2);
  float t = 20 + random_num;

  // Publish temperature value
  temperature.publish(t);

  #ifdef DEBUG
    Serial.print("Temperature: ");
    Serial.println(t);
  #endif

  #ifdef DEBUG
    Serial.println("Publish operation completed.");
  #endif

  delay(5000);    // Send info every second not to saturate the channel
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
}

