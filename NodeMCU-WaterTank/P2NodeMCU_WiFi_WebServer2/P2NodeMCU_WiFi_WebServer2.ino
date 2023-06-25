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

const char* ssid = "ApaITU";
const char* pass = "1234567890";


IPAddress staIP(192,168,2,20);
IPAddress staGW(192,168,2,1);
IPAddress staDNS(192,168,1,200);
IPAddress subnet(255,255,255,0);


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
  server.begin();

  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  
  
}

void rootHandler(){
  server.send(200,"text/plain","webserver");
}
void loop() {
  server.handleClient();
  

}
