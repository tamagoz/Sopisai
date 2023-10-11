/*
Chumpee Hospital
02/Jan/2023
Operation Room 5 - 6
*/

#include <ModbusMaster.h>
#include <Smoothed.h>

#define DIS 8
#define HEATER 10
#define CDU 11
#define AHU 9
#define XY_ID 1
#define HMI_ID 2

ModbusMaster hmi;

ModbusMaster xy;
Smoothed<unsigned int> adcIn1;
Smoothed<unsigned int> adcIn2;
Smoothed<unsigned int> adcIn3;
Smoothed<unsigned int> adcIn4;

void setup() {
  Serial.begin(9600);

  pinMode(HEATER, OUTPUT);
  pinMode(CDU, OUTPUT);
  pinMode(AHU, OUTPUT);
  pinMode(DIS, OUTPUT);

  adcIn1.begin(SMOOTHED_AVERAGE, 10);
  adcIn2.begin(SMOOTHED_AVERAGE, 10);
  adcIn3.begin(SMOOTHED_AVERAGE, 10);
  adcIn4.begin(SMOOTHED_AVERAGE, 10); 
  adcIn1.clear();
  adcIn2.clear();
  adcIn3.clear();
  adcIn4.clear();

  xy.begin(XY_ID, Serial);
  hmi.begin(HMI_ID, Serial);
}

void loop() {
  uint8_t result;
  uint16_t temp, humi, setHumi, setPressure;
  uint16_t temp1, humi1;
  boolean onCoil;

  unsigned int ps1 = analogRead(A0);
  unsigned int ps2 = analogRead(A1);
  unsigned int ps3 = analogRead(A2);
  unsigned int ps4 = analogRead(A3);


  adcIn1.add(ps1);
  adcIn2.add(ps2);
  adcIn3.add(ps3);
  adcIn4.add(ps4);
  uint16_t pressure1 = adcIn1.get();
  uint16_t pressure2 = adcIn2.get();
  uint16_t pressure3 = adcIn3.get();
  uint16_t pressure4 = adcIn4.get();

  uint16_t mPressure3 = map(pressure2, 0, 1024, 0, 500);    //Room
  uint16_t mPressure4 = map(pressure1, 0, 1024, 0, 500);    //AHU
  uint16_t mPressure5 = map(pressure1, 0, 1024, 0, 250);    //ACH
  uint16_t mpressure6 = map(pressure3, 0, 1024, 0, 100);    //Offset Room
  uint16_t mpressure7 = map(pressure4, 0, 1024, 0, 100);    //Offset ACH 
  uint16_t mPressure1 = (mPressure3 + (mpressure6 - 50));   //Room
  uint16_t mPressure2 = (mPressure3 + (mpressure7 - 50));   //ACH


  xy.clearResponseBuffer();
  result = xy.readInputRegisters(1, 2);
  delay(30);
  if (result == xy.ku8MBSuccess) {
    temp  = xy.getResponseBuffer(0);
    //temp  = temp + 1;
    humi = xy.getResponseBuffer(1) / 10;
    //humi  = humi + 1;
  }

  hmi.clearResponseBuffer();
  result = hmi.readCoils(0, 1);
  if (result == hmi.ku8MBSuccess) {
    onCoil = hmi.getResponseBuffer(0);
  }

  hmi.clearResponseBuffer();
  result = hmi.readHoldingRegisters(8, 4);
  delay(70);
  if (result == hmi.ku8MBSuccess) {

    setHumi = hmi.getResponseBuffer(0);
    setPressure = hmi.getResponseBuffer(2);
  }
  if (onCoil == 1) {
    digitalWrite(DIS, HIGH);
    digitalWrite(AHU, HIGH);
    //digitalWrite(CDU, HIGH);
    int settemp = 20;
    if (temp >= (settemp + 1)) digitalWrite(CDU, HIGH);
    if (temp <= (settemp - 1)) digitalWrite(CDU, LOW);
    if (humi >= (setHumi + 5)) digitalWrite(HEATER, HIGH);
    if (humi <= (setHumi - 5)) digitalWrite(HEATER, LOW);
    
  } else {
    digitalWrite(DIS, LOW);
    digitalWrite(AHU, LOW);
    digitalWrite(CDU, LOW);
    digitalWrite(HEATER, LOW);

    temp = 0;
    humi = 0;
    mPressure1 = 0;
    mPressure2 = 0;
  }
  hmi.clearTransmitBuffer();
  if (mPressure3 > setPressure) {
    hmi.writeSingleCoil(1, 1);
  } else {
    hmi.writeSingleCoil(1, 0);
  }
  hmi.clearTransmitBuffer();
  hmi.setTransmitBuffer(0, mPressure1);
  hmi.setTransmitBuffer(2, mPressure2);
  hmi.setTransmitBuffer(4, temp);
  hmi.setTransmitBuffer(6, humi);
  hmi.writeMultipleRegisters(0, 8);
  delay(900);
}
