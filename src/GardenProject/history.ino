/*
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
   --Version 1.5 --
     05/05/20 - Subscribe to sleepTime to control the sleep time..
              - changed WiFi tab to setupWiFi
   --Version 1.6 --
     05/06/20 - Added ota. Here's how to use it.
                1. From an MQTT program, set sleepTime to one minute.
                2. Send "true" to otaTopic, QOS=0, Retained=true
                3. Wait for sleepTme to expire, guaranteeing that the ESP woke up and
                   received the retained message to go into the OTA mode.
                4. From the IDE, upload the sketch.
                5. From an MQTT program, send "false" to otaTopic, QOS=0, Retained=true
                   Note: the "false" needs to be sent to otaTopic before the upload finishes
                6. Set sleepTime back to the desired sleep time.
   --Version 1.7 --
     05/07/20 - Added topic tCorrect to send a temperature correction value
   --Version 2.0 --
     05/11/20 - Changed topic sensors to status
                Added IP and RSSI to the status.
v  --Version 2.1 --
     05/11/20 - Added OTA flag to the status.

*/
