#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


IPAddress staIP(192,168,2,25);
IPAddress gw(192,168,2,1);
IPAddress mask(255,255,255,0);
IPAddress dns(192,168,1,200);

const char* ssid = "ApaITU";
const char* pass = "1234567890";

//String url="http://192.168.2.24/pumpON";
String url="http://192.168.1.23/pumpTGL";

void setup(){
    //Serial.begin(115200);
    //Serial.println();
    //Serial.println();
    //Serial.println("["  + String(millis()) + "ms]\tboooooting");
    WiFi.config(staIP,gw,mask,dns);
    WiFi.begin(ssid,pass);
    while(WiFi.status()!= 3){
        static int counter=0;
        delay(200);
        //Serial.print(".");
        if(counter>20)ESP.deepSleep(0);
        counter++;
    }
    //Serial.println(staIP);
    
}



void loop(){
    static int counter;
    if(WiFi.status() == 3){
        WiFiClient client;
        HTTPClient http;
        http.begin( client, url);
        int httpCode = http.GET();
        if(httpCode>0){
            String payload = http.getString();
            if(payload == "OK"){
                for(int i=0; i<10; i++){
                  digitalWrite(2,LOW);
                  delay(200);
                  digitalWrite(2,HIGH);
                  delay(200);
                }
                //Serial.println("["  + String(millis()) + "ms]\tsleeping...");
                ESP.deepSleep(0);
            }
        }
    }
    delay(1000);
    //Serial.println("Gagal "  +String(counter++));
    if(counter>3)ESP.deepSleep(0);
}
    
