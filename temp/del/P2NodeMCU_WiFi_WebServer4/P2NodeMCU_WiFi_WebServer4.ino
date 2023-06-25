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

ESP8266WebServer server(80);

String page = "<html><body><h1>NodeMCU WiFi & WebServer<h1><p>Duration : <span id='duration'>@@DURATION@@</span> us</p><p>Distance : <span id='distance'>@@DISTANCE@@</span> cm</P></body><script> setInterval(function ( ) {var xhttp = new XMLHttpRequest();xhttp.onreadystatechange = function() {if (this.readyState == 4 && this.status == 200) {document.getElementById('duration').innerHTML = this.responseText;}};xhttp.open('GET', '/getDuration', true);xhttp.send();}, 1000 ) ; setInterval(function ( ) {var xhttp = new XMLHttpRequest();xhttp.onreadystatechange = function() {if (this.readyState == 4 && this.status == 200) {document.getElementById('distance').innerHTML = this.responseText;}};xhttp.open('GET', '/getDistance', true);xhttp.send();}, 1000 ) ;</script></html>";

const char* ssid = "ApaITU";
const char* pass = "1234567890";


IPAddress staIP(192,168,2,20);
IPAddress staGW(192,168,2,1);
IPAddress staDNS(192,168,1,200);
IPAddress subnet(255,255,255,0);

//pins
const int trigger = D5;
const int echo = D6;
float sow = 0.034; // 0.034 cm/uS

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

  server.on("/",rootHandler);
  server.on("/getDuration",durationHandler);
  server.on("/getDistance",distanceHandler);
  server.begin();

  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);
  
}

void durationHandler(){
  String str = (String)durationUS(trigger,echo);
  server.send(200,"text/plain",str);
}

void distanceHandler(){
  String str = (String)distanceCM(trigger,echo);
  server.send(200,"text/plain",str);
}

void rootHandler(){
  String str = page;
  str.replace("@@DURATION@@",(String)durationUS(trigger,echo));
  str.replace("@@DISTANCE@@",(String)distanceCM(trigger,echo));
  server.send(200,"text/html",str);
}
void loop() {
  server.handleClient();
  

}
