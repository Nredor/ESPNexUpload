#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <FS.h>
#include <SoftwareSerial.h>
#include <ESPNexUpload.h>

SoftwareSerial softSerial(5, 4); /* For Wemos D1 mini RX:D1/5, TX:D2/4 */

const char* ssid = "your_wlan_ssid";
const char* password = "your_wlan_password";
const char* host = "nextion";

// Name for updatefile (no need to change, used only internally)
String updateFileName = "/update.tft";

ESP8266WebServer server(80);

//holds the current upload
File fsUploadFile;

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

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

void handleFileUpload(){ // upload a new file to the SPIFFS
  HTTPUpload& upload = server.upload();

  // Check if file seems valid nextion tft file
  if(!upload.filename.endsWith(".tft")){
    return server.send(500, "text/plain", "ONLY TFT FILES ALLOWED\n"); 
  }
        
  if(upload.status == UPLOAD_FILE_START){
    // String filename = upload.filename;
    
    // Using always same name for updates
    String filename = updateFileName;
    
    //if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    
    fsUploadFile = SPIFFS.open(filename, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);

      Serial.println("Sending file to display");
      updateNextion();
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

void updateNextion() {
  ESPNexUpload nex_download(updateFileName.c_str(), 115200, &softSerial);
  
  String status = "";
  bool result = nex_download.upload(status);
  
  if(result) {
    server.sendHeader("Location","/success.html");      // Redirect the client to the success page
    server.send(303);
  } else {
    server.sendHeader("Location","/failure.html?reason=" + status);      // Redirect the client to the success page
    server.send(303);
  }
}

void setup(void){
  Serial.begin(115200);
  Serial.print("\n");
  
  Serial.setDebugOutput(true);
  SPIFFS.begin();  

  //WIFI INIT
  Serial.printf("Connecting to %s\n", ssid);
  if (String(WiFi.SSID()) != String(ssid)) {
    WiFi.begin(ssid, password);
  }
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  MDNS.begin(host);
  Serial.print("http://");
  Serial.print(host);
  Serial.println(".local");
  
  //SERVER INIT
  server.on("/", HTTP_POST,                       // if the client posts to the upload page
    [](){ server.send(200); },                          // Send status 200 (OK) to tell the client we are ready to receive
    handleFileUpload                                    // Receive and save the file
  );

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  server.begin();
  Serial.println("HTTP server started");

}
 
void loop(void){
  server.handleClient();
}
