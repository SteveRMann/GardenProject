
// ================================ wifiConnect =================================
// Connect to WiFi network so we can reach the MQTT broker and publish messages to topics.
//
// Make sure that the global rssi is at the top of the main tab.
// long rssi;


void wifiConnect() {

  Serial.print(F("Connecting to "));
  Serial.println(my_ssid);
  WiFi.persistent( false );             // Disable WiFi persistence.  The ESP8266 will not save WiFi settings in flash memory.
  WiFi.enableInsecureWEP();             // added for esp8288 v2.5.2 library.
  WiFi.begin(my_ssid, my_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println(F(""));
  Serial.print(F("WiFi connected, "));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());

  rssi = WiFi.RSSI();
  Serial.print("Signal Strength (RSSI):");
  Serial.println(rssi);
}
