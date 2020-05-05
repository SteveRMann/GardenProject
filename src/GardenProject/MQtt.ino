
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

#ifdef mqttSubscribe
    char subTopic[40];
    byte numberOfSubs = (sizeof(mqttSubs) / sizeof(mqttSubs[0]));
    Serial.print(numberOfSubs);
    Serial.println(F(" subscriptions: "));
    for (int i = 0 ; i < numberOfSubs; i++) {
      strcpy(subTopic, mqttSubs[i]);
      Serial.print(F("Subscribing to "));
      Serial.println(subTopic);
      client.subscribe(subTopic);
    }

#endif

  }
}


#ifdef mqttSubscribe
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


//  if (topic == timeTopic) {
//    //Time from the Node Red is always 8 characters.
//    //For example: "12: 34: 56" or "01: 05: 67"
//  }

  if (topic == sleepTopic) {
    //Time in ms to sleep
    sleepSeconds = messageString.toInt();
    Serial.print(F("New sleepTime= "));
    Serial.print(sleepSeconds);
    Serial.println(F(" seconds"));

  }


}           //callback

#endif
