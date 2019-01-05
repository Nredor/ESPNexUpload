/*
  This sketch is based on the default HttpClient example that comes with the Arduino ESP core.
  And was adapted for use with the ESPNexUpload library.

  You need to change wifi cred, host(server) and url (file name)

  Created on: 19 Dec 2018
  by Onno Dirkzwager
*/

#if defined ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WiFiMulti.h>
  #include <ESP8266HTTPClient.h>

  ESP8266WiFiMulti WiFiMulti;
#elif defined ESP32
  #include <WiFi.h>
  #include <WiFiMulti.h>
  #include <HTTPClient.h>

  WiFiMulti WiFiMulti;
#endif
#include <ESPNexUpload.h>

/*
  ESP8266 uses Software serial RX:5, TX:4 Wemos D1 mini RX:D1, TX:D2 
  ESP32 uses Hardware serial RX:16, TX:17
  Serial pins are defined in the ESPNexUpload.cpp file
*/

const char* ssid      = "your_wlan_ssid";
const char* password  = "your_wlan_password";
const char* host      = "your_host.com";
const char* url       = "/update.tft";
bool updated          = false;


void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  // Wait for Wifi connection to establish
  while (WiFiMulti.run() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

bool updated          = false;


void setup() {
  Serial.begin(115200);

  Serial.println("\nRunning HttpClient Example\n");

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Explicitly set the ESP to be a WiFi-client, otherwise, it by default,
  // would try to act as both a client and an access-point and could cause
  // network-issues with your other WiFi-devices on your WiFi-network.
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  // Wait for Wifi connection to establish
  while (WiFiMulti.run() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  // Report connection details
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("\n");
}

void loop() {
  delay(5000);

  if(!updated){
    Serial.print("connecting to ");
    Serial.println(host);
    
    HTTPClient http;
    
    // begin http client
    #if defined ESP8266
      if(!http.begin(host, 80, url)){
    #elif defined ESP32
      if(!http.begin(String("http://") + host + url)){
    #endif
      Serial.println("connection failed");
      return;
    }
  
    Serial.print("Requesting URL: ");
    Serial.println(url);
    
  
    // This will send the (get) request to the server
    int code          = http.GET();
    int contentLength = http.getSize();
      
    // Update the nextion display
    if(code == 200){
      Serial.println("File received. Update Nextion...");

      bool result;

      // initialize ESPNexUpload
      ESPNexUpload nextion(115200);

      // set callback: What to do / show during upload..... Optional!
      nextion.setUpdateProgressCallback([](){
        Serial.print(".");
      });
      
      // prepare upload: setup serial connection, send update command and send the expected update size
      result = nextion.prepareUpload(contentLength);
      
      if(!result){
          Serial.println("Error: " + nextion.statusMessage);
      }else{
          Serial.print(F("Start upload. File size is: "));
          Serial.print(contentLength);
          Serial.println(F(" bytes"));
          
          // Upload the received byte Stream to the nextion
          result = nextion.upload(*http.getStreamPtr());
          
          if(result){
            updated = true;
            Serial.println("\nSuccesfully updated Nextion!");
          }else{
            Serial.println("\nError updating Nextion: " + nextion.statusMessage);
          }

          // end: wait(delay) for the nextion to finish the update process, send nextion reset command and end the serial connection to the nextion
          nextion.end();
      }
      

    }else{
      // else print http error
      Serial.println(String("HTTP error: ")+ http.errorToString(code).c_str());
    }

    http.end();
    Serial.println("Closing connection\n");
  }
}
