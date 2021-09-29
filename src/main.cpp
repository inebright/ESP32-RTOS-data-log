#include <Arduino.h>
#include <FreeRTOS.h>
#include <WiFi.h>

#include <SPIFFS.h>

#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>

#include "wifi_credentials.h"

#define WIFI_NETWORK
#define WIFI_PASSWORD
#define WIFI_TIMEOUT_MS

AsyncWebServer server(80);

void connectToWifi(void * parameters){
  Serial.print("Connecting to Wifi");
  WiFi.mode(WIFI_STA); // station to connect to existing wifi
  
  unsigned long startAttemptTime = millis();

}

// SPIFFS.begin();

// server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

void setup() {
  Serial.begin(9600);
  xTaskCreate( 
    connectToWifi, // function name
    "connect to wifi",
    1000, // STACK SIZE increase sif a lot of variables
    NULL, // task priority
    1 , // task priority
    NULL  // task handler
  );
  xTaskCreate( 
    connectToWifi, // function name
    "connect to wifi",
    1000, // STACK SIZE increase sif a lot of variables
    NULL, // task priority
    1 , // task priority
    NULL  // task handler
  );
}

void loop() {
  // put your main code here, to run repeatedly:

}