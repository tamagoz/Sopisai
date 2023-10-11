#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <Smoothed.h>
#include <ModbusMaster.h>
#include <EEPROM.h>

/** the current address in the EEPROM (i.e. which byte we're going to write to next) **/
int addr = 0;

#define Y0 8
#define Y1 9
#define Y2 10
#define Y3 11

#define I0 4
#define I1 5
#define I2 6
#define I3 7

ModbusMaster node;
SoftwareSerial nextion(2, 3); // RX, TX

Smoothed <unsigned int> adcVal1;
Smoothed <unsigned int> adcVal2;

int intAvPressure1;
int intAvPressure2;
String strCommand;
String temp;
String humi;
unsigned long uvTime = 0;
boolean onUV = false;
unsigned long preMillis = 0;
unsigned long preReadTemp = 0;
unsigned int intTemp;
unsigned int intHumi;
unsigned int TempSett = 24;
unsigned int HumiSett = 55;

void setup() {

  pinMode(Y0, OUTPUT);
  pinMode(Y1, OUTPUT);
  pinMode(Y2, OUTPUT);
  pinMode(Y3, OUTPUT);

  pinMode(I0, INPUT);
  pinMode(I1, INPUT);
  pinMode(I2, INPUT);
  pinMode(I3, INPUT);

  Serial.begin(9600);
  nextion.begin(9600);

  initAdcVal();
  node.begin(1, Serial);
  wdt_enable(WDTO_4S);
  
  EEPROM.get(0, uvTime);
}

void loop() {
  wdt_reset();

  //Serial.print(intAvPressure1);
  unsigned long curMillis = millis();
  if (onUV) {
    if (curMillis - preMillis > 3600000) {
      uvTime++;
      EEPROM.put(0, uvTime);
      preMillis = curMillis;
    }
  }

  if (curMillis - preReadTemp > 2000) {
    readTemp();
    readAdc();
    preReadTemp = curMillis;
    
  }

  if (nextion.available()) {
    char c = nextion.read();    
    switch (c) {
      case 'r':
        updateUI();
        break;
      case '1':
        digitalWrite(Y0, HIGH);
        digitalWrite(Y1, HIGH);
        digitalWrite(Y2, HIGH);
        digitalWrite(Y3, HIGH);
        break;
      case '2':
        digitalWrite(Y0, HIGH);
        digitalWrite(Y1, HIGH);
        digitalWrite(Y2, HIGH);
        digitalWrite(Y3, LOW);
        break;
      case '3':
        digitalWrite(Y0, LOW);
        digitalWrite(Y1, HIGH);
        digitalWrite(Y2, HIGH);
        digitalWrite(Y3, HIGH);
        break;
      case '4':
        digitalWrite(Y0, LOW);
        digitalWrite(Y1, HIGH);
        digitalWrite(Y2, HIGH);
        digitalWrite(Y3, LOW);
        break;

      case '0':
        digitalWrite(Y0, LOW);
        digitalWrite(Y1, LOW);
        digitalWrite(Y2, LOW);
        digitalWrite(Y3, LOW);
        break;
    }
  }//end read nextion
}


void readTemp() {
  uint8_t result;
  uint16_t data[2]; // prepare variable of storage data from sensor
  
  result = node.readInputRegisters(1, 2); // Read 2 registers starting at 1)
   // intTemp = node.getResponseBuffer(0) / 10.0f;
   // intHumi = node.getResponseBuffer(1) / 10.0f;    
  if (result == node.ku8MBSuccess) {   
    intTemp = node.getResponseBuffer(0) / 10.0f;
    intHumi = node.getResponseBuffer(1) / 10.0f;   
  }else{
    intTemp = 1;
    intHumi = 1;
  }
  
}

void initAdcVal() {
  adcVal1.begin(SMOOTHED_AVERAGE, 10);
  adcVal2.begin(SMOOTHED_AVERAGE, 10);
}

void readAdc() {
  unsigned int tmp1 = analogRead(A0);
  unsigned int tmp2 = analogRead(A1);
  adcVal1.add(tmp1);
  adcVal2.add(tmp2);
  intAvPressure1 = map(adcVal1.get(), 0, 1024, -50, 50);
  intAvPressure2 = map(adcVal2.get(), 0, 1024, -50, 50);
  //intAvPressure1 = (intAvPressure1 + (-7));
  //intAvPressure2 = (intAvPressure2 + (-4));
}

void updateUI() {
  String tmp = "va2.val=" + String(intAvPressure1);
  printNextion(tmp);
  tmp = "va3.val=" + String(intAvPressure2);
  printNextion(tmp);
  //tmp = "t0.txt=\"25\"";
  tmp = tmp = "va0.val=" + String(intTemp);
  printNextion(tmp);
  //tmp = "t1.txt=\"50\"";
  tmp = tmp = "va1.val=" + String(intHumi);
  printNextion(tmp);
  tmp = "n0.val=0" + String(uvTime);
  printNextion(tmp);
  tmp = "n1.val=" + String(uvTime);
  printNextion(tmp);
  if(!digitalRead(I0)||intAvPressure2>440){
    tmp = "p4.pic=18";
    printNextion(tmp);
  }else{
    tmp = "p4.pic=15";
    printNextion(tmp);
  }
}

void updateUI1() {
  String tmp = "t2.txt=\"0\"";
  printNextion(tmp);
  tmp = "t3.txt=\"0\"";
  printNextion(tmp);
  tmp = "t0.txt=\"0\"";
  printNextion(tmp);
  tmp = "t1.txt=\"0\"";
  printNextion(tmp);
  tmp = "n0.val=0" + String(uvTime);
  printNextion(tmp);
  tmp = "n1.val=" + String(uvTime);
  printNextion(tmp);
}

void printNextion(String tmp) {
  nextion.print(tmp);
  nextion.write(0xff);
  nextion.write(0xff);
  nextion.write(0xff);
}
