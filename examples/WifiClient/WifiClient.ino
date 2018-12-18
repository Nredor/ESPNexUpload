/*
    This sketch is based on the default WifiClient example that comes with the Arduino ESP core.
    And was adapted for use with the ESPNexUpload library.
    
    You need to change wifi cred, host(server) and url (file name)
*/

#if defined ESP8266
  #include <ESP8266WiFi.h>
#elif defined ESP32
  #include <WiFi.h>
#endif
#include <ESPNexUpload.h>

/*
  ESP8266 uses Software serial RX:5, TX:4 Wemos D1 mini RX:D1, TX:D2 
  ESP32 uses Hardware serial RX:16, TX:17
  Serial pins are defined in the ESPNexUpload.cpp file
*/

const char* ssid      = "WireDirk";
const char* password  = "luchtschip";
const char* host      = "iotappstory.com";
const char* url       = "/ota/update.tft";
bool updated          = false;


void setup(){
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
  // would try to act as both a client and an access-point and could cause
  // network-issues with your other WiFi-devices on your WiFi-network.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for Wifi connection to establish
  while (WiFi.status() != WL_CONNECTED){
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


void loop(){
  delay(5000);

  if(!updated){
    Serial.print("connecting to ");
    Serial.println(host);
  
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)){
      Serial.println("connection failed");
      return;
    }
  
    Serial.print("Requesting URL: ");
    Serial.println(url);
  
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while(client.available() == 0){
      if(millis() - timeout > 5000){
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    // Scan reply header for succes (code 200 OK) and the content lenght
    int contentLength;
    int code;
    while(client.available()){
  
      String line = client.readStringUntil('\n');
      //Serial.println(line); // Read all the lines of the reply from server and print them to Serial
      
      if(line.startsWith("HTTP/1.1 ")){
        line.remove(0, 9);
        code = line.substring(0, 3).toInt();
  
        if(code != 200){
          line.remove(0, 4);
          Serial.println(line);
          break;
        }
  
      }else if(line.startsWith("Content-Length: ")){
        line.remove(0, 16);
        contentLength = line.toInt();
  
      }else if(line == "\r"){
        line.trim();
        break;
      } 
    }

    // Update the nextion display
    if(code == 200){
      Serial.println("File received. Update Nextion...");
      
      ESPNexUpload nextion(client, contentLength, 115200);
      
      if(nextion.upload()){
        updated = true;
        Serial.println("Succesfully updated Nextion!");
      }else{
        Serial.println("Error updating Nextion: " + nextion.statusMessage);
      }
    }
  
    Serial.println("Closing connection\n");
  }
}
