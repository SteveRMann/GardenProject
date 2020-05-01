
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
  }
}
