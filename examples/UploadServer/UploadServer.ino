#include <FS.h>

#if defined ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266mDNS.h>
#elif defined ESP32
  #include <WiFi.h>
  #include <WebServer.h>
  #include <ESPmDNS.h>
  #include <SPIFFS.h>
#endif

#include <ESPNexUpload.h>

/*
  ESP8266 uses Software serial RX:5, TX:4 Wemos D1 mini RX:D1, TX:D2 
  ESP32 uses Hardware serial RX:16, TX:17
  Serial pins are defined in the ESPNexUpload.cpp file
*/

const char* ssid      = "your_wlan_ssid";
const char* password  = "your_wlan_password";
const char* host      = "nextion";

// used only internally
int fileSize  = 0;
bool result   = true;

// init Nextion object
ESPNexUpload nextion(115200);

#if defined ESP8266
  ESP8266WebServer server(80);
#elif defined ESP32
  WebServer server(80);
#endif


String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}


bool handleFileRead(String path){                           // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";             // If a folder is requested, send the index file
  String contentType = getContentType(path);                // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {   // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                          // If there's a compressed version available
      path += ".gz";                                        // Use the compressed verion
    File file = SPIFFS.open(path, "r");                     // Open the file
    size_t sent = server.streamFile(file, contentType);     // Send it to the client
    file.close();                                           // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);      // If the file doesn't exist, return false
  return false;
}


// handle the file uploads
void handleFileUpload(){
  HTTPUpload& upload = server.upload();

  // Check if file seems valid nextion tft file
  if(!upload.filename.endsWith(".tft")){
    return server.send(500, "text/plain", "ONLY TFT FILES ALLOWED\n"); 
  }
  
  if(!result){
    // Redirect the client to the failure page
    server.sendHeader("Location","/failure.html?reason=" + nextion.statusMessage);
    server.send(303);
    return false;
  }

  Serial.println("\nConnect to Nextion display");
        
  
  if(upload.status == UPLOAD_FILE_START){

    // Prepair the Nextion display by seting up serial and telling it the file size to expect
    result = nextion.prepairUpload(fileSize);
    
    if(result){
      Serial.print("Start upload. File size is: ");
      Serial.print(fileSize);
      Serial.println(" bytes");
    }else{
      Serial.println(nextion.statusMessage + "\n");
      return false;
    }
    
  }else if(upload.status == UPLOAD_FILE_WRITE){

    // Write the received bytes to the nextion
    result = nextion.upload(upload.buf, upload.currentSize);
    
    if(result){
      Serial.print(".");
    }else{
      Serial.println(nextion.statusMessage + "\n");
      return false;
    }
  
  }else if(upload.status == UPLOAD_FILE_END){

    // End the serial connection to the Nextion and softrest it
    nextion.end();
    
    Serial.println("");
    //Serial.println(nextion.statusMessage);
    return true;
  }
}


void setup(void){
  Serial.begin(115200);
  Serial.println("");
  
  Serial.setDebugOutput(true);
  if(!SPIFFS.begin()){
       Serial.println("An Error has occurred while mounting SPIFFS");
       return;
  } 

  //WIFI INIT
  Serial.printf("Connecting to %s\n", ssid);
  if (String(WiFi.SSID()) != String(ssid)) {
    WiFi.begin(ssid, password);
  }
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected! IP address: ");
  Serial.println(WiFi.localIP());

  MDNS.begin(host);
  Serial.print("http://");
  Serial.print(host);
  Serial.println(".local");
  
  //SERVER INIT
  server.on("/", HTTP_POST, [](){ 
    Serial.println("Succesfull upload\n");
    
    // Redirect the client to the success page after handeling the file upload
    server.sendHeader("Location","/success.html");
    server.send(303);
    return true;
  },
    // Receive and save the file
    handleFileUpload
  );

  // receive fileSize once a file is selected (Workaround as the file content-length is of by +/- 200 bytes. Known issue: https://github.com/esp8266/Arduino/issues/3787)
  server.on("/fs", HTTP_POST, [](){
    fileSize = server.arg("fileSize").toInt();
    server.send(200, "text/plain", "");
  });

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  server.begin();
  Serial.println("\nHTTP server started");

}
 
void loop(void){
  server.handleClient();
}
