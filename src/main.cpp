#include <Arduino.h>
#include <FreeRTOS.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <AsyncJson.h>

#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

//wifi login info
#include "wifi_credentials.h"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
//firebase signUp variables
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
//sensor data variables
#define SOUND_SPEED 0.034 //define sound speed in cm/uS
const int trigPin = 5;
const int echoPin = 18;
long duration;
float distanceCm;
//checkWifiConnection variables
unsigned long previousMillis = 0;
unsigned long interval = 30000;

AsyncWebServer server(80);

// SetUp functions
void connectToWifi(){
  // might move to setup
  Serial.print("Connecting to Wifi");
  WiFi.mode(WIFI_STA); // station to connect to existing wifi
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }

  Serial.println("[WIFI] Connected: " + WiFi.localIP());
}

void webServerSetup( void * parameters) {
  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  MDNS.begin("demo-server");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(distanceCm).c_str());
  });
  server.onNotFound([](AsyncWebServerRequest *request){request->send(404);});
  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}

void firebaseSetup(){
  /* Assign the api key (required) */
  config.api_key = API_KEY;

   /* Assign the user sign in credentials */
   // auth.user.email = USER_EMAIL;
   // auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

// FreeRTOS Tasks
void collectSensorData( void * parameter) {
  for(;;) {
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculate the distance
    distanceCm = duration * SOUND_SPEED/2;
    // Prints the distance in the Serial Monitor
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);

    vTaskDelay(500); //0.5s
  }
}

// modify function more
void checkWifiConnection(void * parameters){  
  for(;;){
    unsigned long currentMillis = millis();
    // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
    if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
      Serial.print(millis());
      Serial.println("Reconnecting to WiFi...");
      WiFi.disconnect();
      WiFi.reconnect();
      previousMillis = currentMillis;
    }
  } 
}

void sendToFirebase( void * parameter) {
  for(;;) {
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
      sendDataPrevMillis = millis();
      // Write an float number on the database path sensor/data
      if (Firebase.RTDB.setFloat(&fbdo, "sensor/data", distanceCm)){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
      count++;
    }
  }
}

void setup() {
  Serial.begin(115200); // set baud rate 115200
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  connectToWifi();
  firebaseSetup();
  
  xTaskCreatePinnedToCore(
      webServerSetup, /* Function to implement the task */
      "Task1", /* Name of the task */
      1024,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      NULL,  /* Task handle. */
      0
  ); /* Core where the task should run */  

  xTaskCreatePinnedToCore(
      collectSensorData, /* Function to implement the task */
      "Task2", /* Name of the task */
      1024,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      NULL,  /* Task handle. */
      1
  ); /* Core where the task should run */

  xTaskCreatePinnedToCore(
      checkWifiConnection, /* Function to implement the task */
      "Task3", /* Name of the task */
      1024,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      NULL,  /* Task handle. */
      0
  ); /* Core where the task should run */

  xTaskCreatePinnedToCore(
      sendToFirebase, /* Function to implement the task */
      "Task4", /* Name of the task */
      1024,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      NULL,  /* Task handle. */
      0
  ); /* Core where the task should run */
}

void loop() {

}