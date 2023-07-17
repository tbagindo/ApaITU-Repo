/* 
 *  
 *  ESP01 WiFi Remote sketch
 */
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

IPAddress staIP(192,168,2,25);
IPAddress gw(192,168,2,1);
IPAddress mask(255,255,255,0);
IPAddress dns(192,168,1,200);

const char* ssid="ApaITU";
const char* pass="1234567890";

void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.config(staIP,gw,mask,dns);
  WiFi.begin(ssid,pass);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "http://192.168.2.24/pumpON");
    int httpCode = http.GET();
    if(httpCode>0){
      String payload = http.getString();
      if(payload  == "ON"){
        Serial.println("millis() : " + String(millis()));
        delay(1000);
        ESP.deepSleep(0);
      }
    }
  }
}
