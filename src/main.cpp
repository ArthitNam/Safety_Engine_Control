/*
   Safety Engine Control (PT&E) **พี่อมร
   By Arthit_N ... Tel.0959058972  E-Mail:tgbroadcast@gmail.com
   Kickoff 26/01/2022
   Ver.  0.9.0
   put on github ..
*/
#include <Arduino.h>
#include <Wire.h>
#include <virtuabotixRTC.h>

virtuabotixRTC myRTC(7, 6, 5); //  Pin RTC (CLK,DAT,RST)

void setup() {
  Serial.begin(115200);
  Serial.println("------------------------------------------------");
  Serial.println("Safety Engine Control");
  Serial.println("Ver.  0.9.0");

  // seconds, minutes, hours, day of the week, day of the month, month, year  //  Set วันเวลา RTC
  //myRTC.setDS1302Time(0, 20, 11, 3, 26, 1, 2022);                          //  Set วันเวลา RTC

  myRTC.updateTime();
  Serial.println("------------------------------------------------");
  Serial.print("Current Date / Time: ");
  Serial.print(myRTC.dayofmonth);
  Serial.print(" / ");
  Serial.print(myRTC.month);
  Serial.print(" / ");
  Serial.print(myRTC.year);
  Serial.print("  ");
  Serial.print(myRTC.hours);
  Serial.print(": ");
  Serial.print(myRTC.minutes);
  Serial.print(": ");
  Serial.println(myRTC.seconds);

  
}

void loop() {
  
}