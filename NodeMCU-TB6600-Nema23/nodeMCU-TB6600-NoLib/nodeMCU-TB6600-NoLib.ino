#define PULSEPIN D5
#define DIRPIN D6
#define ENPIN D7

#include <ESP8266WiFi.h>

const char* ssid = "mktApaITU";
const char* pass = "1234567890";


IPAddress staIP(192,168,1,24);
IPAddress staGW(192,168,1,200);
IPAddress staDNS(192,168,1,200);
IPAddress subnet(255,255,255,0);

int dir=0;
int PulsePerFS = 4;                   // 4 Pulses Per Full Step
int mstepsPerRev = 200 * PulsePerFS;  // 1.8deg per fullStep >> 200 FullSteps per Revolution
int revs = 8;                         // requested Revolutions
int pulseSend = mstepsPerRev * revs ; // 2400 pulses send to the TB6600 => 3 revolution
uint32_t stepsDelay=200;              // 500 microseconds delay * 2 = 1ms delay between pulse
                                      // 60 microSteps/seconds

void setup() {
  Serial.begin(115200);
  Serial.println();

  //wifi 
  /*
  WiFi.config(staIP,staGW,subnet,staDNS);
  WiFi.begin(ssid,pass);

  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi Connected, IP : ");
  Serial.println(WiFi.localIP());
  */  
  pinMode(PULSEPIN,OUTPUT);
  pinMode(DIRPIN,OUTPUT);
  pinMode(ENPIN,OUTPUT);
  digitalWrite(ENPIN,LOW);

}

void loop() {
  digitalWrite(DIRPIN,!dir);
  Serial.println("Change Dir and Start MicroStepping");
  for(int i =0; i<pulseSend; i++){
    digitalWrite(PULSEPIN,HIGH);
    delayMicroseconds(stepsDelay);
    digitalWrite(PULSEPIN,LOW);
    delayMicroseconds(stepsDelay);
    yield();
  }
  Serial.println("Done !!! Delay 1 Seconds");
  delay(1000);
}
