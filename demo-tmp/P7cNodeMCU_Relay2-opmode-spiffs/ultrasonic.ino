float dcal(float dist, int triggerPin, int  echoPin){
  for(int i=0; i<10; i++){
    dist = ((9 * dist) + distanceCM(triggerPin,echoPin))/10.0;
    delay(100);
  }
  return dist;
}

float distAVG(float dist, int triggerPin,  int echoPin){
  float readDist = distanceCM(triggerPin,echoPin);
  //if(abs(dist-readDist)<10.0){
  if(readDist<100.0){
      dist = ((99*dist) + readDist)/100.0; 
      Serial.println("distAVG : " + String(dist,2));
  }
  //}
  
  return dist;

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

  float d = 0.5 * durationUS(triggerPin,echoPin) * sow;
  Serial.println("Distance : " + String(d) + " cm");
  
  return d;
}

float wLevel(float dist){
  float f = sensorH - dist;
  if(f<0)f=0;
  if(f>maxLevel)f = maxLevel;
  return f;
}

float wVolume(float level){
  float f = LpCM * level;
  return f;
}


int pCent(float wl){
  int p = (int) ((wl)/maxLevel)*100.0;
  Serial.println("level : "  + String(p) +  "%");
  //if(p>99)p=100;
  //if(p<1)p=0;
  return p;
}

void sendServerData(){

  float rawDist = distanceCM(trigger,echo);
  distance = round2(distAVG(distance,trigger,echo));
  float waterLevel = wLevel(distance);
  waterLevel= round2(waterLevel);
  float volume = wVolume(waterLevel);
  volume = round2(volume);
  int percent = (int)((waterLevel/maxLevel)*100);

  if(percent>offLimit && !digitalRead(Relay1)){
    R1State = 0;
    digitalWrite(Relay1,!R1State);
  }
  if(percent<onLimit && digitalRead(Relay1)){
    R1State = 1;
    digitalWrite(Relay1,!R1State);
  }
  
  //json empty String
  String jsonString ="";
  
  //Json container (200Byte)
  StaticJsonDocument<200>doc;
  
  //Json object name object
  JsonObject object = doc.to<JsonObject>();
  
  //insert value to objects
  object["counter"] = counter;
  object["rawDist"] = String(rawDist,2);
  object["wDistance"] = String(distance,2);
  object["waterLevel"] = String(waterLevel,2);
  object["percent"] = percent;
  object["volume"] = String(volume,2);
  object["relay1state"] = R1State;
  object["relay2state"] = R2State;
  object["pMode"] = pMode;
  
  //serialize Json
  serializeJson(doc,jsonString);
  
  //Serial.println(jsonString);
  
  //send jsonString over webSockets
  webSocket.broadcastTXT(jsonString);
  if(counter>6000)counter=0;
}


double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}
