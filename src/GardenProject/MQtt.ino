
// ================================== mqttConnect() =================================
// boolean connect(const char* id, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);

void mqttConnect() {
  while (!client.connected()) {
    Serial.print(F("MQTT connection..."));
    if (client.connect(connectName)) {
      Serial.println(F("connected"));
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F("- trying again in 5-seconds."));
      delay(5000);
    }



    char subTopic[40];
    byte numberOfSubs = (sizeof(mqttSubs) / sizeof(mqttSubs[0]));
    Serial.print(numberOfSubs);
    Serial.println(F(" subscriptions: "));
    for (int i = 0 ; i < numberOfSubs; i++) {
      strcpy(subTopic, mqttSubs[i]);
      Serial.print(F("Subscribing to "));
      Serial.println(subTopic);
      client.subscribe(subTopic);
      delay(pubsubDelay);
    }
  }
}



// **********************************  mqtt callback *************************************
// This function is executed when some device publishes a message to a topic that this ESP8266 is subscribed to.
//
void callback(String topic, byte * message, unsigned int length) {

  Serial.println();
  Serial.print(F("Message arrived on topic: "));
  Serial.println(topic);


  // Convert the character array to a string
  String messageString;
  for (int i = 0; i < length; i++) {
    messageString += (char)message[i];
  }
  messageString.trim();
  messageString.toUpperCase();          //Make the string upper-case


  Serial.print("messageString: ");
  Serial.print(messageString);
  Serial.println();


  if (topic == otaTopic) {
    otaFlag = false;
    if (messageString == "TRUE") {
      otaFlag = true;
    }
  }

  if (topic == cmdTopic) {
    char buf[24];   
    strcpy(buf, messageString.c_str());

    // Tokenize
    char *tmpbuf;
    tmpbuf = strtok(buf, ",");
    sleepSeconds = atoi(tmpbuf);                       //sleepSeconds
    Serial.print(F("New sleepTime= "));
    Serial.print(sleepSeconds);
    Serial.println(F(" seconds"));

    tmpbuf = strtok(NULL, ",");
    tCorrection = atoi(tmpbuf);                        //tCorrection
    Serial.print(F("New Temperature Correction= "));
    Serial.print(tCorrection);
    Serial.println(F(" degrees"));

    tmpbuf = strtok(NULL, ",");
    int otaVal = atoi(tmpbuf);                         //OTA Flag
    otaFlag = false;
    if (otaVal == 1) {
      otaFlag = true;
      Serial.println(F("OTA Mode"));
    }
  }
} //callback
