# ESPNexUpload
ESP8266 library for uploading .tft file to Nextion display using SPIFFS and SoftwareSerial. Currently it isn't possible to use HardwareSerial.

## Usage
See example. Library expects that given file path is found at ESP8266 flash memory(SPIFFS). Included example provides webserver with file upload to SPIFFS. Sometimes upload doesnt work -> try to power cycle display.

## Note on baudrates
- Nextion <-> ESP8266 communication seems to be quite fault prone. 
- With ESP8266 arduino core v2.4.0: Doesn't work with 115200 baudrate. 
57600 seems to work fine. 
- With ESP8266 arduino core v2.3.0: 115200 works fine. 

## Releases
- v0.2.0 - Removed unnecessary delays. Works now with latest ESP8266 arduinocore (see notes)
- v0.1.1 - Updated to comply with Arduino Library manager
- v0.1.0 - Working with basic functionality. 

## Notices
Original version is part of https://github.com/itead/ITEADLIB_Arduino_Nextion
