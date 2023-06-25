void motorInit(){
  stepper.setMaxSpeed(2000);
  stepper.setAcceleration(1000);
  stepper.setEnablePin(ENPIN);
  stepper.disableOutputs();
}

void bouncing(){
  //save last position and reset current position
  lPos = aPos;
  cPos =0;
  Serial.print("POS: ");
  Serial.println(stepper.currentPosition());  
  stepper.setCurrentPosition(0);
  Serial.println("Stop and Start counter direction");
  stepper.stop(); //stop motor
  stepper.setMaxSpeed(reqSpeed); //set speed
  step = -1 * step;
  stepper.move(step); //set distance
}

void mStop(){
  rFlag = false;
  mState = 0;
  bState=0;
  //save last position and reset current position
  lPos = aPos;
  cPos =0;
  Serial.print("POS: ");
  Serial.println(stepper.currentPosition());  
  stepper.setCurrentPosition(0);
  Serial.println("STOP ");
  stepper.stop(); //stop motor
  stepper.disableOutputs();
}

void motorHandler(){
  if (rFlag){
    mState = 1;
    cPos = stepper.currentPosition();
    aPos = lPos + cPos;
    if (abs(cPos) < reqSteps){
      mState = 1;
      stepper.enableOutputs();
      stepper.run();
    }else{ // steps done
      if(bFlag){
        bouncing();
      }else{
        mStop(); 
      }
    }
  }else{
    mState=0;
    return;
  }
}
 
void serialHandler(){  
  if (Serial.available() > 0){
    comChar = Serial.read();
    
    if (comChar == 'r'){ //r[steps] [speed]
      rFlag = true; 
      reqSteps = Serial.parseFloat(); // Get Requested Steps number
      reqSpeed = Serial.parseFloat(); // Get Requested Speed in Steps/seconds
 
      Serial.print(reqSteps);
      Serial.print(reqSpeed);
      Serial.println("Run Stepper Motor cw");
      stepper.setMaxSpeed(reqSpeed);
      stepper.move(reqSteps);
    }
    
    if (comChar == 'd'){ //d[steps] [speed]
      rFlag = true; 
      reqSteps = Serial.parseFloat(); // Get Requested Steps number
      reqSpeed = Serial.parseFloat(); // Get Requested Speed in Steps/seconds

      Serial.println("Run Stepper Motor cw");
      stepper.setMaxSpeed(reqSpeed);
      stepper.move(-1*reqSteps);
    }
    
    if (comChar == 's'){ // Stop !!!
      mStop();
    }
    if(!rFlag){
      if (comChar == 'a'){ //a[accel] in Steps/seconds"     
        reqAccel = Serial.parseFloat(); 
        stepper.setAcceleration(reqAccel); 
        Serial.println("new accel : " + String(reqAccel) + "steps/seconds\"");
      }
      if (comChar == 'z'){ //restart
        Serial.print("Reboot ");
        for(int i=0; i<10; i++){
          Serial.print(".");
          delay(300);
        }
        ESP.restart();
      }
    }
  }
}
