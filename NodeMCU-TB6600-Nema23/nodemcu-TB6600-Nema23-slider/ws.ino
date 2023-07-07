void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      //Serial.printf("[%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        //Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message to client
        webSocket.sendTXT(num, "0");
      }
      break;

    case WStype_TEXT:
      //Serial.printf("[%u] get Text: %s\n", num, payload);
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
  reqSteps = doc["step"];
  reqSpeed = doc["speed"];
  reqAccel = doc["accel"];
  mState = doc["mState"];
  bState = doc["bState"];
  int dir = doc["dir"];
  //Serial.println(bState);
  if(mState || bState){
    rFlag = true;
    stepper.setAcceleration(reqAccel);
    //Serial.println("button Run");
    switch (dir){
      case 0:
        step = reqSteps;
        bFlag = false;
        break;
      case 1: 
        step = -1 * reqSteps;
        bFlag = false;
        break;
      case 2:
        step = reqSteps;
        bFlag = true;
        break;
    }
    stepper.setMaxSpeed(reqSpeed); //set speed
    stepper.move(step); //set distance
     
  }else{
    mStop();
  }

}
void sendWsData(){
    String jsonString ="";
    StaticJsonDocument<200>doc;
    JsonObject object = doc.to<JsonObject>();
  
    //insert value to objects
    object["aSpeed"] = String(stepper.speed(),2);
    object["cPos"] = cPos;
    object["aPos"] = aPos;
    object["lPos"] = lPos;
    object["mState"] = mState;
    object["bState"] = bState;
  
    //serialize Json
    serializeJson(doc,jsonString);
  
    //send jsonString over webSockets
    webSocket.broadcastTXT(jsonString);      
}
