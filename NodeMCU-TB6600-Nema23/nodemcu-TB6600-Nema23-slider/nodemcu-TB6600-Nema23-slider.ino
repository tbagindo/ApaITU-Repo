#include <AccelStepper.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include "index.h"
#define PULSPIN D5
#define DIRPIN D6
#define ENPIN D7
 
//AccelStepper Mode = 1, Driver with 2 pin step and dir
AccelStepper stepper(1, PULSPIN, DIRPIN);


ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const char* ssid = "ApaITU";
const char* pass = "1234567890";


IPAddress staIP(192,168,2,24);
IPAddress staGW(192,168,2,1);
IPAddress staDNS(192,168,1,200);
IPAddress subnet(255,255,255,0);

String page = FPSTR(MAIN_PAGE);

uint32_t pMillis;
long cPos=0;  // value from currentPosition()
long aPos=0;  // absolute Position
long lPos=0;

int mState =0;
int bState =0;
long step;
long reqSteps = 0;
long reqSpeed = 0;
long reqAccel = 0;
char comChar;

bool rFlag = false;
bool bFlag = false;

int counter =0;
int pCounter =0;
 
void setup()
{
  motorInit();
  Serial.begin(115200); //define baud rate
  Serial.println("Testing Accelstepper"); //print a message
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

  server.on("/", [](){server.send(200,"text/html",page);});
  server.begin();  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
 
}
void loop(){
  server.handleClient();
  webSocket.loop();
  serialHandler();
  motorHandler();
  if(millis()-pMillis>600){
      sendWsData();
      pMillis=millis();
  }
}
 
 
