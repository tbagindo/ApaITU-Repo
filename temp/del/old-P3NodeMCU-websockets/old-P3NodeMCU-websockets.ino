/*
 April 2023 ApaITU
 video    : NodeMCU & Ultrasonic #2
 Series   : Water tank Project 
 Sensor   : HC-SR04
 
 We will use hc-sr04 to measure water tank level, for example
 we have water tank
 capacity       : 500L
 Height         : 100cm 
 Sensor Height  : 115cm (from bottom of tank)

 by measuring the distance from sensor to water  surface we could calculate the water level
 waterLevel = SensorHeight - sensingDistance

 Based on waterLevel we could also measure the water volume, we need to calculate the volume/cm 
 VolumePerCM  = capacity/100cm
              = 500L/100cm
              = 5L/cm

 Volume       = waterLevel * VolumePerCM
 for example we read waterLevel = 10cm

 Volume       = 10cm * 5L/cm
              = 50L

 Who ultrasonic works?
 Ultrasonic act like a radar, measuring the distance by  sending ultrasonic sound wave (40Khz) 
 and timing the wave travel since it sended from sensor, reflecting from the object and traveling back 
 to the sensor.

 Sound wave has speed of 340m/s if we multiple it with the time (duration) we get the distance value. 
 We have take into account the measuring time is twice the distance.

 The formula :
 distance     = 0.5 * duration * 340m/s

 Typically HC-SR04 measuring the traveltime in uS or 1 of million of seconds and calculate the distance
 in cm, we need to transform the unit from m/s to cm/uS
 SpeedOfSound_CMuS  = 340m * 100cm/m / (1S * 1000000uS/S)
                    = 34000cm/1000000uS
                    = 0.034 cm/uS

 how microController get information from HC-SR04?
 HC-SR04 has alot of variant, working on 3.3V, 5V i2c protocol or simple with trigger and echo pin.
 We use 4 pin with echo and trigger pins
 vcc      >> 3.3V
 gnd      >> gnd
 trigger  >> D5
 echo     << D6

 1)  resetting the sensor
 we need to send signal HIGH for  2ms to sensor. this sequence will reset the sensor
 2) triggering the sensor for start sending sound wave :
 send singal HIGH for 10ms
 3) timing the  HIGH signal comming from echo pin

 float distanceCM(int triggerPin, int echoPin);
 long durationUS(int triggerPin, int echoPin);
 
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <ArduinoJson.h>

// wifi ssid & password
const char* ssid = "ApaITU";
const char* pass = "1234567890";

// IP Address

IPAddress staIP(192,168,2,20);
IPAddress staGW(192,168,2,1);
IPAddress staDNS(192,168,1,200);
IPAddress subnet(255,255,255,0);


// String page
String page = "<html><body><h1>NodeMCU WiFi & WebServer</h1><p>Duration : <span id='duration'></span> us</p><p>Distance : <span id='distance'></span> cm</P></body><script>var Socket;function init(){Socket = new WebSocket('ws://' + window.location.hostname + ':81/');Socket.onmessage = function(event){processCommand(event);};}function processCommand(event){var obj = JSON.parse(event.data);document.getElementById('duration').innerHTML = obj.duration;document.getElementById('distance').innerHTML = obj.distance;console.log(obj.duration);console.log(obj.distance);}window.onload = function(event) {init();}</script></html>";

// sensor pins
const int trigger = D5;
const int echo = D6;

// sensor const
const float sow = 0.034;  // 0.034 cm/uS
const float Lpcm = 5.0;   // Volume L per cm

// timer variable
uint32_t pMillis;

// constructor
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Ultrasonic  & NodeMCU");

  WiFi.config(staIP,staGW,subnet,staDNS);
  WiFi.begin(ssid,pass);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("WiFi Connected, IP : ");
  Serial.println(WiFi.localIP());

  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);
  server.on("/",rootHandler);
  server.begin();

  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
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
      break;
      
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\n", num, length);
      hexdump(payload, length);
      // send message to client
      // webSocket.sendBIN(num, payload, length);
      break;
  }

}


void rootHandler(){
  server.send(200,"text/html",page);
}

long durationUS(int triggerPin,int echoPin){
  //reset 
  digitalWrite(triggerPin,HIGH);
  delay(2);
  digitalWrite(triggerPin,LOW);

  //trigger
  digitalWrite(triggerPin,HIGH);
  delay(10);
  digitalWrite(triggerPin,LOW);

  long duration = pulseIn(echoPin,HIGH);
  Serial.print("Duration : ");
  Serial.print(duration);
  Serial.println(" uS");
  return duration;
}

float distanceCM(int triggerPin, int echoPin){

  float distance = 0.5 * durationUS(triggerPin,echoPin) * sow;
  Serial.println("Distance : " + String(distance) + " cm");
  return distance;
}

void sendServerData(){
  String jsonString ="";
  StaticJsonDocument<200>doc;
  JsonObject sensor = doc.to<JsonObject>();
  sensor["duration"] = durationUS(trigger,echo);
  sensor["distance"] = distanceCM(trigger,echo);
  serializeJson(doc,jsonString);
  Serial.println(jsonString);
  webSocket.broadcastTXT(jsonString);
}

void loop() {
  //watch  for event
  server.handleClient();
  webSocket.loop();
  if(millis() - pMillis>500){
    sendServerData();
    pMillis = millis();
  }

}
