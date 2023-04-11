/*
 April 2023 ApaITU
 video    : NodeMCU & Ultrasonic #2
 Series   : Water tank Project 
 Sensor   : HC-SR04
 
 We will use hc-sr04 to measure water tank level, for example
 we have water tank
 capacity       : 500L
 Height         : 100cm 
 Sensor Height  : 115cm (from bottom of tank)

 by measuring the distance from sensor to water  surface we could calculate the water level
 waterLevel = SensorHeight - sensingDistance

 Based on waterLevel we could also measure the water volume, we need to calculate the volume/cm 
 VolumePerCM  = capacity/100cm
              = 500L/100cm
              = 5L/cm

 Volume       = waterLevel * VolumePerCM
 for example we read waterLevel = 10cm

 Volume       = 10cm * 5L/cm
              = 50L

 Who ultrasonic works?
 Ultrasonic act like a radar, measuring the distance by  sending ultrasonic sound wave (40Khz) 
 and timing the wave travel since it sended from sensor, reflecting from the object and traveling back 
 to the sensor.

 Sound wave has speed of 340m/s if we multiple it with the time (duration) we get the distance value. 
 We have take into account the measuring time is twice the distance.

 The formula :
 distance     = 0.5 * duration * 340m/s

 Typically HC-SR04 measuring the traveltime in uS or 1 of million of seconds and calculate the distance
 in cm, we need to transform the unit from m/s to cm/uS
 SpeedOfSound_CMuS  = 340m * 100cm/m / (1S * 1000000uS/S)
                    = 34000cm/1000000uS
                    = 0.034 cm/uS

 how microController get information from HC-SR04?
 HC-SR04 has alot of variant, working on 3.3V, 5V i2c protocol or simple with trigger and echo pin.
 We use 4 pin with echo and trigger pins
 vcc      >> 3.3V
 gnd      >> gnd
 trigger  >> D5
 echo     << D6

 1)  resetting the sensor
 we need to send signal HIGH for  2ms to sensor. this sequence will reset the sensor
 2) triggering the sensor for start sending sound wave :
 send singal HIGH for 10ms
 3) timing the  HIGH signal comming from echo pin

 float distanceCM(int triggerPin, int echoPin);
 long durationUS(int triggerPin, int echoPin);
 
 */

//pins
const int trigger = D5;
const int echo = D6;
float sow = 0.034; // 0.034 cm/uS

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Ultrasonic  & NodeMCU");

  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);

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

  float distance = 0.5 * durationUS(triggerPin,echoPin) * sow;
  Serial.println("Distance : " + String(distance) + " cm");
}

void loop() {
  distanceCM(trigger,echo);
  delay(1000);
 
}
