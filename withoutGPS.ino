#include <math.h>
#include <Sun.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
#include <DS1302.h>
#include <Wire.h>
#include <BH1750.h>
// MACRO CONSTANT
#define NAME "rain.txt"
#define PERIOD 60 // MINUTE
#define WAIT_TIME 1000 // MS scale
#define SERIAL_BUS 115200
#define LEN 51
// MACRO CONSTANT FOR MOTOR
#define PIN_BODY 7
#define PIN_HEAD 6
// MACRO CONSTANT FOR SD
#define MOSI 11
#define SCK_SD 13
#define MISO 12
#define CS 4
// MACRO CONSTANT FOR RTC
#define CLK 10  // SCK_PIN
#define DAT 9   // IO_PIN
#define RST 8   // RST_PIN
// MACRO CONSTANT FOR LIGHTMETER
#define SCL A5
#define SDA A4

DS1302 rtc(RST, DAT, CLK);
Sun s;
Time t = rtc.time();
File myFile;
Servo body, head;
BH1750 lmt;

void setSD() {
  if(SD.begin(CS)) {
    Serial.println(F("SD open!"));
  } else {
    Serial.println(F("SD fail!"));
    return;
  }
  myFile = SD.open(NAME, FILE_WRITE);
  if(myFile) {
    Serial.println(F("pointer open successfully!"));
    myFile.println("myFile open");
  } else {
    Serial.println(F("pointer open fail!"));
  }
}

void setup() {
  Serial.begin(SERIAL_BUS);
  s.setLoc(126.9519, 37.4598);
  setSD();
  lmt.begin();
  body.attach(PIN_BODY);
  head.attach(PIN_HEAD);
  Serial.println(F("Setup end!"));
  delay(WAIT_TIME);
}

void loop() {
  static int cnt = 0;
  cnt++;
  Serial.print(cnt); Serial.println("'s loop!");
  if(cnt > PERIOD * 60) {
    myFile.close();
    Serial.println(F("loop end!"));
    while(1) {
      delay(WAIT_TIME);
    }
  }

  t = rtc.time();
  s.setTime(t.yr, t.mon, t.date, t.hr, t.min, t.sec);
  s.calcPos();

  body.write(s.azimuth);
  head.write(s.altitude);

  Serial.print(s.azimuth); Serial.print(", ");
  Serial.println(s.altitude);

  uint16_t lux = lmt.readLightLevel();
  char buf[LEN];
  snprintf(buf, sizeof(buf),
           "%04d,%02d,%02d,%02d,%02d,%02d,",
           t.yr, t.mon, t.date, t.hr, t.min, t.sec);
  Serial.print(buf); Serial.print(lux); Serial.println(" lx");
  myFile.print(buf); myFile.println(lux);

  delay(WAIT_TIME);
}
