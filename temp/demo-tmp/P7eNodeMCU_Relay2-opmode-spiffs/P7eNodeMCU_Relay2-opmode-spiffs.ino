

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
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include "index.h"


ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const char* ssid = "mktApaITU";
const char* pass = "1234567890";

IPAddress staIP(192,168,1,23);
IPAddress staGW(192,168,1,200);
IPAddress staDNS(192,168,1,200);
IPAddress subnet(255,255,255,0);

//SPIFFS uploader
File fsUploadFile;
String fname;

//ino
String fileName = __FILE__;



//pins
int trigger = D5;
int echo = D6;
const int Relay1 = D1;
const int Relay2 = D2; // high level trigger Relay

const float sow = 0.034; // 0.034 cm/uS
const float tankCapacity = 475.0;
const float sensorH = 100.0;
const float maxLevel = 95.0;
const float LpCM = tankCapacity/maxLevel; // Volume per 1 cm water level increased
float distance;

unsigned long pMillis =0;
unsigned long pMillis2 =0;
const uint32_t dataInterval  = 400;
const uint32_t sensorInterval = 600;

int offLimit=92;
int onLimit=60;

int  counter;


int R1State = 0;
int R2State = 0;

//opMode
int pMode = 0; // 0 - manual , 1 - automatic
int minThreshold = 70;
int maxThreshold = 95;
int pState;
String page = FPSTR(MAIN_PAGE);

void setup() {
  SPIFFS.begin();
  Serial.begin(115200);
  Serial.println();
  Serial.println("NodeMCU WiFi & WebServer");
  Serial.println("read initial distances  value");
  
  //sensor pinMode
  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(Relay1,OUTPUT);
  pinMode(Relay2,OUTPUT);
  digitalWrite(Relay1,!R1State);
  digitalWrite(Relay2,!R2State);
  
  distance = dcal(distance,trigger,echo);
  //wifi
  WiFi.setOutputPower(20.5);
  WiFi.config(staIP,staGW,subnet,staDNS);
  WiFi.begin(ssid,pass);

  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("WiFi Connected, IP : ");
  Serial.println(WiFi.localIP());

  //request handler
  server.on("/index", handleIndexFile);
  server.on("/upload", HTTP_POST, [](){
    server.send(200, "text/plain", "{\"success\":1}");
  }, handleFileUpload);
  server.on("/list",HTTP_GET,handleFileList);
  server.on("/",rootHandler);
  server.on("/getDuration",durationHandler);
  server.on("/getDistance",distanceHandler);
  server.on("/T1",t1Handler);
  server.on("/T2",t2Handler);
  server.on("/restart",restartHandler);
  server.on("/fname",fileHandler);
  server.on("/pumpON",pumpON);
  server.on("/pumpTGL",pumpToggle);
  server.begin();

  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");


  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  otaSetup();
  
}

void pumpToggle(){
  R1State = !R1State;
  digitalWrite(Relay1,!R1State);
  server.send(200,"text/plain","OK");
}

void pumpON(){
  R1State =  1;
  digitalWrite(Relay1,!R1State);
  server.send(200,"text/plain","OK");
}

void fileHandler(){
  server.send(200,"text/plain",fileName);
}

void t1Handler(){
  trigger = D3;
  echo = D4;
  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);
  server.send(200,"text/html",page);
}

void t2Handler(){
  trigger = D5;
  echo = D6;
  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);
  server.send(200,"text/html",page);
}

void restartHandler(){
  ESP.restart();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message to client
        webSocket.sendTXT(num, "0");
      }
      break;

    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      // send message to client
      // webSocket.sendTXT(num, "message here");
      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      StaticJsonDocument<200>doc;
      deserializeJson(doc,payload);
      int cid = doc["did"];
      int cdata = doc["value"];
      Serial.println(cid);
      switch (cid){
        case 1:
          if(R1State != cdata){
            R1State = cdata;
            digitalWrite(Relay1,!R1State); 
          }
          break;
        case 2: 
          if(R2State != cdata){
            R2State = cdata;
            digitalWrite(Relay2,!R2State); 
          }
          break;
        case 3: 
          if(pMode != cdata)pMode = cdata;
          break;
      }
      pMillis=millis();
      break;
  }

}

    

void durationHandler(){
  String s = (String)durationUS(trigger,echo);
  server.send(200,"text/plain",s);
}


void distanceHandler(){
  String s = (String)distanceCM(trigger,echo);
  server.send(200,"text/plain",s);
}

void rootHandler(){
  
  //String str = page;
  //str.replace("@@DURATION@@", (String)durationUS(trigger,echo));
  //str.replace("@@DISTANCE@@", (String)distanceCM(trigger,echo));
  server.send(200,"text/html",page);
}
void loop() {
  server.handleClient();
  webSocket.loop();
  ArduinoOTA.handle();

  if(millis()-pMillis>1000){ // 5000 milli seconds
      pMillis = millis();
      sendServerData();
  }
  //get distance every 10seconds
  if(millis()-pMillis2>1000){
    pMillis2=millis();
    counter++;
    //Serial.println("distanceAVG :  " + String(distance) + "cm");
  }
}
