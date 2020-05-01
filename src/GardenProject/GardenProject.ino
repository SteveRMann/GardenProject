#define sketchName "gardenProject.ino, V1.4.1"
/*

   IDE:
     Board: Lolin(Wemos) D1 R2 & Mini

   History:
   --Version 1 --
     5/20/2019 - Clone of ds18b20.ino
     5/20/2019 - Removed OLED code.
     5/21/2019 - Moved all code from loop) to init()
     5/21/2019 - Moved read ds to a function.
     5/22/2019 - mqtt publish is working.
     5/24/2019 - Removed sketchPath
     5/28/2019 - Added LED on D2
     5/29/2019 - Bumped sleep to one minute
     5/31/2019 - Attempts at adding LWT to tell Home Assistant if the node goes offline
     6/03/2019 - Removed LWT temporarily.
     6/16/2019 - Renamed to "gardenProject"
     6/17/2019 - Made a combined sensors topic
   --Version 1.1 --
     6/21/2019 - Added code to make WiFi.begin work with Version 2.5.2 of the esp8266 boards library.
   --Version 1.2 --
     10/08/19 - Crated new tabs: WiFi, readDS
   --Version 1.3 --
     10/10/20 - Changed MQTT nodename to garden
   --Version 1.3.1 --
     04/30/20 - Added a delay() after each publish.
              - Fixed a problem on PCB, R3 is a pullup, not a pulldown
   --Version 1.4.0 --
     04/30/20 - Added code to turn sensor transistor on/off..
   --Version 1.4.1 --
     05/01/20 - Added code to send RSSI over MQTT



   Reference:
   http://www.esp8266learning.com/wemos-mini-ds18b20-temperature-sensor-example.php#codesyntax_3

*/


#include <OneWire.h>            // Driver for DS18X Temperature Sensors.
#include <ESP8266WiFi.h>        // Connect (and reconnect) an ESP8266 to the a WiFi network.
#include <PubSubClient.h>       // connect to a MQTT broker and publish/subscribe messages in topics.

#ifndef Kaywinnet
#include "D:\River Documents\Arduino\libraries\kaywinnet.h"
#endif

// ****************************** Globals  ******************************
//#define my_ssid "Kaywinnet"
//#define my_password "806194edb8"
#define NODENAME "garden"

const char *timeTopic = NODENAME "/time";
const char *temperatureTopic = NODENAME "/temperature";
const char *moistureTopic = NODENAME "/moisture";
const char *sensorsTopic = NODENAME "/sensors";
const char *rssiTopic = NODENAME "/rssi";
const char *connectName =  NODENAME "garden1";            //Must be unique on the network
//const char *mqtt_Server = "192.168.1.124";
const int mqttPort = 1883;

const int sleepSeconds = 30;
const int pubsubDelay = 20;

long rssi;                      // Used in the WiFi tab

OneWire  ds(D4);                // Define an instance of the ds18b20 on pin D4

// Declare an object of class WiFiClient, which allows to establish a connection to a specific IP and port
// Declare an object of class PubSubClient, which receives as input of the constructor the previously defined WiFiClient.

WiFiClient GardenClient;                // The constructor MUST be unique on the network.
PubSubClient client(GardenClient);

#define DEBUG true  //set to true for debug output, false for no debug ouput
#define Serial if(DEBUG)Serial




// =================================== setup() ===================================
void setup(void)
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println(sketchName);
  Serial.println();
  Serial.print(F("temperatureTopic= "));
  Serial.println(temperatureTopic);
  Serial.print(F("moistureTopic= "));
  Serial.println(moistureTopic);
  Serial.print(F("sensorsTopic= "));
  Serial.println(sensorsTopic);
  Serial.print(F("rssiTopic= "));
  Serial.println(rssiTopic);
  Serial.println();

  pinMode(D2, OUTPUT);
  digitalWrite(D2, HIGH);          // Turn on LED- We're awake.

  Serial.println("Calling wifiConnect()");
  wifiConnect();


  // Call the PubSubClent setServer method, passing the broker address and the port.
  client.setServer(mqtt_server, mqttPort);
  mqttConnect();


  float fahrenheit = readDS();
  Serial.print(F("  Temperature = "));
  Serial.print(fahrenheit);
  Serial.println(F(" Fahrenheit"));
  Serial.println();
  client.publish(temperatureTopic, String(fahrenheit).c_str());   // Publish temperature on temperature_topic
  delay(pubsubDelay);                                             // Publish never completes without a delay

  int moistureVal = analogRead(0);                                // Read the moisture sensor
  client.publish(moistureTopic, String(moistureVal).c_str());     // Publish moisture value on moisture_topic
  delay(pubsubDelay);                                             // Publish never completes without a delay

  digitalWrite(D5, LOW);                                          // Turn off moisture reading

  String temperatureString = String(fahrenheit).c_str();
  String moistureString = String(moistureVal).c_str();

  Serial.println();
  Serial.print(F("temperatureString= "));
  Serial.println(temperatureString);
  Serial.print(F("moistureString= "));
  Serial.println(moistureString);
  Serial.println();


  // Send both sensors under one topic to sensorsTopic
  String gardenSensors = temperatureString + "," + moistureString;
  Serial.print(F("gardenSensors= \""));
  Serial.print(gardenSensors);
  Serial.println(F("\""));                                                 // Cloing quote.

  client.publish(sensorsTopic, (char*) gardenSensors.c_str());             // Publish both sensors
  delay(pubsubDelay);                                                      // Publish never completes without a delay

  String temp_str;
  char temp[50];
  temp_str = String(rssi);                              //convert the rssi to a String
  temp_str.toCharArray(temp, temp_str.length() + 1);    //packaging up the data to publish to mqtt whoa...
  bool rs = client.publish(rssiTopic, temp);            //Publish the rssi value
  delay(pubsubDelay);                                   //Publish never completes without a delay

  if (rs == true) {
    // ---------- Sleep ----------
    // Connect D0 to RST to wake up
    pinMode(D0, WAKEUP_PULLUP);
    digitalWrite(D2, LOW);                                          // Turn off LED- We're sleeping.
    Serial.print(F("Sleeping, "));
    Serial.print(sleepSeconds);
    Serial.println(F(" seconds."));
    ESP.deepSleep(sleepSeconds * 1000000);
  }
  else {
    // Try again
    Serial.println(F("*()*()*()*()*()*()*()*()*()*"));
    Serial.println(F("rs is false, publish failed."));
  }
}


// ==================================== loop() ====================================
void loop(void) {}