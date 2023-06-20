/**

void opMode(){
  if(pMode){
    if(percent<minLevel)pState =  HIGH;
    if(percent>maxLevel)pState = LOW;
  }else{
    
  }
}
*/
int pCent(){
  int p = (int) ((sensorH-distance)/maxLevel)*100;
  Serial.println("level : "  + String(p) +  "%");
  return p;
}
