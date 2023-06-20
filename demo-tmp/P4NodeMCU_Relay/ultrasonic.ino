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

float wLevel(float dist){
  float f = sensorH - dist;
  return f;
}

float wVolume(float level){
  float f = LpCM * level;
  return f;
}



void sendServerData(){
  float distance = distanceCM(trigger,echo);
  distance = round2(distance);
  float waterLevel = wLevel(distance);
  waterLevel= round2(waterLevel);
  float volume = wVolume(waterLevel);
  volume = round2(volume);
  
  //json empty String
  String jsonString ="";
  
  //Json container (200Byte)
  StaticJsonDocument<200>doc;
  
  //Json object name object
  JsonObject object = doc.to<JsonObject>();
  
  //insert value to objects
  object["wDistance"] = distance;
  object["waterLevel"] = waterLevel;
  object["percent"] = waterLevel;
  object["volume"] = volume;
  object["relay1state"] = R1State;
  object["relay2state"] = R2State;
  object["pMode"] = pMode;
  
  //serialize Json
  serializeJson(doc,jsonString);
  
  Serial.println(jsonString);
  
  //send jsonString over webSockets
  webSocket.broadcastTXT(jsonString);
}


double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}
