# ESP32-RTOS-data-log
Application to display charted sensor data and periodically send to Firebase for storage

ESP32 comes with 2 Xtensa 32-bit LX6 microprocessors: core 0 and core 1. So, it is dual core.
This is built in PlatformIO using the ESP32 Arduino Framework
 Protocol core (CPU0) in ESP32 is used to run various wifi protocols stacks. application core (CPU1) is used to run application programs separate from the networking layer. If you have a critical application using Bluetooth and WiFi, it is recommended to use CPU1 or application core only for user applications.

Using HC-SR04 to track what enters fov
trigPin is pin 5
echoPin is pin 18

Dependencies can be found in platformio.ini file
- HTTP Web Server 
- espasync webserver
- arduinoJson
wifi credentials can be found in wifi_credentials.h

others
- install node and npm
- install firebase tools

freeRTOS manages getting sensor readings, checking wifi connection, wifi client to firebase, webserver to connect locally

We use SPIFFS to upload file system(HTML, CSS) under data folder
- under platform, first build file system image, then upload filesystem upload
- different from standard serial upload

// initialise firebase
// put html scripts
// add styles
