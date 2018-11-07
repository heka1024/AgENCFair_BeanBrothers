#include <math.h>
#include <Sun.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
#include <DS1302.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
// MACRO CONSTANT
#define SERIAL_BUS 115200
#define GPS_BUS 9600
#define WAIT_TIME 1000 // MS scale
#define LEN 51
// MACRO CONSTANT FOR MOTOR
#define PIN_BODY 9
#define PIN_HEAD 8
// MACRO CONSTANT FOR SD
#define MOSI 11
#define SCK_SD 13
#define MISO 12
#define CS 4
// MACRO CONSTANT FOR RTC
#define CLK 10  // SCK_PIN
#define DAT 9   // IO_PIN
#define RST 8   // RST_PIN
// MACRO CONSTANT FOR GPS
#define RX 3
#define TX 2
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
  if(!SD.begin(CS)) {
    Seiral.println(F("SD Card Open successfully!"));
    return;
  } else {
    Serial.println(F("SD Card Open Fail!"))
  }

  myFile = SD.open("output.txt", FILE_WRITE);
  if(myFile) {
    Serial.println(F("myFile open"));
    myFile.println("myFile open");
  } else {
    Serial.println(F("Fail to open myFile"));
  }
}

void setup() {
  Serial setup
  Serial.begin(SERIAL_BUS);
  {
    SoftwareSerial mySerial(TX, RX);
    Adafruit_GPS GPS(&mySerial);

    // GPS setup
    GPS.begin(GPS_BUS);
    char c;
    while(!GPS.parse(GPS.lastNMEA())) {
      c = GPS.read();
    }
    Serial.println(F("parsing end!"));
    Serial.println(GPS.latitudeDegrees);
    Serial.println(GPS.longitudeDegrees);
    setup location
    s.setLoc(GPS.longitudeDegrees, GPS.latitudeDegrees);
  }

  // setup others
  setSD();
  lmt.begin();
  body.attach(PIN_BODY);
  head.attach(PIN_HEAD);

  Serial.println(F("setup end\n"));
  delay(WAIT_TIME);
}

void loop() {
  t = rtc.time();
  s.setTime(t.yr, t.mon, t.date, t.hr, t.min, t.sec);
  s.calcPos();

  body.write(s.azimuth);
  head.write(s.altitude);

  uint16_t lux = lmt.readLightLevel();
  char buf[LEN];
  snprintf(buf, sizeof(buf),
           "%04d,%02d,%02d,%02d,%02d,%02d,",
           t.yr, t.mon, t.date, t.hr, t.min, t.sec);
  Serial.print(F("  val : ")); Serial.println(lux);
  myFile.print(buf); myFile.println(lux);

  delay(WAIT_TIME);
}
