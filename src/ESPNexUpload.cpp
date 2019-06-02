/**
 * @file NexUpload.cpp
 *
 * The implementation of uploading tft file for nextion displays. 
 * 
 * Modified to work with ESP8266 and SoftwareSerial
 * @author Ville Vilpas (psoden@gmail.com)
 * @date   2018/2/3
 * @version 0.2.0
 *
 * Original version (a part of https://github.com/itead/ITEADLIB_Arduino_Nextion)
 * @author  Chen Zengpeng (email:<zengpeng.chen@itead.cc>)
 * @date    2016/3/29
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

//#define DEBUG_SERIAL_ENABLE
#include "ESPNexUpload.h"


#if defined ESP8266

	#include <SoftwareSerial.h>
	
	#ifndef NEXT_RX
		#define NEXT_RX 14	// Nextion RX pin | Default 14 / D5
		#define NEXT_TX 12	// Nextion TX pin | Default 12 / D6
	#endif
	#ifndef nexSerial
		SoftwareSerial softSerial(NEXT_RX, NEXT_TX);
		#define nexSerial softSerial
		#define nexSerialBegin(a) nexSerial.begin(a)
	#endif
	
#elif defined ESP32
	
	#ifndef NEXT_RX
		#define NEXT_RX 16	// Nextion RX pin | Default 16
		#define NEXT_TX 17	// Nextion TX pin | Default 17
	#endif
	#ifndef nexSerial
		#define nexSerial Serial2
		#define nexSerialBegin(a) nexSerial.begin(a, SERIAL_8N1, NEXT_RX, NEXT_TX)
	#endif
	
#endif


#ifdef DEBUG_SERIAL_ENABLE
    #define dbSerialPrint(a)    Serial.print(a)
    #define dbSerialPrintln(a)  Serial.println(a)
    #define dbSerialBegin(a)    Serial.begin(a)
#else
    #define dbSerialPrint(a)    do{}while(0)
    #define dbSerialPrintln(a)  do{}while(0)
    #define dbSerialBegin(a)    do{}while(0)
#endif



ESPNexUpload::ESPNexUpload(uint32_t upload_baudrate){
    _upload_baudrate = upload_baudrate;
}



bool ESPNexUpload::connect(){
    #if defined ESP8266
        yield();
    #endif
	
    dbSerialBegin(115200);
	dbSerialPrintln(F("Serial tests & connect"));
	
    if(_getBaudrate() == 0){
        statusMessage = F("get baudrate error");
        dbSerialPrintln(statusMessage);
        return false;
    }
    if(!_setUploadBaudrate(_upload_baudrate)){
        statusMessage = F("modifybaudrate error");
        dbSerialPrintln(statusMessage);
        return false;
    }
	
	return true;
}



bool ESPNexUpload::prepareUpload(uint32_t file_size){
    _undownloadByte = file_size;
	return this->connect();
}



uint16_t ESPNexUpload::_getBaudrate(void){
	
    _baudrate = 0;
    uint32_t baudrate_array[7] = {115200,19200,9600,57600,38400,4800,2400};
    for(uint8_t i = 0; i < 7; i++)
    {
        if(_searchBaudrate(baudrate_array[i]))
        {
            _baudrate = baudrate_array[i];
            dbSerialPrintln(F("get baudrate"));
            break;
        }
    }
    return _baudrate;
}



bool ESPNexUpload::_searchBaudrate(uint32_t baudrate){
	
    #if defined ESP8266
        yield();
    #endif
	
    String string = String("");  
    nexSerialBegin(baudrate);
	
    this->sendCommand("DRAKJHSUYDGBNCJHGJKSHBDNÿÿÿ");
    this->sendCommand("connectÿÿÿ");
    this->sendCommand("ÿÿconnectÿÿÿ");

    this->recvRetString(string);
    if(string.indexOf(F("comok")) != -1){
        return 1;
    } 
    return 0;
}



void ESPNexUpload::sendCommand(const char* cmd){
	
    #if defined ESP8266
        yield();
    #endif
	
    while(nexSerial.available()){
        nexSerial.read();
    }

    nexSerial.print(cmd);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
}



uint16_t ESPNexUpload::recvRetString(String &string, uint32_t timeout,bool recv_flag){
	
    #if defined ESP8266
        yield();
    #endif
	
    uint16_t ret = 0;
    uint8_t c = 0;
    long start;
    bool exit_flag = false;
    start = millis();
	
    while (millis() - start <= timeout){
		
        while (nexSerial.available()){
			
            c = nexSerial.read(); 
            if(c == 0){
                continue;
            }
			
            string += (char)c;
            if(recv_flag){
                if(string.indexOf(0x05) != -1){ 
                    exit_flag = true;
                } 
            }
        }
        if(exit_flag){
            break;
        }
    }
	
    ret = string.length();
    return ret;
}



bool ESPNexUpload::_setUploadBaudrate(uint32_t baudrate){
	
    #if defined ESP8266
        yield();
    #endif
	
    String string = String(""); 
    String cmd = String("");
    
    String filesize_str = String(_undownloadByte,10);
    String baudrate_str = String(baudrate);
    cmd = "whmi-wri " + filesize_str + "," + baudrate_str + ",0";

    dbSerialPrintln(cmd);
    this->sendCommand("");
    this->sendCommand(cmd.c_str());
    delay(50);

    nexSerialBegin(baudrate);
    dbSerialPrintln(F("Changing baudrate..."));
    dbSerialPrintln(baudrate);

    this->recvRetString(string, 500);
    if(string.indexOf(0x05) != -1)
    { 
        return 1;
    } 
    return 0;
}



void ESPNexUpload::setUpdateProgressCallback(THandlerFunction value){
	_updateProgressCallback = value;
}



bool ESPNexUpload::upload(const uint8_t *file_buf, size_t buf_size){
	
    #if defined ESP8266
        yield();
    #endif
	
    uint8_t c;
    uint8_t timeout = 0;
    String string = String("");
	
    for(uint16_t i = 0; i < buf_size; i++){
		
		// Users must split the .tft file contents into 4096 byte sized packets with the final partial packet size equal to the last remaining bytes (<4096 bytes).
		if(_sent_packets == 4096){
			
			// wait for the Nextion to return its 0x05 byte confirming reception and readiness to receive the next packets
			this->recvRetString(string,500,true);  
			if(string.indexOf(0x05) != -1){ 
				//Serial.println("Received 0x05");
				
				// reset sent packets counter
				_sent_packets = 0;
				
				// reset receive String
				string = "";
			}else{
				if(timeout >= 8){
					statusMessage = F("serial connection lost");
					dbSerialPrintln(statusMessage);
					return false;
				}
				
				timeout++;
				//Serial.println("Waiting for 0x05");
			}
			
			// delay current byte
			i--;
			
		}else{
			
			// read buffer
			c = file_buf[i];
			
			// write byte to nextion over serial
			nexSerial.write(c);
			
			// update sent packets counter
			_sent_packets++;
		}
    }
	
    return true;  
}



bool ESPNexUpload::upload(Stream &myFile){
    #if defined ESP8266
        yield();
    #endif
	
	// create buffer for read
	uint8_t buff[2048] = { 0 };

	// read all data from server
	while(_undownloadByte > 0 || _undownloadByte == -1){

		// get available data size
		size_t size = myFile.available();

		if(size){
			// read up to 2048 byte into the buffer
			int c = myFile.readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

			// Write the buffered bytes to the nextion. If this fails, return false.
			if(!this->upload(buff, c)){
				return false;
			}else{
				if(_updateProgressCallback){
					_updateProgressCallback();
				}
			}

			if(_undownloadByte > 0) {
				_undownloadByte -= c;
			}
		}
		delay(1);
	}

    return true;  
}



void ESPNexUpload::softReset(void){
	
    // soft reset nextion device
	this->sendCommand("rest");
}



void ESPNexUpload::end(){
	
    // wait for the nextion to finish internal processes
    delay(1600);
	
	// soft reset the nextion
	this->softReset();
	
    // end Serial connection
    nexSerial.end();
	
	// reset sent packets counter
	_sent_packets = 0;
	
    statusMessage = F("upload ok");
    dbSerialPrintln(statusMessage + F("\r\n"));
}
