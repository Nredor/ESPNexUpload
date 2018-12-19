/*
  This sketch is based on the SD card read/write example that comes with the ESP core.
  And was adapted for use with the ESPNexUpload library.

  This example shows how to read an nextion.tft file from SD 
  and send it to the Nextion display over serial.
  
  The ESP8266 circuit:
  SD card attached to SPI bus as follows:
  ** MOSI - pin 11
  ** MISO - pin 12
  ** CLK - pin 13
  ** CS - pin 4

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe
  modified 19 Dec 2018
  by Onno Dirkzwager

  This example code is in the public domain.
*/

#include <FS.h>
#if defined ESP32
  #include <SPIFFS.h>
#endif
#include <ESPNexUpload.h>

File myFile;
bool updated = false;


void setup() {
  Serial.begin(115200);
  delay(10);
  
  Serial.println("Mounting SPIFFS...");
  if(!SPIFFS.begin()){
       Serial.println("An Error has occurred while mounting SPIFFS");
       return;
  } 
}

void loop() {
  delay(5000);
  
  if(!updated){
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    Serial.println("Opening update.tft from SPIFFS");
    myFile = SPIFFS.open("update.tft", "r");      // open for reading
  
    if(myFile){
      Serial.println("File opened. Update Nextion...");
  
      ESPNexUpload nextion(myFile, myFile.size(), 115200);
      
      if(nextion.upload()){
        updated = true;
        Serial.println("Succesfully updated Nextion!");
      }else{
        Serial.println("Error updating Nextion: " + nextion.statusMessage);
      }
  
      // close the file:
      myFile.close();
    }else{
      // if the file didn't open, print an error:
      Serial.println("error opening update.tft");
    }
  }
}