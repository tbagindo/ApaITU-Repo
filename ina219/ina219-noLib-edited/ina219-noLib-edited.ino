#include  <Wire.h>

#define CONF_REG 0x00
#define SHUNT_REG 0x01
#define BUS_REG 0x02
#define POWER_REG 0x03
#define CURRENT_REG 0x04
#define CALIBRATION_REG 0x05

uint16_t calValue = 13653;
uint16_t config =0x2000 | 0x0800 | 0x0180 | 0x0018 | 0x07;
uint16_t powerMultiplier_mW = 40;
float currentDivider_mA = 0.5;
float shuntLSB_mV = 0.01; 
uint16_t busLSB_mV = 4;


uint16_t readReg(uint8_t reg){
  uint16_t data =0;
  uint8_t hData = 0;
  uint8_t lData = 0;

  Wire.beginTransmission(0x40);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(0x40, static_cast<uint8_t>(2));
  if(Wire.available()){
    hData = Wire.read();       //High Byte
    lData = Wire.read();   //Low Byte  
  }
  data = (hData<<8) + lData;
  return data;
}

uint8_t writeReg(uint8_t reg, uint16_t data){
  uint8_t lData = data & 255;
  uint8_t hData = data >> 8;
  Wire.beginTransmission(0x40);
  Wire.write(reg);
  Wire.write(hData);
  Wire.write(lData);
  return Wire.endTransmission();
}
void setup() {
  Serial.begin(115200);
  Serial.println();
  Wire.begin();
  writeReg(0x00,config);
  writeReg(0x05,13653);

}

void loop() {

  int16_t shunt_raw = 0;
  uint16_t bus_raw = 0;
  uint16_t power_raw = 0;
  int16_t current_raw = 0;

  shunt_raw = readReg(0x01);
  bus_raw = (readReg(0x02)>>3);
  power_raw = readReg(0x03);
  current_raw = readReg(0x04);

  Serial.println("shunt_raw \t : " + String(shunt_raw));
  Serial.println("bus_raw \t : " + String(bus_raw));
  Serial.println("current_raw \t : " + String(current_raw)); 
  Serial.println("power_raw \t : " + String(power_raw));  
 
  Serial.println();
  Serial.println("V Shunt \t : " + String(shunt_raw * shuntLSB_mV) + " mV");
  Serial.println("V Bus \t\t : " + String(bus_raw * busLSB_mV * 0.001) + " V"); 
  Serial.println("V Load \t\t : " + String((shunt_raw * shuntLSB_mV) + (bus_raw * busLSB_mV)) + " mV"); 
  Serial.println("Current \t : " + String(current_raw / currentDivider_mA) + " mA");
  Serial.println("Power \t\t : " + String(power_raw * powerMultiplier_mW) + " mW"); 
  Serial.println("============================================");
  delay(3000);
}
