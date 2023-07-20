#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

IPAddress staIP(192,168,2,24);
IPAddress gw(192,168,2,1);
IPAddress mask(255,255,255,0);
IPAddress dns(192,168,1,200);

const char* ssid = "ApaITU";
const char* pass = "1234567890";

static const char MAIN_PAGE[] PROGMEM = R"==(
<html>
<body>
     <h1>wifi switch esp01</h1>
    pump status  :  <span id="state"></span>
</body>
<script>
     setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
       document.getElementById("state").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/pState", true);
    xhttp.send();
  }, 1000 ) ;
</script>
</html>
)==";

const int PUMP = D4;
int pState = 0;
uint32_t pMillis;
int counter = 0;

void setup(){
    Serial.begin(115200);
    Serial.println();
    pinMode(PUMP,OUTPUT);
    digitalWrite(PUMP, !pState);
    WiFi.config(staIP,gw,mask,dns);
    WiFi.begin(ssid,pass);
    while(WiFi.status()!= 3){
        Serial.print(".");
        delay(1000);
    }
    Serial.println();
    server.on("/",indexHandler);
    server.on("/pState",pumpState);
    server.on("/pumpON",pumpON);
    server.begin();
    Serial.print("http://");
    Serial.print(staIP);
    Serial.println("/");
    
}

void indexHandler(){
    String page = FPSTR(MAIN_PAGE);
    server.send(200,"text/html",page);
    Serial.println("indexHandler");
}

void pumpState(){
    static int pcount=0;
    String state;
    if(pState){
        state ="ON";
    }else{
        state ="OFF";
    }
    server.send(200,"text/plain",state);
    Serial.println("/pState : " + String(pcount++));
}

void pumpON(){
    pState = 1;
    digitalWrite(PUMP,!pState);
    server.send(200,"text/plain","OK");
    Serial.println("/pumpaON");
}

void loop(){
    server.handleClient();

    //=====START OF  DEMO
    //demostration purpose only turn off pump after 6seconds
    static bool cFlag;
    uint32_t now = millis();
    if(now-pMillis>1000){
        pMillis = now;
        if(pState){
            if(!cFlag){
                Serial.println("Start Counter");               
                cFlag = true;  
            }
            counter++;      
            if(counter>3){
                counter = 0;
                pState = 0;
                digitalWrite(PUMP,!pState);
                Serial.println("Reset Pump");
                
            }
        }else{
            if(cFlag){
                cFlag = false;
                counter = 0;
            }
        }
           
      }
      // END OF DEMO ======
    }
