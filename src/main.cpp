/*
   Safety Engine Control (PT&E) **พี่อมร
   By Arthit_N ... Tel.0959058972  E-Mail:tgbroadcast@gmail.com
   Kickoff 26/01/2022
   Ver.  0.9.0
   put on github 26/01/2022
*/
#include <Arduino.h>
#include <Wire.h>
#include <virtuabotixRTC.h>

#define TEST_PIN 10
#define ARMED_PIN 11

#define LED_YELLOW 15
#define LED_RED 16
#define BUZZER_PIN 17

virtuabotixRTC myRTC(7, 6, 5); //  Pin RTC (CLK,DAT,RST)

bool goodDateTime;
bool testSw = false;
bool armedSw = false;

void readDateTime()
{
  myRTC.updateTime();
  Serial.println("------------------------------------------------");
  if (myRTC.year < 2021 || myRTC.year > 2052)
  {
    goodDateTime = false;
    Serial.println("Date Time Fault");
  }
  else
  {
    goodDateTime = true;
    Serial.println("Date Time is OK");
  }
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

void startTone(){
  tone(BUZZER_PIN, 262, 200);
  delay(100);
  noTone(BUZZER_PIN);

  tone(BUZZER_PIN, 330, 200);
  delay(100);
  noTone(BUZZER_PIN);

  tone(BUZZER_PIN, 392, 200);
  delay(100);
  noTone(BUZZER_PIN);

  tone(BUZZER_PIN, 523, 200);
  delay(200);
  noTone(BUZZER_PIN);

  delay(200);

  tone(BUZZER_PIN, 523, 200);
  delay(80);
  noTone(BUZZER_PIN);

  tone(BUZZER_PIN, 659, 200);
  delay(70);
  noTone(BUZZER_PIN);

  tone(BUZZER_PIN, 784, 200);
  delay(70);
  noTone(BUZZER_PIN);

  tone(BUZZER_PIN, 1047, 200);
  delay(300);
  noTone(BUZZER_PIN);
}

void beep(){
  tone(BUZZER_PIN, 523, 200);
  delay(100);
  noTone(BUZZER_PIN);
  delay(100);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("------------------------------------------------");
  Serial.println("Safety Engine Control");
  Serial.println("Ver.  0.9.0");

  pinMode(TEST_PIN, INPUT);
  pinMode(ARMED_PIN, INPUT);

  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  delay(500);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  beep();
  delay(200);
  startTone();

  // seconds, minutes, hours, day of the week, day of the month, month, year  //  Set วันเวลา RTC
  // myRTC.setDS1302Time(0, 20, 11, 3, 26, 1, 2022);                          //  Set วันเวลา RTC
  readDateTime();
}

void readTestSw(){
  if (digitalRead(TEST_PIN) == LOW)
  {
    testSw = true;
    //Serial.println("Test Mode !");
    return;
    digitalWrite(LED_YELLOW, HIGH);
  }
  if (digitalRead(TEST_PIN) == HIGH)
  {
    testSw = false;
    //Serial.println("Normal Mode");
    //return;
    digitalWrite(LED_YELLOW, LOW);
  }
}
void readArmedSw()
{
  if (digitalRead(ARMED_PIN) == LOW)
  {
    armedSw = true;
     //Serial.println("ARMED Disable !");
    //return;
    digitalWrite(LED_YELLOW, HIGH);
  }
  if (digitalRead(ARMED_PIN) == HIGH)
  {
    armedSw = false;
     //Serial.println("ARMED Enable");
     //return;
    digitalWrite(LED_YELLOW, LOW);
  }
}

void loop()
{
  readTestSw();
  readArmedSw();
  delay(1000);
}