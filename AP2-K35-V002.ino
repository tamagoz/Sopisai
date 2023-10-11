/*
  Modbus-Arduino Example - Lamp (Modbus Serial)
  Copyright by André Sarmento Barbosa
  http://github.com/andresarmento/modbus-arduino
  https://github.com/emelianov/modbus-esp8266

  Project      Test_In_Out
  Description  Read digital input (Opto Isolator) then output to Relay directly

  Date         2021-04-01

  Hardware     AVR-AP2 (MCU=ATmega328, XTAL=16.0Mhz, Vcc=5.0V)
  I/O interface
    Relay output , Active high
    RELAY_0  = Pin 8
    RELAY_1  = Pin 9
    RELAY_2  = Pin 10
    RELAY_3  = Pin 11

    Digital input , Active low
    IN_0  = Pin 4
    IN_1  = Pin 5
    IN_2  = Pin 6
    IN_3  = Pin 7
******************************************************************************
  MICRO RESEARCH TECHNOLOGY CO.,LTD. , www.micro-research.co.th
******************************************************************************/

#include <Modbus.h>
#include <ModbusSerial.h>
#include <Wire.h>
#include <Smoothed.h>

// Define hardware I/O
#define RELAY_0  8
#define RELAY_1  9
#define RELAY_2  10
#define RELAY_3  11

#define IN_0    4
#define IN_1    5
#define IN_2    6
#define IN_3    7



// Used Pins ADC
const int analog_ISOLATE  = A0;  // Analog input pin that the potentiometer is attached to
const int analog_ANTE     = A1;  // Analog input pin that the potentiometer is attached to
const int ISOLATE_Cal     = A2;  // Analog input pin that the potentiometer is attached to
const int ANTE_Cal        = A3;  // Analog input pin that the potentiometer is attached to

Smoothed <float> mySensor;
Smoothed <float> mySensor2;

ModbusSerial mb;

float currentSensorValue;
float currentSensorValue1;
int currentSensorValue2;
int currentSensorValue3;

float smoothedSensorValueAvg;
float smoothedSensorValueExp;

float outputValue1;        // value read from the po
float outputValue2;        // value read from the po

int Int1 = 0;
int Int2 = 0;
int Int3 = 0;
int Int4 = 0;

int Int1_o = 0;
int Int2_o = 0;
int Int3_o = 0;
int Int4_o = 0;

int incomingByte = 0;      // for incoming serial data

unsigned long lastGetI2CSensorTime = 0;
unsigned long ts = 0;

// Modbus Registers Offsets (0-9999)
const int LAMP1_COIL = 100;
const int LAMP2_COIL = 101;
const int LAMP3_COIL = 102;
const int LAMP4_COIL = 103;

const int SWITCH1_ISTS = 100;
const int SWITCH2_ISTS = 101;
const int SWITCH3_ISTS = 102;
const int SWITCH4_ISTS = 103;

const int SENSOR_IREG1 = 100;
const int SENSOR_IREG2 = 101;
const int SENSOR_IREG3 = 102;
const int SENSOR_IREG4 = 103;
const int SENSOR_IREG5 = 104;
const int SENSOR_IREG6 = 105;
const int SENSOR_IREG7 = 106;
const int SENSOR_IREG8 = 107;







void setup() {

  analogReference(DEFAULT);       // ADC Reference = +5V

  // Config Modbus Serial (port, speed, byte format)
  mb.config(&Serial, 9600, SERIAL_8N1);
  mb.setSlaveId(10);

  Serial.begin(9600);

  pinMode(RELAY_0, OUTPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);

  pinMode(IN_0, INPUT);
  pinMode(IN_1, INPUT);
  pinMode(IN_2, INPUT);
  pinMode(IN_3, INPUT);

  ts = millis();

  mySensor.begin(SMOOTHED_AVERAGE, 10);
  mySensor2.begin(SMOOTHED_EXPONENTIAL, 10);
  mySensor.clear();

  mb.addCoil(LAMP1_COIL);
  mb.addCoil(LAMP2_COIL);
  mb.addCoil(LAMP3_COIL);
  mb.addCoil(LAMP4_COIL);

  mb.addIsts(SWITCH1_ISTS);
  mb.addIsts(SWITCH2_ISTS);
  mb.addIsts(SWITCH3_ISTS);
  mb.addIsts(SWITCH4_ISTS);

  mb.addIreg(SENSOR_IREG1);
  mb.addIreg(SENSOR_IREG2);
  mb.addIreg(SENSOR_IREG3);
  mb.addIreg(SENSOR_IREG4);
  mb.addIreg(SENSOR_IREG5);
  mb.addIreg(SENSOR_IREG6);
  mb.addIreg(SENSOR_IREG7);
  mb.addIreg(SENSOR_IREG8);


}






void loop() {

  if (millis() - lastGetI2CSensorTime > 1000ul)
  {
    currentSensorValue = analogRead(analog_ISOLATE);
    currentSensorValue1 = analogRead(analog_ANTE);
    currentSensorValue2 = analogRead(ISOLATE_Cal);
    currentSensorValue3 = analogRead(ANTE_Cal);

    mySensor.add(currentSensorValue);
    mySensor2.add(currentSensorValue1);

    smoothedSensorValueAvg = mySensor.get();
    outputValue1 = smoothedSensorValueAvg;
    //outputValue1 = map(smoothedSensorValueAvg, 0, 4096, 0, 250);

    smoothedSensorValueExp = mySensor2.get();
    outputValue2 = smoothedSensorValueExp;
    //outputValue2 = map(smoothedSensorValueExp, 0, 4096, 0, 250);

    //Serial.print(outputValue1,0);
    //Serial.print(", ");
    //Serial.print(outputValue2,0);
    //Serial.print(", ");
    //Serial.print(currentSensorValue2);
    //Serial.print(", ");
    //Serial.println(currentSensorValue3);

    float lastValueStoredAvg = mySensor.getLast();
    float lastValueStoredExp = mySensor2.getLast();



    Int1 = digitalRead(IN_0);
    if (Int1 == 1) {
      Int1_o = 49;
    }
    else {
      Int1_o = 0;
    }
    Int2 = digitalRead(IN_1);
    if (Int2 == 1) {
      Int2_o = 50;
    }
    else {
      Int2_o = 0;
    }
    Int3 = digitalRead(IN_2);
    if (Int3 == 1) {
      Int3_o = 51;
    }
    else {
      Int3_o = 0;
    }
    Int4 = digitalRead(IN_3);
    if (Int4 == 1) {
      Int4_o = 52;
    }
    else {
      Int4_o = 0;
    }

    lastGetI2CSensorTime = millis();
  }




  mb.task();

  mb.Ists(SWITCH1_ISTS, digitalRead(IN_0));
  mb.Ists(SWITCH2_ISTS, digitalRead(IN_1)); 
  mb.Ists(SWITCH3_ISTS, digitalRead(IN_2));
  mb.Ists(SWITCH4_ISTS, digitalRead(IN_3));


  digitalWrite(RELAY_0, mb.Coil(LAMP1_COIL));
  digitalWrite(RELAY_1, mb.Coil(LAMP2_COIL));
  digitalWrite(RELAY_2, mb.Coil(LAMP3_COIL));
  digitalWrite(RELAY_3, mb.Coil(LAMP4_COIL));


  if (millis() > ts + 1000) {
    ts = millis();
    mb.Ireg(SENSOR_IREG1, (outputValue1));
    mb.Ireg(SENSOR_IREG2, (outputValue2));
    mb.Ireg(SENSOR_IREG3, (currentSensorValue2));
    mb.Ireg(SENSOR_IREG4, (currentSensorValue3));
    mb.Ireg(SENSOR_IREG5, (Int1_o));
    mb.Ireg(SENSOR_IREG6, (Int2_o));
    mb.Ireg(SENSOR_IREG7, (Int3_o));
    mb.Ireg(SENSOR_IREG8, (Int4_o));
  }
}



//=======================================================
//=======================================================
