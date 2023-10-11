#include <ModbusMaster.h>
#include <Smoothed.h>

#define DIS     8 
#define HEATER  9
#define CDU    10
#define AHU    11
#define XY_ID   1
#define HMI_ID  2

ModbusMaster hmi;
ModbusMaster xy;
Smoothed <unsigned int> adcIn1; 
Smoothed <unsigned int> adcIn2;

void setup() {
  Serial.begin(9600);
  pinMode(HEATER, OUTPUT);
  pinMode(CDU, OUTPUT);
  pinMode(AHU, OUTPUT);

  adcIn1.begin(SMOOTHED_AVERAGE, 10);
  adcIn2.begin(SMOOTHED_AVERAGE, 10);
  adcIn1.clear();
  adcIn2.clear();
  
  xy.begin(XY_ID, Serial);
  hmi.begin(HMI_ID, Serial);
}

void loop() {
  uint8_t result;
  uint16_t temp,humi,setHumi,setPressure;
  boolean onCoil;

  unsigned int ps1 = analogRead(A0);
  unsigned int ps2 = analogRead(A1);
  unsigned int ps3 = analogRead(A2);
  unsigned int ps4 = analogRead(A3);
  
  adcIn1.add(ps1);
  adcIn2.add(ps2);
  uint16_t pressure1 = adcIn1.get();
  uint16_t pressure2 = adcIn2.get();
  uint16_t pressure4 = ps3;
  uint16_t pressure5 = ps4;
  
  
  uint16_t mPressure1 = map(pressure1,0,1024,0,500);
  uint16_t mPressure2 = map(pressure1,0,1024,0,100);
  uint16_t mPressure3 = map(pressure2,0,1024,0,500);
  uint16_t mPressure4 = map(pressure4,0,1024,0,50);
  uint16_t mPressure5 = map(pressure5,0,1024,0,50);
  uint16_t mPressure6 = (pressure1 + (pressure4 - 25));
  uint16_t mPressure7 = (pressure2 + (pressure5 - 25));
  

  xy.clearResponseBuffer();
  result = xy.readInputRegisters(1,2);
  delay(30);
  if(result == xy.ku8MBSuccess){
    temp=xy.getResponseBuffer(0);
    humi=xy.getResponseBuffer(1)/10;
  }
   
  hmi.clearResponseBuffer();
  result = hmi.readCoils(0, 1);
  if(result == hmi.ku8MBSuccess){
    onCoil = hmi.getResponseBuffer(0);
  }
  
  hmi.clearResponseBuffer();
  result = hmi.readHoldingRegisters(8, 4);
  delay(70);
  if(result == hmi.ku8MBSuccess){
    setHumi=hmi.getResponseBuffer(0);
    setPressure=hmi.getResponseBuffer(2);
  }
  if(onCoil==1){
    digitalWrite(AHU,HIGH);
    digitalWrite(CDU,HIGH);
    if(humi>=(setHumi+5)) digitalWrite(HEATER,HIGH);
    if(humi<=(setHumi-5)) digitalWrite(HEATER,LOW);
  }else{
    digitalWrite(AHU,LOW);
    digitalWrite(CDU,LOW);
    digitalWrite(HEATER, LOW);
  }
  hmi.clearTransmitBuffer();
  if(mPressure3>setPressure){
    hmi.writeSingleCoil(1, 1);
  }else{
    hmi.writeSingleCoil(1, 0);
  }
  hmi.clearTransmitBuffer();
  hmi.setTransmitBuffer(0, mPressure6);
  hmi.setTransmitBuffer(2, mPressure7);
  hmi.setTransmitBuffer(4, temp);
  hmi.setTransmitBuffer(6, humi);
  hmi.writeMultipleRegisters(0,8);
  delay(900);
}
