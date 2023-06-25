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
      inTXT(payload);
      break;
  }
}


void inTXT(uint8_t * payload){
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
      if(pMode != cdata){
        pMode = cdata;
        if(pMode == 0){
          R1State = 0;
          R2State = 0;
          digitalWrite(Relay1,!R1State);
          digitalWrite(Relay2,!R2State);
        }
      }
      break;
  }
}
