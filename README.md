# ESPNexUpload
ESP8266 & ESP32 library for uploading .tft files to the Nextion display over serial. The ESP32 uses HardwareSerial and the ESP8266 uses SoftwareSerial. Pins are defined in the ESPNexUpload.cpp file.

## IMPORTANT NOTES

### Arduino core version
ESP8266: Due to bugs in older arduino cores we recommend using version 2.5.1 or higher.
ESP32: Latest arduino core should work fine

### Hardresets (should be no need for this after library version 0.5.5)
If the nextion displays an error: Failed to update, Connection lost, Incompatible upload etc.
You will need to hardreset(power cycle) the display before trying the next update. This might be an issue if you are updating from a remote location. You should therefore consider adding a transistor or relay to your nextion power lines.

## Usage
This library expects a byte (buffer of Stream), filesize and serial baudrate as parameters. The examples provided demonstrate how to use these with Wifi, SPIFFS, SD cards and a local webserver. 

#### Begin
```C++
  // initialize ESPNexUpload
  ESPNexUpload nextion(115200);
  
  // prepare upload: setup serial connection, send update command and send the expected update size
  // returns a True or False.
  nextion.prepareUpload(fileSize);
```

#### When providing a buffer
```C++
  /* begin buffer loop */
      // Upload the received byte buffer to the nextion
      // returns a True or False.
      nextion.upload(upload.buf, upload.bufSize);
  /* end buffer loop */
```
The library example "[UploadServer](examples/UploadServer/UploadServer.ino#L100)" uses this approach.
</br>

#### When providing a stream
```C++
  // Upload the received byte Stream to the nextion
  // returns a True or False.
  nextion.upload(stream)
```
The library examples "[WifiClient](examples/WifiClient/WifiClient.ino#L128), [HttpClient](examples/HttpClient/HttpClient.ino#L123), [SPIFFSRead](examples/SPIFFSRead/SPIFFSRead.ino#L62) & [SDRead](examples/SDRead/SDRead.ino#L61)" use this approach.
</br>

#### End
```C++
  // end: wait(delay) for the nextion to finish the update process,
  // send nextion reset command and end the serial connection to the nextion
  nextion.end();
```
</br>

## Releases
- v0.5.7 - Merged two small changes
- v0.5.6 - Forgot to update library.properties version for Arduino Library manager, fixed here.
- v0.5.5 - Bug fixes regarding baud rates and improvements for debug messages
- v0.5.0 - Improved implementation of the nextion upload v1.1 protocol
- v0.4.0 - Fixed combatibility issues with some displays
- v0.3.1 - Fixed typo in UploadServer example
- v0.3.0 - ESP32 support and lots of improvements by Onno-Dirkzwager
- v0.2.0 - Removed unnecessary delays. Works now with latest ESP8266 arduinocore (see notes)
- v0.1.1 - Updated to comply with Arduino Library manager
- v0.1.0 - Working with basic functionality. 

## Notices
Original version is part of https://github.com/itead/ITEADLIB_Arduino_Nextion
