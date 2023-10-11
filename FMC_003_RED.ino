//Project FMC Relay 24 H

//unsigned long period1 = 43200000;  //ระยะเวลาที่ต้องการรอ
unsigned long period1 = 10000;       //ระยะเวลาที่ต้องการรอ
unsigned long last_time1 = 0;        //ประกาศตัวแปรเป็น global เพื่อเก็บค่าไว้ไม่ให้ reset จากการวนloop

//unsigned long period2 = 86400000;  //ระยะเวลาที่ต้องการรอ
unsigned long period2 = 60000;       //ระยะเวลาที่ต้องการรอ
unsigned long last_time2 = 0;        //ประกาศตัวแปรเป็น global เพื่อเก็บค่าไว้ไม่ให้ reset จากการวนloop

//unsigned long period3 = 43200000;  //ระยะเวลาที่ต้องการรอ
unsigned long period3 = 250;         //ระยะเวลาที่ต้องการรอ
unsigned long last_time3 = 0;        //ประกาศตัวแปรเป็น global เพื่อเก็บค่าไว้ไม่ให้ reset จากการวนloop

bool onCoil1 = false;
bool onCoil2 = false;
bool onCoil3 = false;

const int ledPin1 = 16;  // the pin that the LED is attached to
const int ledPin2 = 5;   // the pin that the LED is attached to
const int ledPin3 = 4;   // the pin that the LED is attached to
const int ledPin4 = 14;  // the pin that the LED is attached to
const int ledPin5 = 2;   // the pin that the LED is attached to



void setup() {
  Serial.begin(115200);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(ledPin5, OUTPUT);
}

void loop() {
  //Serial.println(last_time1);
  if (millis() - last_time1 > period1) {
    last_time1 = millis();  //เซฟเวลาปัจจุบันไว้เพื่อรอจนกว่า millis() จะมากกว่าตัวมันเท่า period
    onCoil1 = !onCoil1;
    Serial.println(onCoil1);
  }
  //Serial.println(last_time2);
  if (millis() - last_time2 > period2) {
    last_time2 = millis();  //เซฟเวลาปัจจุบันไว้เพื่อรอจนกว่า millis() จะมากกว่าตัวมันเท่า period
    onCoil2 = !onCoil2;
    Serial.print(onCoil2);
  }
  //Serial.println(last_time3);
  if (millis() - last_time3 > period3) {
    last_time3 = millis();  //เซฟเวลาปัจจุบันไว้เพื่อรอจนกว่า millis() จะมากกว่าตัวมันเท่า period
    onCoil3 = !onCoil3;
    Serial.print(onCoil3);
  }

  if (onCoil1 == HIGH) {
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, LOW);
    digitalWrite(ledPin4, LOW);
    //digitalWrite(ledPin5, HIGH);
  } else {
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, HIGH);
    digitalWrite(ledPin4, HIGH);
    //digitalWrite(ledPin5, LOW);
  }

  if (onCoil2 == HIGH) {
    //digitalWrite(ledPin1, HIGH);
    //digitalWrite(ledPin2, HIGH);
    //digitalWrite(ledPin3, HIGH);
    //digitalWrite(ledPin4, LOW);
    //digitalWrite(ledPin5, HIGH);
  } else {
    //digitalWrite(ledPin1, LOW);
    //digitalWrite(ledPin2, LOW);
    //digitalWrite(ledPin3, LOW);
    //digitalWrite(ledPin4, HIGH);
    //digitalWrite(ledPin5, LOW);
  }

  if (onCoil3 == HIGH) {
    //digitalWrite(ledPin1, HIGH);
    //digitalWrite(ledPin2, HIGH);
    //digitalWrite(ledPin3, HIGH);
    //digitalWrite(ledPin4, HIGH);
    digitalWrite(ledPin5, HIGH);
  } else {
    //digitalWrite(ledPin1, LOW);
    //digitalWrite(ledPin2, LOW);
    //digitalWrite(ledPin3, LOW);
    //digitalWrite(ledPin4, LOW);
    digitalWrite(ledPin5, LOW);
  }
}


//////////////////////////////////////END////////////////////////////////////