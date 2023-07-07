#define PULSEPIN D5
#define DIRPIN D6
#define ENPIN D7


int PulsePerRev = 800;                // 4 microstes & 200Steps/rev
int revs = 3;                         // requested Revolutions
int pulseSend = PulsePerRev * revs ;  // 2400 pulses send to the TB6600 => 3 revolution
uint32_t stepsDelay=500;              // 500 microseconds delay * 2 = 1ms delay between pulse
uint32_t delays =1000;

int dir=0;
int enState = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
 
  pinMode(PULSEPIN,OUTPUT);
  pinMode(DIRPIN,OUTPUT);
  pinMode(ENPIN,OUTPUT);
  digitalWrite(ENPIN,enState);

}

void loop() {
  Serial.println("Start sending Pulse");
  for(int i =0; i<pulseSend; i++){
    digitalWrite(PULSEPIN,HIGH);
    delayMicroseconds(stepsDelay);
    digitalWrite(PULSEPIN,LOW);
    delayMicroseconds(stepsDelay);
    yield();
  }
  Serial.println("Done !!! Delay 1 Seconds");
  delay(delays);
  
  Serial.println("Change Dir : " + String(dir));
  dir = !dir;
  digitalWrite(DIRPIN,dir);
  serialEvent();
}
