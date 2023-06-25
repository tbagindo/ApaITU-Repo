void OperationMode(){
  if(pMode){
    if(percent < onLevel){
      if(R1State == 0){    
        R1State = 1;
        digitalWrite(Relay1,!R1State);
      }
    }
    if(percent > offLevel){
      if(R1State == 1){
        R1State = 0;
        digitalWrite(Relay1,!R1State);
      }
    }
  }
}

void tProtection(int maxProt, int minProt){
  if(percent > maxProt){ 
    R2State = 1; // cut the line hence master Relay (Relay 2)
    digitalWrite(Relay2,!R2State);
  }

  if(percent < minProt){
    R2State = 0; // reconnect the line hence master Relay (Relay 2)
    digitalWrite(Relay2,!R2State);
  }
}
