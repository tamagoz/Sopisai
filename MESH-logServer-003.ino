//************************************************************
// this is a simple example that uses the painlessMesh library to
// setup a single node (this node) as a logging node
// The logClient example shows how to configure the other nodes
// to log to this server
//************************************************************

#include "painlessMesh.h"

#define   MESH_PREFIX     "mesh1234"
#define   MESH_PASSWORD   "12345678"
#define   MESH_PORT       5555

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;
// Prototype
void receivedCallback( uint32_t from, String &msg );


// Send my ID every 10 seconds to inform others
Task logServerTask(10000, TASK_FOREVER, []() {
#if ARDUINOJSON_VERSION_MAJOR==6
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject msg = jsonBuffer.to<JsonObject>();
#else
  DynamicJsonBuffer jsonBuffer;
  JsonObject& msg = jsonBuffer.createObject();
#endif
  msg["topic"] = "logServer" + mesh.getNodeId();
  //msg["nodeId"] = mesh.getNodeId();

  String str;
  
#if ARDUINOJSON_VERSION_MAJOR==6
  serializeJson(msg, str);
#else
  msg.printTo(str);
#endif
  mesh.sendBroadcast(str);

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

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE | DEBUG ); // all types on
  //mesh.setDebugMsgTypes( ERROR | CONNECTION | SYNC | S_TIME );  // set before init() so that you can see startup messages
  //mesh.setDebugMsgTypes( ERROR | CONNECTION | S_TIME );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);

  mesh.onNewConnection([](size_t nodeId) {
    //Serial.printf("New Connection %u\n", nodeId);
  });

  mesh.onDroppedConnection([](size_t nodeId) {
    //Serial.printf("Dropped Connection %u\n", nodeId);
  });

  // Add the task to the your scheduler
  userScheduler.addTask(logServerTask);
  logServerTask.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();

  HowManyNode();

}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf(msg.c_str());
  Serial.printf("\n");
}

void HowManyNode() {
  unsigned long BLINK_PERIOD = 3000000;
  unsigned long BLINK_DURATION = 100000;
  int LED = 2;

  bool onFlag = true;
  uint32_t cycleTime = mesh.getNodeTime() % BLINK_PERIOD;

  for ( uint8_t i = 0; i < (mesh.getNodeList().size() + 1); i++ ) {
    uint32_t onTime = BLINK_DURATION * i * 2;

    if ( cycleTime > onTime && cycleTime < onTime + BLINK_DURATION )
      onFlag = false;
  }

  pinMode(LED, OUTPUT);
  digitalWrite(LED, onFlag);
}

//-------------------------------------------------------
