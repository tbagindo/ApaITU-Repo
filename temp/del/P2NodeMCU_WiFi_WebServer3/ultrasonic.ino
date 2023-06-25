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
