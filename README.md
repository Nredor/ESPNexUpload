# ESPNexUpload
ESP8266 & ESP32 library for uploading .tft files to the Nextion display over serial. The ESP32 uses HardwareSerial and the ESP8266 uses SoftwareSerial. Pins are defined in the ESPNexUpload.cpp file.

## Usage
This library expects a Stream, filesize and serial baudrate as parameters. The examples provided demonstrate how to get file Streams from Wifi, SPIFFS, SD cards and a local webserver. 

## Hardresets
If the nextion displays an error: Failed to update, Connection lost, Incompatible upload etc.
You will need to hardreset(power cycle) the display before trying the next update. This might be an issue if you are updating from a remote location. You should therefore consider adding a transistor or relay to your nextion power lines.

## Note on baudrates
- In previous versions the Nextion <-> ESP communication was quite fault prone. 
- This seems to no longer be the case. 115200 seems to work fine. 

## Releases
- v0.2.0 - Removed unnecessary delays. Works now with latest ESP8266 arduinocore (see notes)
- v0.1.1 - Updated to comply with Arduino Library manager
- v0.1.0 - Working with basic functionality. 

## Notices
Original version is part of https://github.com/itead/ITEADLIB_Arduino_Nextion
