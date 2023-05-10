#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <time.h>
#include <ESP32Time.h>

//Wifi Credentials.
const char* ssid = "YOUR SSID";
const char* password = "YOUR PASSWORD";
//NTP Server settings (UK).
const char* ntpServer = "pool.ntp.org";
const long  gmtOffSet = 0;
const int   daylightSavingsOffset = 3600;

//ints to store out time broken down.
int hrFirst, hrSecond, minFirst, minSecond, day;
//Amount to divide color value by, decreasing its brightness.
int brightnessReduce = 1; //1-16 is a good range to work in.

//Char to store the full time string given by RTC.
char fullTime[8];

//Color sources.
uint32_t fillColor, dayColor, hrFirstColor, hrSecondColor, minFirstColor, minSecondColor;

ESP32Time rtc(0);
//Define the number of leds the strip contains and the pin it's connected to.
Adafruit_NeoPixel ledDay = Adafruit_NeoPixel(7, 15, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledHourFirst = Adafruit_NeoPixel(2, 2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledHourSecond = Adafruit_NeoPixel(9, 4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledMinuteFirst = Adafruit_NeoPixel(5, 5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledMinuteSecond = Adafruit_NeoPixel(9, 18, NEO_GRB + NEO_KHZ800);

void setup() {
  ledDay.begin();
  ledHourFirst.begin();
  ledHourSecond.begin();
  ledMinuteFirst.begin();
  ledMinuteSecond.begin();
}

void doShow(){
  ledDay.show();
  ledHourFirst.show();
  ledHourSecond.show();
  ledMinuteFirst.show();
  ledMinuteSecond.show();
}

void doFill(){
  ledDay.fill(fillColor, 0, 7);
  ledHourFirst.fill(fillColor, 0, 2);
  ledHourSecond.fill(fillColor, 0, 9);
  ledMinuteFirst.fill(fillColor, 0, 5);
  ledMinuteSecond.fill(fillColor, 0, 9);
}

//Set RTC using the data from the ntp pool.
void setRTC(){
  configTime(gmtOffSet, daylightSavingsOffset, ntpServer);
  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
    rtc.setTimeStruct(timeinfo);
    //Call our updateTime function to set init time.
    updateTime();
  }
}

void updateTime(){
  //Copy rtc.getTime, which returns string to our fullTime char.
  strcpy(fullTime, rtc.getTime().c_str());

  //Update our time ints using the fullTime char elements.
  hrFirst = fullTime[0] - '0';
  hrSecond = fullTime[1] - '0';
  minFirst = fullTime[3] - '0';
  minSecond = fullTime[4] - '0';

  //Update day, getDayOfWeek returns int.
  day = rtc.getDayofWeek();

  //Set our led strip positions, input is located at the top of the view, start with our
  //maximum and subtract the current.
  day == 0 ? day = 6 : day = day -1;
  ledDay.setPixelColor(day, dayColor);
  ledHourFirst.setPixelColor(2 - hrFirst, hrFirstColor);
  ledHourSecond.setPixelColor(9 - hrSecond, hrSecondColor);
  ledMinuteFirst.setPixelColor(5 - minFirst, minFirstColor);
  ledMinuteSecond.setPixelColor(9 - minSecond, minSecondColor);
}

void doWiFi(){
  //Set WiFi mode to station mode to connect to access point.
  WiFi.mode(WIFI_STA);
  //Begin WiFi using previously declared credentials.
  WiFi.begin(ssid, password);
  //While not connected we fill all leds with red.
    while(WiFi.status() != WL_CONNECTED){
      //Fill LEDS with red.
      fillColor = Adafruit_NeoPixel::Color(255, 0, 0);
      doFill();
      doShow();
    }
  //Connected, fill with white.
  fillColor = Adafruit_NeoPixel::Color(255, 255, 255);
  doFill();
  doShow();
  //Set RTC.
  setRTC();
  //Delay 500ms to give everything a chance to catch up.
  delay(500);
  //WiFi no longer required, disconnect.
  WiFi.disconnect();
}

void setColorBrightness(){
  //The value of brightnessReduce is used to divide the color value by, reducing its brightness.
  //Modify these values to suit, as is, it increases from 7-9am and decreases finally to black/blank between 19-22.
  switch (rtc.getHour(true)){
    //rtc.getHour(true) returns 24hr, 0-23.
    case 7:
      brightnessReduce = 8;
      break;
    case 8:
      brightnessReduce = 4;
      break;
    case 9:
      brightnessReduce = 1;
      break;
    case 19: 
      brightnessReduce = 4;
      break;
    case 20:
      brightnessReduce = 8;
      break;
    case 21:
      brightnessReduce = 12;
      break;
    case 22:
      brightnessReduce = 16;
      break;
  }

  //Set color values divided by reduce amount.
  dayColor = Adafruit_NeoPixel::Color(204 / brightnessReduce, 51 / brightnessReduce, 0 / brightnessReduce);
  hrFirstColor = Adafruit_NeoPixel::Color(51 / brightnessReduce, 153 / brightnessReduce, 255 / brightnessReduce);
  hrSecondColor = Adafruit_NeoPixel::Color(51 / brightnessReduce, 204 / brightnessReduce, 51 / brightnessReduce);
  minFirstColor = Adafruit_NeoPixel::Color(204 / brightnessReduce, 0 / brightnessReduce, 204 / brightnessReduce);
  minSecondColor = Adafruit_NeoPixel::Color(255 / brightnessReduce, 153 / brightnessReduce, 0 / brightnessReduce);
  fillColor = Adafruit_NeoPixel::Color(255 / brightnessReduce, 255 / brightnessReduce, 255 / brightnessReduce);
}



void loop() {
  //If fullTime[0] contains no usable value, we either have not pulled data from NTP pool or something went wrong somehow.
  //Do WiFi connect.
  while(fullTime[0] == '\0'){
    doWiFi();
  }

  //Set color values based on brightnessReduce int.
  setColorBrightness();
  //fullTime[0] contains useable values, we can assume everything has gone as expected.
  doFill();
  //Call updateTime method after fill.
  updateTime();
  doShow();
  //Delay by 1000ms (1 second).
  delay(1000);
}
