

/*
 April 2023
 video    : NodeMCU WiFi & WebServer
 Series   : Water Tank Project
 Sensor   : HC-SR04

 ESP8266 WiFi Module
 Library  : #include <ESP8266WiFi.h>
 instance : WiFi

 WiFi Modes : WiFi.mode()
 1) WIFI_STA (default mode)
 2) WIFI_AP
 3) WIFI_AP_STA
 4) WIFI_OFF

 credential
 const char* ssid ="ApaITU";
 const char* pass ="1234567890";

 initiate WiFi station interface
 WiFi.begin(ssid,pass);
 
 static ip configuration
 WiFi.config(ip,gw,subnet,dns);


 ESP8266 WebServer
 Library  : #include <ESP8266WebServer.h>

 instance construction
 ESP8266WebServer server(80);

 server.begin();

 loop()
 server.handleClient();

 event handler
 server.on("/",rootHandler);

 void roodHandler(){
 }
 
 */


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const char* ssid = "mktApaITU";
const char* pass = "1234567890";


IPAddress staIP(192,168,1,24);
IPAddress staGW(192,168,1,200);
IPAddress staDNS(192,168,1,200);
IPAddress subnet(255,255,255,0);

//SPIFFS uploader
File fsUploadFile;

//pins
const int trigger = D6;
const int echo = D7;
const int Relay1 = D4;
const int Relay2 = D8; // low level trigger Relay

const float sow = 0.034; // 0.034 cm/uS
const float tankCapacity = 475.0;
const float sensorH = 100.0;
const float maxLevel = 95.0; //waterLevel at tankCapacity
const float LpCM = tankCapacity/maxLevel; // Volume per 1 cm water level increased
float distance = 0;

unsigned long pMillis =0;

//Relay State & operation Mode
int R1State = 0;
int R2State = 0;
int pMode = 0; // 0 - manual , 1 - automatic
int offLevel = 85;
int onLevel = 60;
int percent; // waterLevel in %

void setup() {
  SPIFFS.begin();
  Serial.begin(115200);
  Serial.println();
  Serial.println("NodeMCU WiFi & WebServer");

  //wifi
  WiFi.config(staIP,staGW,subnet,staDNS);
  WiFi.begin(ssid,pass);

  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("WiFi Connected, IP : ");
  Serial.println(WiFi.localIP());

  //server request handler
  server.serveStatic("/",SPIFFS, "/index.html");
  server.serveStatic("/index.css", SPIFFS, "/index.css");
  server.serveStatic("/index.js", SPIFFS, "/index.js");
  server.serveStatic("/apaitu.jpg", SPIFFS, "/apaitu.jpg");

  server.serveStatic("/tankdemo", SPIFFS, "/tankDemo.html");
  server.serveStatic("/tank.html", SPIFFS, "/tank.html");
  server.serveStatic("/tank.css", SPIFFS, "/tank.css");
  server.serveStatic("/tank.js", SPIFFS, "/tank.js");
 
  //Upload SPIFFS File over Network
  server.on("/upload", HTTP_POST, [](){
    server.send(200, "text/plain", "{\"success\":1}");
  }, handleFileUpload); 

  //File list
  server.on("/list",HTTP_GET,handleFileList);

  server.begin();

  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  //sensor pinMode
  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(Relay1,OUTPUT);
  pinMode(Relay2,OUTPUT);
  digitalWrite(Relay1,!R2State);
  digitalWrite(Relay2,!R2State);
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  otaSetup();
}

/*
void IndexFileHandler(){
  File file = SPIFFS.open("/index.html","r");
  server.streamFile(file, "text/html");
  file.close();
}
*/

void durationHandler(){
  String s = (String)durationUS(trigger,echo);
  server.send(200,"text/plain",s);
}

void distanceHandler(){
  String s = (String)distanceCM(trigger,echo);
  server.send(200,"text/plain",s);
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  webSocket.loop();
  OperationMode();
  //tProtection(89,80);  //percent > 89%,r2 on, 57%  r2 off

  if(millis()-pMillis>200){ // milli seconds
      pMillis = millis();
      sendServerData();
  }
}
