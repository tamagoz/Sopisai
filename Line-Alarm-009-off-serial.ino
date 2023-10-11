
//----------------------------------------------------------------------
//  https://www.praphas.com/forum/index.php?topic=356.0
//  https://github.com/TridentTD/TridentTD_LineNotify
//  Alarm Preatment RO Fresenius
//  24-02-2023


#include <WiFi.h>
#include <TridentTD_LineNotify.h>
#include <ShiftRegister74HC595.h>
//#include <esp_task_wdt.h>

#define SSID "GreatODev"
#define PASSWORD "1683vios"

//#define LINE_TOKEN  "1SM5jeZQTyCmpVA6Za9SHSl6C7mkKOFlXuPsuLL9Nfo"  //PreteatMent_Alarm
#define LINE_TOKEN "94gjTOIMpr8GzKNfEk3K24Q77N6qCqgUBoX1xab1Xnc"  //แจ้งเตือนระบบน้ำ RO

ShiftRegister74HC595<1> output1(33, 13, 26);
//ShiftRegister74HC595<1> output1(25, 13, 26);      //NEW

#define sensor1 36
#define sensor2 39
#define sensor3 34
#define sensor4 35

#define LED1 22
#define LED2 5
#define LED3 23

int buttonState1 = 0;  // variable for reading the pushbutton status
int buttonState2 = 0;  // variable for reading the pushbutton status
int buttonState3 = 0;  // variable for reading the pushbutton status
int buttonState4 = 0;  // variable for reading the pushbutton status

boolean flagRun = false;

unsigned long WaterDrop = 0;
unsigned long WaterLeak = 0;
unsigned long LineTests = 0;
unsigned long testloops = 0;

unsigned long previousMillis = 0;
unsigned long interval = 30000;

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  //Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    output1.set(2, LOW);
    output1.set(3, LOW);
    //Serial.print(".");
    //delay(500);
  }
  //Serial.println(WiFi.localIP());
}

void setup() {
  //Serial.begin(9600);

  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);

  output1.set(0, HIGH);  // set single pin HIGH
  output1.set(1, HIGH);  // set single pin HIGH
  output1.set(2, HIGH);  // set single pin HIGH
  output1.set(3, HIGH);  // set single pin HIGH

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);

  initWiFi();

  //esp_task_wdt_init(3, true);  //enable panic so ESP32 restarts
  //esp_task_wdt_add(NULL);      //add current thread to WDT watch

  LINE.setToken(LINE_TOKEN);

  // ส่งข้อความ
  //  LINE.notify("ถ้งน้ำดิบ ขนาด 1000 ลิตร หมด โปรดดำเนินการตรวจสอบโดยด่วน");
  //  LINE.notify("ระบบน้ำ OR System Leak มีน้ำรั่วที่พื้น โปรดทำการแก้ไขด่วน");
  //  LINE.notify("ทดสอบระบบ Line แจ้งเตือน โดยเจ้าหน้าที่");
  LINE.notify("Line Alarm แจ้งเตือน สถานะพร้อมทำงาน");
  //  LINE.notify("analogread.com");

  // ส่งตัวเลข
  //  int va = 25;
  //  float pi = 3.14159265359;
  //  float temp = 39.12345;
  //  LINE.notify(va);      // จำนวนเต็ม
  //  LINE.notify(pi, 4);   // ทศนิยม 4 หลัก

  // ส่งเป็นชุดข้อความ
  //  LINE.notify("Temp = "+String(temp,3)+" C");

  // ส่งสติกเกอร์
  //  LINE.notifySticker("อุณหภูมิสูง", 4, 274); // ส่ง Line Sticker ด้วย PackageID 1 , StickerID 2

  // ส่งรูปภาพ ด้วย url
  //  LINE.notifyPicture("https://cq.lnwfile.com/_/cq/_raw/0w/1o/gu.jpg");
}

void loop() {

  // put your main code here, to run repeatedly:
  //esp_task_wdt_reset();

  buttonState1 = digitalRead(sensor1);
  buttonState2 = digitalRead(sensor2);
  buttonState3 = digitalRead(sensor3);
  buttonState4 = digitalRead(sensor4);

  if (buttonState1 == LOW) {
    // turn LED on:
    output1.set(0, LOW);  // set single pin HIGH

    unsigned long currentMillis = millis();
    if (currentMillis - WaterDrop >= 60000) {
      WaterDrop = currentMillis;
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);

      LINE.notify("ถ้งน้ำดิบ ขนาด 1000 ลิตร หมด โปรดดำเนินการตรวจสอบโดยด่วน");
      //Serial.print("1");
    }
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    output1.set(0, HIGH);  // set single pin HIGH

    //LINE.notify("ถ้งน้ำดิบ ขนาด 1000 ลิตร ได้รับการแก้ไขแล้ว");
    //Serial.print("1");
  }

  if (buttonState2 == LOW) {
    // turn LED off:
    output1.set(1, LOW);  // set single pin HIGH
    delay(90000);
    unsigned long currentMillis = millis();
    if (currentMillis - WaterLeak >= 60000) {
      WaterLeak = currentMillis;
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      LINE.notify("ระบบน้ำ OR System Leak มีน้ำรั่วที่พื้น โปรดทำการแก้ไขด่วน");
      //Serial.print("2");
    }
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    output1.set(1, HIGH);  // set single pin HIGH

    //LINE.notify("ระบบน้ำ OR System Leak มีน้ำรั่วที่พื้น ได้รับการแก้ไขแล้ว");
    //Serial.print("2");
  }

  if (buttonState3 == LOW) {
    // turn LED off:
    unsigned long currentMillis = millis();
    if (currentMillis - LineTests >= 3000) {
      LineTests = currentMillis;

      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      output1.set(2, LOW);  // set single pin HIGH
      LINE.notify("Line Alarm ทดสอบระบบแจ้งเตือน สถานะพร้อมทำงาน");
      //Serial.print("3");
    }
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    output1.set(2, HIGH);  // set single pin HIGH

    //LINE.notify("ระบบน้ำ OR System Leak มีน้ำรั่วที่พื้น ได้รับการแก้ไขแล้ว");
    //Serial.print("2");
  }

  if (buttonState4 == LOW) {
    // turn LED off:
    unsigned long currentMillis = millis();
    if (currentMillis - testloops >= 3000) {
      testloops = currentMillis;

      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      output1.set(3, LOW);  // set single pin HIGH

      //LINE.notify("ลิฟท์หมายเลข 2 ขัดข้อง โปรดทำการแก้ไขด่วน");
      //Serial.print("4");
    }
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    output1.set(3, HIGH);  // set single pin HIGH

    //LINE.notify("ระบบน้ำ OR System Leak มีน้ำรั่วที่พื้น ได้รับการแก้ไขแล้ว");
    //Serial.print("2");
  }

  if (WiFi.status() != WL_CONNECTED) {
    output1.set(2, LOW);
    output1.set(3, LOW);
  } else {
    output1.set(2, HIGH);
    output1.set(3, HIGH);
  }

  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
    //Serial.print(millis());
    //Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}


//---------------ทดสอบการทำงาน------------------------
