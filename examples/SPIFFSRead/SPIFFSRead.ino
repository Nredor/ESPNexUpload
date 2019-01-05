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
  
  Serial.println("\nRunning SPIFSSRead Example\n");
  
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
    Serial.println("\n\nOpening update.tft from SPIFFS");
    myFile = SPIFFS.open("/update.tft", "r");      // open for reading
  
    if(myFile){
      Serial.println("File opened. Update Nextion...");

      bool result;
      
      // initialize ESPNexUpload
      ESPNexUpload nextion(115200);

      // set callback: What to do / show during upload.... Optional!
      nextion.setUpdateProgressCallback([](){
        Serial.print(".");
      });
      
      // prepare upload: setup serial connection, send update command and send the expected update size
      result = nextion.prepareUpload(myFile.size());
      
      if(!result){
          Serial.println("Error: " + nextion.statusMessage);
      }else{
          Serial.print(F("Start upload. File size is: "));
          Serial.print(myFile.size());
          Serial.println(F(" bytes"));
          
          // Upload the received byte Stream to the nextion
          result = nextion.upload(myFile);
          
          if(result){
            updated = true;
            Serial.println("\nSuccesfully updated Nextion!");
          }else{
            Serial.println("\nError updating Nextion: " + nextion.statusMessage);
          }

          // end: wait(delay) for the nextion to finish the update process, send nextion reset command and end the serial connection to the nextion
          nextion.end();
      }
  
      // close the file:
      myFile.close();
    }else{
      // if the file didn't open, print an error:
      Serial.println("error opening update.tft");
    }
  }
}
