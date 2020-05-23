#define sketchName "gardenProject.ino, V2.2"

/*
   IDE:
     Board: Lolin(Wemos) D1 R2 & Mini

   Reference:
   http://www.esp8266learning.com/wemos-mini-ds18b20-temperature-sensor-example.php#codesyntax_3

*/


#include <OneWire.h>            // Driver for DS18X Temperature Sensors.
#include <ESP8266WiFi.h>        // Connect (and reconnect) an ESP8266 to the a WiFi network.
#include <PubSubClient.h>       // connect to a MQTT broker and publish/subscribe messages in topics.
#include <ArduinoOTA.h>

#ifndef Kaywinnet
#include "D:\River Documents\Arduino\libraries\Kaywinnet.h"  // WiFi credentials
#endif


// ****************************** Globals  ******************************
#define NODENAME "garden"      //Must be unique on the net.

//Probe Calibration
int dry = 850;                  //Dry sensor
int wet = 390;                  //Wet sensor


#define hostPrefix NODENAME     // For setupWiFi()
char macBuffer[24];             // Holds the last three digits of the MAC, in hex.

const char *statusTopic = NODENAME "/status";             //Sends the temperature, moisture, IP and RSSI in one payload.
const char *otaTopic = NODENAME "/ota";                   //Sets a flag to ut the ESP into OTA mode.
const char *cmdTopic = NODENAME "/cmd";                   //Sends a command string: sleepTime, correction, ota (0 or 1)
const char *connectName =  NODENAME "garden";             //Must be unique on the network
const int mqttPort = 1883;

bool otaFlag = false;
int sleepSeconds = 120;
const int pubsubDelay = 20;           //Time between publishes
long rssi;                            //Used in the WiFi tab
float tCorrection = 0.0;              //Temperature correction.
char rssi_string[50];                 //RSSI in char array


OneWire  ds(D4);                      //Create an instance of the ds18b20 on pin D4


static const char *mqttSubs[] = {
  cmdTopic
};



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
  Serial.print(F("statusTopic= "));
  Serial.println(statusTopic);
  Serial.println();

  otaFlag = false;
  pinMode(D2, OUTPUT);
  digitalWrite(D2, HIGH);          // Turn on LED- We're awake.

  setup_wifi();
  start_OTA();


  // Call the PubSubClent setServer method, passing the broker address and the port.
  client.setServer(mqtt_server, mqttPort);
  mqttConnect();
  client.setCallback(callback);


  //Ensure we've sent & received everything before sleeping
  //This adds 1-second to the wake time.
  for (int i = 0; i < 5; i++)
  {
    client.loop();                 // Normally at the top of the loop.
    delay(100);
  }


  String rssiTemp;                                              //RSSI in String
  rssiTemp = String(rssi);                                      //convert the rssi to a String
  rssiTemp.toCharArray(rssi_string, rssiTemp.length() + 1);     //packaging up the data to publish to mqtt whoa...

  float fahrenheit = readDS();
  Serial.print(F("temperatureRaw= "));
  Serial.println(fahrenheit);
  Serial.print(F("tCorrection= "));
  Serial.print(tCorrection);
  fahrenheit = fahrenheit + tCorrection;

  int moistureVal = analogRead(0);                              //Read the moisture sensor
  Serial.print(F("\n\nraw= "));
  Serial.println(moistureVal);
  int moistureValPct = map(moistureVal, wet, dry, 100, 0);
  Serial.print(F("mapped= "));
  Serial.print(moistureValPct);
  Serial.println(F(" % "));


  String temperatureString = String(fahrenheit).c_str();
  String moistureRawString = String(moistureVal).c_str();
  String moisturePctString = String(moistureValPct).c_str();
  Serial.print(F("moistureRawString = "));
  Serial.println(moistureRawString);
  Serial.print(F("moisturePctString = "));
  Serial.println(moisturePctString);
  Serial.print(F("rssi_string = "));
  Serial.println(rssi_string);
  Serial.print(F("Temperature = "));
  Serial.print(fahrenheit);
  Serial.println(F(" Fahrenheit"));
  Serial.println();
  Serial.print(sleepSeconds);
  Serial.println(F(" seconds"));
  Serial.print(F("IP = "));
  Serial.println(WiFi.localIP());



  // Send data to statusTopic
  String status = temperatureString + ", " +
                  moisturePctString + ", " +
                  moistureRawString + ", " +
                  WiFi.localIP().toString() + ", " +
                  rssi_string + ", " +
                  String(sleepSeconds) + ", " +
                  otaFlag;

  Serial.print(F("status = \""));
  Serial.println(F("Temp, moistureRaw, moisture%, IP, rssi, sleep, OTA"));
  Serial.print(F("status= \""));
  Serial.print(status);
  Serial.println(F("\""));                                                 // Cloing quote.

  client.publish(statusTopic, (char*) status.c_str());                     // Publish all data
  delay(pubsubDelay);                                                      // Publish never completes without a delay







  if (!otaFlag) {                                       //If the otaFlag is set, then drop into loop to wait for the ota upload.
    // ---------- Sleep ----------
    // Connect D0 to RST to wake up
    pinMode(D0, WAKEUP_PULLUP);
    digitalWrite(D2, LOW);                                          // Turn off LED- We're sleeping.
    Serial.print(F("Sleeping, "));
    Serial.print(sleepSeconds);
    Serial.println(F(" seconds."));
    ESP.deepSleep(sleepSeconds * 1000000);
  }
  Serial.println(F("Dropping into loop()"));
}

// ==================================== loop() ====================================
void loop(void) {

  ArduinoOTA.handle();
  ArduinoOTA.onEnd([]() {
    Serial.println("\nUpload Ended");
    otaFlag = false;
  });


}
