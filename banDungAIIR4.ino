#include <Smoothed.h>
#include <ShiftRegister74HC595.h>
#include <SoftwareSerial.h>
#include <esp_task_wdt.h>
#include <ModbusMaster.h>
#include "EEPROM.h"

#define ADC1 (33)
#define ADC2 (32)

#define IN1   (36)
#define IN2   (39)
#define IN3   (34)
#define IN4   (35)

SoftwareSerial nextion(19, 18);//rx,tx

Smoothed <unsigned int> adcVal1;
Smoothed <unsigned int> adcVal2;

ShiftRegister74HC595<1> output1(25, 13, 26);

unsigned int intAvPressure1;
unsigned int intAvPressure2;
unsigned int intTemp;
unsigned int TempSett = 23;
unsigned int intHumi;
unsigned long preReadTemp = 0;
unsigned long liftTime = 0;
unsigned long preTimer = 0;
boolean flagRun = false;

String strNextionRec;
ModbusMaster node;

uint8_t pinValues[] = { B00001010 };
uint8_t onMode1[] = { B00001101 };// out4, out3, out2, out1
uint8_t onMode2[] = { B00001001 };
uint8_t onMode3[] = { B00001110 };
uint8_t onMode4[] = { B00001010 };
uint8_t onMode5[] = { B00001011 };
uint8_t onMode6[] = { B00001100 };


void setup() {
  // put your setup code here, to run once:
  pinMode(IN1, INPUT);
  pinMode(IN2, INPUT);
  pinMode(IN3, INPUT);
  pinMode(IN4, INPUT);
  Serial.begin(9600);
  Serial2.begin(9600);
  node.begin(1, Serial2);
  nextion.begin(9600);
  initAdcVal();
  output1.setAllLow();
  EEPROM.begin(10);
  esp_task_wdt_init(3, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  liftTime = EEPROM.readUInt(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  esp_task_wdt_reset();
  unsigned long currentMillis = millis();

  if (currentMillis - preReadTemp >= 300000) {
    preReadTemp = currentMillis;
    readTemp();
  }
  if (currentMillis > preTimer + 3600000) {    
    liftTime++;
    //EEPROM.writeUInt(0, liftTime);
    EEPROM.put(0, liftTime);
    preTimer = currentMillis;
  }

  if (nextion.available()) {
    char c = nextion.read();
    switch (c) {
      case '0':
        //readAdcVal();

        intTemp = 0;
        intHumi = 0;
        intAvPressure1 = 0 ;
        intAvPressure2 = 0;

        updateUi();

        output1.setAllLow();
        break;
      case '1':
        readAdcVal();

        updateUi();
        if (intTemp < TempSett) {
          output1.setAll(onMode6);
          //delay(120000);
        }    
        else {
          output1.setAll(onMode5);
        }
        break;
      case '2':
        readAdcVal();

        updateUi();
        output1.setAll(onMode2);
        break;
      case '3':
        readAdcVal();

        updateUi();
        output1.setAll(onMode3);
        break;
      case '4':
        readAdcVal();

        updateUi();
        output1.setAll(onMode4);
        break;
    }
  }
}

void updateUi() {


  String tmp = "n2.val=" + String(intTemp);
  printNextion(tmp);
  tmp = "n3.val=" + String(intHumi);
  printNextion(tmp);
  tmp = "n4.val=" + String(intAvPressure1);
  printNextion(tmp);
  tmp = "n5.val=" + String(intAvPressure2);
  printNextion(tmp);
  tmp = "n0.val=" + String(liftTime);
  printNextion(tmp);
  tmp = "n1.val=" + String(liftTime);
  printNextion(tmp);
  boolean in1 = digitalRead(IN1);
  boolean in2 = digitalRead(IN2);
  boolean in3 = digitalRead(IN3);
  boolean in4 = digitalRead(IN4);
  if (in1 || in2 || in3 || in4) {
    //tmp = "p4.pic=15";
    //printNextion(tmp);
  } else {
    //tmp = "p4.pic=15";
    //printNextion(tmp);
  }

}

void initAdcVal() {
  adcVal1.begin(SMOOTHED_AVERAGE, 10);
  adcVal2.begin(SMOOTHED_AVERAGE, 10);
}

void readAdcVal() {
  unsigned int tmp1 = analogRead(ADC1);
  unsigned int tmp2 = analogRead(ADC2);
  adcVal1.add(tmp1);
  adcVal2.add(tmp2);

  intAvPressure1 = map(adcVal1.get(), 0, 4095, 0, -33);
  intAvPressure2 = map(adcVal1.get(), 0, 4095, 0, -33);

  intAvPressure1 = intAvPressure1 + (-6);
  intAvPressure2 = intAvPressure2 + (-1);

  Serial.println(intAvPressure1);
  Serial.println(intAvPressure2);

}

void readTemp() {
  uint8_t result;
  uint16_t data[2]; // prepare variable of storage data from sensor
  result = node.readInputRegisters(1, 2); // Read 2 registers starting at 1)
  if (result == node.ku8MBSuccess)
  {
    intTemp = node.getResponseBuffer(0) / 10.0f;
    intHumi = node.getResponseBuffer(1) / 10.0f;
    intTemp = intTemp - 3;
  }
}

void printNextion(String tmp) {
  nextion.print(tmp);
  nextion.write(0xff);
  nextion.write(0xff);
  nextion.write(0xff);
}
