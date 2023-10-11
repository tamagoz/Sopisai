//************************************************************
// this is a simple example that uses the painlessMesh library to
// setup a node that logs to a central logging node
// The logServer example shows how to configure the central logging nodes
//************************************************************

#include "painlessMesh.h"
#include <ShiftRegister74HC595.h>
#include <ModbusMaster.h>
#include <Smoothed.h>

ShiftRegister74HC595<1> sr(33, 13, 26);

ModbusMaster node;

// Used Pins ADC
const int analog_ISOLATE = 36;  // Analog input pin that the potentiometer is attached to
const int analog_ANTE    = 32;  // Analog input pin that the potentiometer is attached to

Smoothed <float> mySensor;
Smoothed <float> mySensor2;

float currentSensorValue;
float currentSensorValue1;
float smoothedSensorValueAvg;
float smoothedSensorValueExp;
int outputValue1;        // value read from the po
int outputValue2;        // value read from the po

//const int switchPin1 = 36;
//const int switchPin2 = 39;
//const int switchPin3 = 34;
//const int switchPin4 = 35;

//int switch1 = 0;
//int switch2 = 0;
//int switch3 = 0;
//int switch4 = 0;

long ts;


int incomingByte = 0; // for incoming serial data

unsigned long lastGetI2CSensorTime = 0;

#define   MESH_PREFIX     "mesh1234"
#define   MESH_PASSWORD   "12345678"
#define   MESH_PORT       5555

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

// Prototype
void receivedCallback( uint32_t from, String &msg );

size_t logServerId = 0;

// Send message to the logServer every 10 seconds
Task myLoggingTask(10000, TASK_FOREVER, []() {
#if ARDUINOJSON_VERSION_MAJOR==6
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject msg = jsonBuffer.to<JsonObject>();
#else
  DynamicJsonBuffer jsonBuffer;
  JsonObject& msg = jsonBuffer.createObject();
#endif
  msg["topic"] = "sensor";
  msg["value1"] = outputValue1;
  msg["value2"] = outputValue2;

  String str;
#if ARDUINOJSON_VERSION_MAJOR==6
  serializeJson(msg, str);
#else
  msg.printTo(str);
#endif
  if (logServerId == 0) // If we don't know the logServer yet
    mesh.sendBroadcast(str);
  else
    mesh.sendSingle(logServerId, str);

  // log to serial
#if ARDUINOJSON_VERSION_MAJOR==6
  serializeJson(msg, Serial);
#else
  msg.printTo(Serial);
#endif
  Serial.printf("\n");
});




void setup() {
  Serial.begin(115200);

  Serial2.begin(9600);
  while (!Serial2);
  node.begin(1, Serial2);

  //mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);

  // Add the task to the your scheduler
  userScheduler.addTask(myLoggingTask);
  myLoggingTask.enable();

  sr.set(0, HIGH); // set single pin HIGH
  sr.set(1, HIGH); // set single pin HIGH
  sr.set(2, HIGH); // set single pin HIGH
  sr.set(3, HIGH); // set single pin HIGH

  ts = millis();
  
  mySensor.begin(SMOOTHED_AVERAGE, 10);
  mySensor2.begin(SMOOTHED_EXPONENTIAL, 10);
  mySensor.clear();
}






void loop() {


  if (millis() - lastGetI2CSensorTime > 3000ul)                       // 3-Second
  {
    uint8_t result;
    uint16_t data[2]; // prepare variable of storage data from sensor

    result = node.readInputRegisters(1, 2); // Read 2 registers starting at 1)
    if (result == node.ku8MBSuccess)
    {
      Serial.print(node.getResponseBuffer(0) / 10.0f, 1);
      Serial.print(",");
      Serial.print(node.getResponseBuffer(1) / 10.0f, 1);
      Serial.println("");
    }

    // Read the value from the sensor
    currentSensorValue = analogRead(analog_ISOLATE);
    currentSensorValue1 = analogRead(analog_ANTE);

    // Add the new value to both sensor value stores
    mySensor.add(currentSensorValue);
    mySensor2.add(currentSensorValue1);

    // Get the smoothed values
    smoothedSensorValueAvg = mySensor.get();
    outputValue1 = smoothedSensorValueAvg;

    smoothedSensorValueExp = mySensor2.get();
    outputValue2 = smoothedSensorValueExp;

    Serial.print(outputValue1);
    Serial.print(",");
    Serial.println(outputValue2);

    lastGetI2CSensorTime = millis();
  }

  // it will run the user scheduler as well
  mesh.update();
}



void receivedCallback( uint32_t from, String &msg ) {
  //Serial.printf("logClient: Received from %u msg=%s\n", from, msg.c_str());
  Serial.printf(msg.c_str());

  // Saving logServer
#if ARDUINOJSON_VERSION_MAJOR==6
  DynamicJsonDocument jsonBuffer(1024 + msg.length());
  DeserializationError error = deserializeJson(jsonBuffer, msg);
  if (error) {
    Serial.printf("DeserializationError\n");
    return;
  }
  JsonObject root = jsonBuffer.as<JsonObject>();
#else
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(msg);
#endif
  if (root.containsKey("topic")) {
    if (String("logServer").equals(root["topic"].as<String>())) {
      // check for on: true or false
      logServerId = root["nodeId"];
      Serial.printf("logServer detected!!!\n");
    }
    Serial.printf(msg.c_str());
    //Serial.printf("\n");
  }
}

//=============================================================
