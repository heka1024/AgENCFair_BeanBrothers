#include <Servo.h>
#include <SD.h>
#include <DS1302.h>
#include <stdio.h>
#include <math.h>
#define L 37 * M_PI / 180 // RADIAN, 위도
#define MAX_ANGLE 180   // 최대 회전 각, 머리는 MAX_ANGLE / 2가 최대 각
#define PIN_BODY 9
#define PIN_HEAD 8
#define WAIT_ITER 10    // 한 번에 회전하는 각도
#define WAIT_TIME 1000 // 한 번 회전에 걸리는 시간, ms 단위
#define L_HERE 127
#define L_STD 135
#define KCEPIN 7
#define KCEPIN 7
#define KIOPIN 6
#define KSCLKPIN 5
#define BUF_MAX 51
#define CDS_PIN A0      // CDS 센서 연결된 핀, 아날로그 신호

typedef struct {
  double azi; // 방위각
  double alt; // 고도
} Sun;

DS1302 rtc(KCEPIN, KIOPIN, KSCLKPIN);
Time t = rtc.time();
File myFile;
Servo body, head;
Sun s;

int monToDate(int mon) {
  int ans;
  if(mon == 2) {
    int leap = t.yr;
    if(leap % 400 == 0) {
      ans = 29;
    } else if(leap % 100 == 0) {
      ans = 28;
    } else if(leap % 4 == 0) {
      ans = 29;
    } else {
      ans = 28;
    }
  }
  switch(mon) {
    case 1:
      ans = 31;
      break;
    case 3:
      ans = 31;
      break;
    case 4:
      ans = 30;
      break;
    case 5:
      ans = 31;
      break;
    case 6:
      ans = 30;
      break;
    case 7:
      ans = 31;
      break;
    case 8:
      ans = 31;
      break;
    case 9:
      ans = 30;
      break;
    case 10:
      ans = 31;
      break;
    case 11:
      ans = 30;
      break;
    case 12:
      ans = 31;
      break;
    default:
      break;
  }
  return ans;
}

Sun radToDeg(Sun from) {
  double K = 180 / M_PI;
  Sun to;
  to.azi = K * from.azi;
  to.alt = K * from.alt;

  return to;
}

Sun calcSun() {
  int mon = t.mon, date = t.date, i;
  Sun ans = {.azi = 0, .alt = 0};
  int n = date;
  // n :: yearday
  for(i = 1; mon > i; i++) {
    n += monToDate(i);
  }

  double B = (360 / 365) * (n - 81) * M_PI / 180;
  double EoT = 9.87 * sin(2 * B) - 7.53 * cos(B) - 1.5 * sin(B);
  double W = (((double) t.hr + (double) t.min / 60 + (double) t.sec / 3600) - 12 + (double) (L_HERE - L_STD) / 15 + (double) EoT / 60) * 15; // DEG
  W = W * M_PI / 180; // DEG

  ans.alt = asin(sin(L) * sin(D) + cos(L) * cos(D) * cos(W));
  ans.azi = asin(-sin(W) * cos(D) / cos(ans.alt));

  ans = radToDeg(ans);

  if(ans.alt < 0) {
    ans.alt = 0;
  }
  if(ans.azi < 90) {
    ans.azi = 90;
  } else if(ans.azi > 270) {
    ans.azi = 270;
  }
  ans.azi -= 90;

  return ans;
}

void writeLux() {
  int val = analogRead(CDS_PIN);
  Serial.print("val : ");
  Serial.println(val);

  char buf[BUF_MAX];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d ", t.yr, t.mon, t.date, t.hr, t.min, t.sec);

  myFile.print(buf);
  myFile.println(val);
}

void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  myFile = SD.open("test.txt", FILE_WRITE);
  if(myFile) {
    Serial.print("myFile open success!\n");
    myFile.print("Today is ");
    char buf[BUF_MAX];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d\n", t.yr, t.mon, t.date, t.hr, t.min, t.sec);
    myFile.println(buf);
  } else {
    Serial.print("myFile open fail!\n");
  }
  body.attach(PIN_BODY);
  head.attach(PIN_HEAD);
}

void loop() {
  static int cnt = 1;
  Serial.print(cnt);
  Serial.print("'s loop!\n");

  t = rtc.time();
  s = calcSun();
  body.write(s.azi);
  body.write(s.alt);

  Serial.print("azi : "); Serial.println(s.azi);
  Serial.print("alt : "); Serial.println(s.alt);
  writeLux();
  Serail.println();

  delay(WAIT_TIME);
}
