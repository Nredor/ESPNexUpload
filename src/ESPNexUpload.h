/**
 * @file NexUpload.h
 *
 * The definition of class NexUpload. 
 * 
 * Modified to work with ESP32, HardwareSerial and removed SPIFFS dependency
 * @author Onno Dirkzwager (onno.dirkzwager@gmail.com)
 * @date   2018/12/26
 * @version 0.3.0
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

#ifndef __ESPNEXUPLOAD_H__
#define __ESPNEXUPLOAD_H__
#include <functional>
#include <Arduino.h>
#include <StreamString.h>

/**
 * @addtogroup CoreAPI 
 * @{ 
 */

// callback template definition
typedef std::function<void(void)> THandlerFunction;

/**
 *
 * Provides the API for nextion to upload the ftf file.
 */
class ESPNexUpload
{
public: /* methods */
	
	// callback template definition
	typedef std::function<void(void)> THandlerFunction;

    /**
     * Constructor. 
     * 
     * @param uint32_t upload_baudrate - set upload baudrate.
     */
    ESPNexUpload(uint32_t upload_baudrate);
    
    /**
     * destructor. 
     * 
     */
    ~ESPNexUpload(){}
	
    /**
     * Connect to Nextion over serial
     *
     * @return true or false.
     */
    bool connect();
    
    /**
     * prepair upload. Set file size & Connect to Nextion over serial
     *
     * @return true if success, false for failure.
     */
	bool prepairUpload(uint32_t file_size);
    
    /**
     * set Update Progress Callback. (What to do during update progress)
     *
     * @return none
     */
	void setUpdateProgressCallback(THandlerFunction value);

    /**
     * start update tft file to nextion. 
     * 
     * @param const uint8_t *file_buf
     * @param size_t buf_size
     * @return true if success, false for failure.
     */
    bool upload(const uint8_t *file_buf, size_t buf_size);

    /**
     * start update tft file to nextion. 
     * 
     * @param Stream &myFile
     * @return true if success, false for failure.
     */
    bool upload(Stream &myFile);
	
    /**
     * Send reset command to Nextion over serial
     *
     * @return none.
     */
	void softReset(void);

    /**
     * Send reset, end serial, reset _sent_packets & update status message
     *
     * @return none.
     */
	void end(void);
	
	
	
public: /* data */ 

    String statusMessage = "";

private: /* methods */

    /*
     * get communicate baudrate. 
     * 
     * @return communicate baudrate.
     *
     */
    uint16_t _getBaudrate(void);

    /*
     * search communicate baudrate.
     *
     * @param baudrate - communicate baudrate.
     *   
     * @return true if success, false for failure. 
     */
    bool _searchBaudrate(uint32_t baudrate);

    /*
     * set download baudrate.
     *
     * @param baudrate - set download baudrate.
     *   
     * @return true if success, false for failure. 
     */
    bool _setUploadBaudrate(uint32_t baudrate);

    
    /*
     * Send command to Nextion.
     *
     * @param cmd - the string of command.
     *
     * @return none.
     */
    void sendCommand(const char* cmd);

    /*
     * Receive string data. 
     * 
     * @param buffer - save string data.  
     * @param timeout - set timeout time. 
     * @param recv_flag - if recv_flag is true,will braak when receive 0x05.
     *
     * @return the length of string buffer.
     *
     */   
    uint16_t recvRetString(String &string, uint32_t timeout = 500,bool recv_flag = false);
    
private: /* data */ 
    uint32_t _baudrate; 	        /* nextion serail baudrate */
    uint32_t _undownloadByte; 	    /* undownload byte of tft file */
    uint32_t _upload_baudrate;      /* upload baudrate */
    uint16_t _sent_packets = 0;     /* upload baudrate */
	
	THandlerFunction _updateProgressCallback;
};
/**
 * @}
 */

#endif /* #ifndef __ESPNEXUPLOAD_H__ */
