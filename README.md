# GardenProject
This project is for the solar-powered Wemos D1 mini with a DS18B20 temperature sensor and a Capacitive Moisture sensor.

---------------
Here is a project by Nick Gammon that uses a supercap instead of a battery.
https://www.gammon.com.au/forum/?id=12821

---------------
The publish functions expect char[] types to be passed in rather than Strings.
You need to use the String.toCharArray() function to convert your strings to the necessary type.

char charBuf[50];
stringOne.toCharArray(charBuf, 50)
myString.toCharArray(buf, len)


----------------------
For future reference:
#include <IPAddress.h>
IPAddress ipAddress(192, 168, 0, 1);  

--------

