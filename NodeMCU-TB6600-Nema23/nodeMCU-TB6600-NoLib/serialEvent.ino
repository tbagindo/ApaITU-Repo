void serialEvent(){
    if (Serial.available() > 0){
    char command = Serial.read();
    if(command == 's'){ //s1000 500 1000
      pulseSend = Serial.parseFloat();
      stepsDelay = Serial.parseFloat();
      delays = Serial.parseFloat();
      Serial.println("delays : " + String(stepsDelay * 2) + "uS");
      //Serial.println("revs : " + String(revs));      
    }
    if(command == 'r'){ //r3 500 1000
      revs = Serial.parseFloat();
      pulseSend = 800 * revs;
      stepsDelay = Serial.parseFloat();
      delays = Serial.parseFloat();
      Serial.println("delays : " + String(stepsDelay * 2) + "uS");
      Serial.println("revs : " + String(revs));      
    }
    if(command == 'e'){ //r1000 500
      enState=1;
    digitalWrite(ENPIN,enState);   
      Serial.println("Enable : " + String(enState));   
    }
    if(command == 'd'){ //r1000 500
      enState=0;
    digitalWrite(ENPIN,enState);   
      Serial.println("Enable : " + String(enState));     
    }
  }

}
