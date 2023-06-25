

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
#include "index.h"


ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const char* ssid = "mktApaITU";
const char* pass = "1234567890";


IPAddress staIP(192,168,1,23);
IPAddress staGW(192,168,1,1);
IPAddress staDNS(192,168,1,200);
IPAddress subnet(255,255,255,0);

//pins
const int trigger = D7;
const int echo = D8;
const int Relay1 = D1;
const int Relay2 = D4; // high level trigger Relay

const float sow = 0.034; // 0.034 cm/uS
const float sensorH = 115.0;
const float LpCM = 5.0; // Volume per 1 cm water level increased

unsigned long pMillis =0;

int R1State = 0;
int R2State = 0;
int pMode = 0; // 0 - manual , 1 - automatic

//String page ="<html><body><h1>NodeMCU WiFi & WebServer</h1><p>Distance : <span id='distance'></span> cm</p><p>Water Level : <span id='waterLevel'></span> cm</p><p>Tank Volume : <span id='volume'></span> L</P><p>RelayState1 : <span id='relay1'></span></p><p>RelayState2 : <span id='relay2'></span></p><p>Pump Mode : <span id='mode'></span></p></body><script>var Socket;function init(){Socket = new WebSocket('ws://' + window.location.hostname + ':81/');Socket.onmessage = function(event){processCommand(event);};}function processCommand(event){var obj = JSON.parse(event.data);document.getElementById('distance').innerHTML = obj.wDistance;document.getElementById('waterLevel').innerHTML = obj.waterLevel;document.getElementById('volume').innerHTML = obj.volume;document.getElementById('relay1').innerHTML = obj.relay1state;document.getElementById('relay2').innerHTML = obj.relay2state;if(obj.pMode == '1'){document.getElementById('mode').innerHTML = 'Automatic';}else{document.getElementById('mode').innerHTML = 'Manual';}console.log(obj.waterLevel);console.log(obj.tankVOlume);}window.onload = function(event) {init();}</script></html>";
String page = FPSTR(MAIN_PAGE);

void setup() {
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

  //request handler
  server.on("/",rootHandler);
  server.on("/getDuration",durationHandler);
  server.on("/getDistance",distanceHandler);
  server.begin();

  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  //sensor pinMode
  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(Relay1,OUTPUT);
  pinMode(Relay2,OUTPUT);
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  
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
      int cid = doc["ch_id"];
      int cdata = doc["ch_data"];
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

  if(millis()-pMillis>1000){ // 5000 milli seconds
      pMillis = millis();
      sendServerData();
  }


}
