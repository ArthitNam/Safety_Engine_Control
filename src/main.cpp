/*
   Safety Engine Control (PT&E) **พี่อมร
   By Arthit_N ... Tel.0959058972  E-Mail:tgbroadcast@gmail.com
   Kickoff 26/01/2022
   put on github 26/01/2022
*/
//**Mega : MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 53(pinMode OUTPUT**)

#include <avr/wdt.h>
#include <Arduino.h>
#include <TimerOne.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <SD.h>
#include <stdio.h>
#include <string.h>
#include <Password.h>
#include <SPI.h>
#include <max6675.h>

// define Pin For Input-Output(mega2560)
// input
#define FLOW_SW 14
#define OIL_PRES_SW 19
#define WATER_TANK 12
#define FIREPUMP_ACC 44
#define PULSEPIN 18
// Oututput
#define LED_YELLOW 15
#define LED_SHUTOFF 16
#define LED_DISABLE 22
#define BUZZER_PIN 17
#define BELL_PIN 42
#define ENGINE_RELAY_SHUTOFF 43
// Button
#define RESET_BUTTON 36
#define UP_BUTTON 38
#define DOWN_BUTTON 39
#define MODE_BUTTON 40
#define ABORT_BUTTON 41
#define SILENCE_ALARM_BUTTON 9
#define DISABLE_SW 11

String version = "0.9.4";  // แก้ไข Version *****

int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
LiquidCrystal_I2C lcd(0x27, 20, 4);
const int chipSelect = 53; // CS SD Spi

String passwd;
Password password = Password("1234");
String newPasswordString = "";
char newPassword[5]; // charater string of newPasswordString
const unsigned long measurementPeriod = 1000;
unsigned long timer;
unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;
unsigned long currentMillis;
unsigned long currentMillis1;
unsigned long countSec;
unsigned long last1 = 0, last2 = 0, last3 = 0, last4 = 0, last5, last6, last7=0, last8=0,last9;
unsigned long waterTankFault_verify = 15000;
unsigned long oilPressFault_verify = 10000;
unsigned long coolingFault_verify = 5000;
unsigned long coolingFault_delay;
unsigned long oilPressFault_delay = 1;
unsigned long dimTime;
unsigned long pageOtherTime = 60000;
unsigned long engineRunTime;
bool engineShutOff = false;
bool goodDateTime;
bool armedSw = false;
bool engineRun = false;
bool engineStart = false;
bool coolSys = false;
bool oilPress = false;
bool oilPress_fault = false;
bool oilPress_fault_delay_start = false;
bool oilPress_fault_verify_start = false;
bool waterTank = false;
bool warning = false;
bool cooling_fault = false;
bool cooling_fault_delay_start = false;
bool cooling_fault_verify_start = false;
bool settingMode = false;
bool showDisplay = false;
bool displayDim = false;
bool silence_alarm = false;
bool tempFault = false;
bool buzzerAlarmON = false;
bool reset = false;
bool noBeep = false;
int page = 1;
int temp, newTemp;
int overTemp;
int historyCount = 0;
String buffer;
bool read = false;
int count = 0;
int buttonState = 1;
int menu = 0;
int countDown = 60;
unsigned long engineRPM;
bool powerOff = false;
unsigned long lastmillis = 0;
bool changePasswordDone = false;
// Read RPM
const byte PulsesPerRevolution = 18;
const unsigned long ZeroTimeout = 100000;
const byte numReadings = 2;
volatile unsigned long LastTimeWeMeasured;
volatile unsigned long PeriodBetweenPulses = ZeroTimeout + 1000;
volatile unsigned long PeriodAverage = ZeroTimeout + 1000;
unsigned long FrequencyRaw;
unsigned long FrequencyReal;
unsigned long RPM;
unsigned int PulseCounter = 1;
unsigned long PeriodSum;
unsigned long LastTimeCycleMeasure = LastTimeWeMeasured;
unsigned long CurrentMicros = micros();
unsigned int AmountOfReadings = 1;
unsigned int ZeroDebouncingExtra;
unsigned long readings[numReadings];
unsigned long readIndex;
unsigned long total;
unsigned long average;
/////////////////////
RTC_DS3231 rtc;
File myFile;
void excuteAction(void);

byte engineChar0[8] = {
    B01111,
    B00011,
    B10100,
    B10100,
    B11100,
    B10110,
    B10010,
    B00011};
byte engineChar1[8] = {
    B11000,
    B01011,
    B00101,
    B00001,
    B00001,
    B00101,
    B01011,
    B10000};
byte upChar[8] = {
    B00100,
    B01110,
    B11111,
    B11111,
    B01110,
    B01110,
    B01110,
    B01110};
byte downChar[8] = {
    B01110,
    B01110,
    B01110,
    B01110,
    B11111,
    B11111,
    B01110,
    B00100};
byte spkChar[8] = {
    B00011,
    B00111,
    B11111,
    B11111,
    B11111,
    B11111,
    B00111,
    B00011};
byte soundChar[8] = {
    B00001,
    B00101,
    B10101,
    B10101,
    B10101,
    B10101,
    B00101,
    B00001};
byte muteChar[8] = {
    B00000,
    B00000,
    B10001,
    B01010,
    B00100,
    B01010,
    B10001,
    B00000};
byte enterChar[8] = {
    B00000,
    B00001,
    B00101,
    B01101,
    B11111,
    B01100,
    B00100,
    B00000};

void writeDataLoger(String event)
{
  myFile = SD.open("data.txt", FILE_WRITE); // เปิดไฟล์เพื่อเขียนข้อมูล โหมด FILE_WRITE
  if (myFile)
  {
    DateTime now = rtc.now();
    Serial.print("Writing to SD Card ");
    myFile.print(event); // สั่งให้เขียนข้อมูล
    myFile.print(now.day());
    myFile.print("-");
    if (now.month() < 10)
    {
      myFile.print("0");
    }
    myFile.print(now.month());
    myFile.print("-");
    myFile.print(now.year());
    myFile.print(" ");
    myFile.print(now.hour());
    myFile.print(":");
    if (now.minute() < 10)
    {
      myFile.print("0");
    }
    myFile.print(now.minute());
    myFile.print(":");
    if (now.second() < 10)
    {
      myFile.print("0");
    }
    myFile.println(now.second());

    myFile.close(); // ปิดไฟล์หลังเขียนเสร็จทุกครั้ง
    Serial.println("done.");
  }
  else
  {
    // ถ้าเปิดไฟล์ไม่สำเร็จ ให้แสดง error
    Serial.println("error opening File");
  }
  // เปิดไฟล์เพื่ออ่าน
  myFile = SD.open("data.txt"); // สั่งให้เปิดไฟล์ชื่อ test.txt เพื่ออ่านข้อมูล
  if (myFile)
  {
    historyCount = 0;
    Serial.println("---------------------------------");
    Serial.println("Read All DataLoger From SD Card..");
    // อ่านข้อมูลทั้งหมดออกมา
    while (myFile.available())
    {
      historyCount++;
      Serial.print(historyCount);
      Serial.print(" ");
      buffer = myFile.readStringUntil('\n');
      Serial.println(buffer); // Printing for debugging purpose
    }
    myFile.close(); // เมื่ออ่านเสร็จ ปิดไฟล์
    Serial.print("History Count = ");
    Serial.println(historyCount);
    Serial.println("---------------------------------");
  }
  else
  {
    // ถ้าอ่านไม่สำเร็จ ให้แสดง error
    Serial.println("error opening SD Card");
  }
}
void verifySD()
{
  pinMode(SS, OUTPUT);
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect))
  {
    Serial.println("initialization failed!");
    lcd.setCursor(0, 2);
    lcd.print("  SD Card Failed!   ");
    delay(2000);
    return;
  }
  Serial.println("initialization done.");
  lcd.setCursor(0, 2); //
  lcd.print("   SD Card is OK    ");
  delay(1000);
}
void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}
String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; // !!! NOTE !!! Remove the space between
  return String(data);
}
void readEeprom()
{
  EEPROM.begin();
  engineRunTime = EEPROM.read(0);
  overTemp = EEPROM.read(10);
  coolingFault_delay = EEPROM.read(20);
  dimTime = EEPROM.read(30);
  engineRPM = EEPROM.read(40);
  passwd = readStringFromEEPROM(50);
  noBeep = EEPROM.read(60);

  if (engineRunTime < 0 || engineRunTime > 4294967294)
  {
    engineRunTime = 0;
  }
  if (overTemp < 10 || overTemp > 180)
  {
    overTemp = 95;
  }
  if (coolingFault_delay < 0 || coolingFault_delay > 60)
  {
    coolingFault_delay = 15;
  }
  if (dimTime < 1 || dimTime > 30)
  {
    dimTime = 15;
  }
  if (engineRPM < 200 || engineRPM > 20000)
  {
    engineRPM = 3000;
  }
  if (passwd == "" || passwd.length() != 4)
  {
    passwd = "1234";
  }
  if (noBeep != false && noBeep != true)
  {
    noBeep = false;
  }

  passwd.toCharArray(newPassword, passwd.length() + 1);
  password.set(newPassword);

  Serial.print("EngineRunTime = ");
  Serial.println(engineRunTime);
  Serial.print("Over Temp = ");
  Serial.println(overTemp);
  Serial.print("Cooling Fault Delay = ");
  Serial.print(coolingFault_delay);
  Serial.println(" S.");
  Serial.print("Dimer LCD Time = ");
  Serial.print(dimTime);
  Serial.println(" M.");
  Serial.print("ENGINE RPM Start = ");
  Serial.println(engineRPM);
  Serial.print("Password = ");
  Serial.println(passwd);
  Serial.print("NoBeep Button = ");
  Serial.println(noBeep);
}
void startTone()
{
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
void beep()
{
  if (!noBeep)
  {
    tone(BUZZER_PIN, 523, 0);
    delay(40);
    noTone(BUZZER_PIN);
  }
}
void displayOff()
{
  lcd.noDisplay();
  lcd.noBacklight();
  password.reset();
}
void displayOn()
{
  lcd.backlight();
  lcd.display();
}
void showTimeNow()
{
  DateTime now = rtc.now();
  Serial.print(now.day());
  Serial.print("-");
  Serial.print(now.month());
  Serial.print("-");
  Serial.println(now.year());
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());
}

void buzzerAlarm()
{
  if (buzzerAlarmON == true)
  {
    if (silence_alarm == false && engineShutOff == false)
    {
      currentMillis = millis();
      if (currentMillis - previousMillis >= 500)
      {
        tone(BUZZER_PIN, 800, 0);
        digitalWrite(BELL_PIN, HIGH);

        if (currentMillis - previousMillis >= 1000)
        {
          noTone(BUZZER_PIN);
          digitalWrite(BELL_PIN, LOW);
          previousMillis = currentMillis;
        }
      }
    }
  }
  else
  {
    noTone(BUZZER_PIN);
    digitalWrite(BELL_PIN, HIGH);
    // if (engineShutOff == false)
    // {
    //   noTone(BUZZER_PIN);
    //   digitalWrite(BELL_PIN, HIGH);
    // }
  }
}
void silenceAlarmReset()
{
  // bool newsilenceAlarm = false;
  // if (newsilenceAlarm != silence_alarm)
  // {
  //   silence_alarm = newsilenceAlarm;
  // }
  silence_alarm = false;
  buzzerAlarmON = true;
  buzzerAlarm();
}
void oilPressFault()
{
  digitalWrite(LED_YELLOW, HIGH);
  if (showDisplay == true)
  {
    silenceAlarmReset();
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("   !! WARNING !!    ");
    lcd.setCursor(0, 2);
    lcd.print(" OIL PRESSURE FAULT ");
    showDisplay = false;
    writeDataLoger("OIL PRESSURE FAULT,");
  }
  // if (millis() - last9 >= 1000 && countDown > 0 && armedSw == false && (digitalRead(ABORT_BUTTON) != LOW))
  // {
  //   countDown--;
  //   Serial.println(countDown);
  //   last9 = millis();
  //   lcd.clear();
  //   lcd.setCursor(0, 0);
  //   lcd.print(" OILING SYSTEM FAULT");
  //   lcd.setCursor(0, 2);
  //   lcd.print(" ShutOFF Countdown !");
  //   lcd.setCursor(7, 3);
  //   lcd.print(countDown);
  //   lcd.print(" Sec.");
  // }
  // if (digitalRead(ABORT_BUTTON) == LOW && engineShutOff == false)
  // {
  //   beep();
  //   countDown = 60;
  // }
  // if (countDown == 0 && engineShutOff == false)
  // {
  //   // Shut OFF
  //   lcd.clear();
  //   lcd.setCursor(0, 1);
  //   lcd.print(" OILING SYSTEM FALUT");
  //   lcd.setCursor(0, 2);
  //   lcd.print(" ! ENGINE SHUTOFF ! ");

  //   tone(BUZZER_PIN, 800, 0);
  //   digitalWrite(LED_SHUTOFF, HIGH);
  //   writeDataLoger("Engine ShutOFF,");
  //   engineShutOff = true;
  // }
}
void coolingFault()
{
  digitalWrite(LED_YELLOW, HIGH);
  if (showDisplay == true)
  {
    silenceAlarmReset();
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("   !! WARNING !!    ");
    lcd.setCursor(0, 2);
    lcd.print("COOLING SYSTEM FAULT");
    showDisplay = false;
    writeDataLoger("Cooling System Fault,");
  }
  if (millis() - last6 >= 1000 && countDown > 0 && armedSw == false && (digitalRead(ABORT_BUTTON) != LOW) && temp >= overTemp)
  {
    countDown--;
    Serial.println(countDown);
    last6 = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("COOLING SYSTEM FAULT");
    lcd.setCursor(0, 2);
    lcd.print(" ShutOFF Countdown !");
    lcd.setCursor(7, 3);
    lcd.print(countDown);
    lcd.print(" Sec.");
  }
  if (digitalRead(ABORT_BUTTON) == LOW && engineShutOff == false)
  {
    beep();
    countDown = 60;
  }
  if (countDown == 0 && engineShutOff == false)
  {
    // Shut OFF
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("COOLING SYSTEM FAULT");
    lcd.setCursor(0, 2);
    lcd.print(" ! ENGINE SHUTOFF ! ");

    tone(BUZZER_PIN, 800, 0);
    digitalWrite(LED_SHUTOFF, HIGH);
    writeDataLoger("Engine ShutOFF,");
    engineShutOff = true;
  }
}
void engineOverTemp()
{
  digitalWrite(LED_YELLOW, HIGH);
  if (showDisplay == true)
  {
    silenceAlarmReset();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("! ENGINE OVER TEMP !");
    lcd.setCursor(7, 1);
    lcd.print(temp, 1);
    lcd.print(" C");
    lcd.print((char)223);
    writeDataLoger("Engine Over Temp,");
    showDisplay = false;
  }
  if (millis() - last6 >= 1000 && countDown > 0 && armedSw == false && (digitalRead(ABORT_BUTTON) != LOW))
  {

    countDown--;
    Serial.println(countDown);
    last6 = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("! ENGINE OVER TEMP !");
    lcd.setCursor(7, 1);
    lcd.print(temp, 1);
    lcd.print(" C");
    lcd.print((char)223);
    lcd.setCursor(0, 2);
    lcd.print(" ShutOFF Countdown  ");
    lcd.setCursor(7, 3);
    lcd.print(countDown);
    lcd.print(" Sec.");
  }
  if (digitalRead(ABORT_BUTTON) == LOW && engineShutOff == false)
  {
    beep();
    countDown = 60;
  }
  if (countDown == 0 && engineShutOff == false)
  {
    // Shut OFF
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("     OVER  TEMP     ");
    lcd.setCursor(0, 2);
    lcd.print(" ! ENGINE SHUTOFF ! ");
    tone(BUZZER_PIN, 800, 0);
    digitalWrite(BELL_PIN, HIGH);
    digitalWrite(LED_SHUTOFF, HIGH);
    digitalWrite(ENGINE_RELAY_SHUTOFF, LOW);
    writeDataLoger("Engine ShutOFF,");
    engineShutOff = true;
  }
}
void waterTank_fault()
{
  digitalWrite(LED_YELLOW, HIGH);
  if (showDisplay == true)
  {
    silenceAlarmReset();
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("   !! WARNING !!    ");
    lcd.setCursor(0, 2);
    lcd.print("  WATER TANK : LOW  ");
    showDisplay = false;
    writeDataLoger("Water tank low,");
  }
}
void readWaterTank()
{
  if (digitalRead(WATER_TANK) == LOW)
  {
    if (last1 < waterTankFault_verify)
    {
      last1++;
      Serial.println(last1);
    }
    if (last1 >= waterTankFault_verify)
    {
      bool newWaterTank = true;
      if (newWaterTank != waterTank)
      {
        waterTank = newWaterTank;
        page = 0;
        showDisplay = true;
        buzzerAlarmON = true;
      }
      waterTank_fault();
    }
  }
  if (digitalRead(WATER_TANK) == HIGH)
  {
    last1 = 0;
    bool newWaterTank = false;
    if (newWaterTank != waterTank)
    {
      waterTank = newWaterTank;
      showDisplay = true;
      //buzzerAlarmON = false;
    }
  }
}
void readOilPressSw()
{
  if (digitalRead(OIL_PRES_SW) == LOW)
  {
    bool newOilPress = true;
    if (newOilPress != oilPress)
    {
      oilPress = newOilPress;
      showDisplay = true;
    }
  }
  if (digitalRead(OIL_PRES_SW) == HIGH)
  {
    bool newOilPress = false;
    if (newOilPress != oilPress)
    {
      oilPress = newOilPress;
      showDisplay = true;
    }
  }
  ///////////////////////
  if (oilPress == false && engineRun == true)
  {
    if (oilPress_fault_delay_start == false)
    {
      last7 = millis();
      oilPress_fault_delay_start = true;
    }
    if (oilPress_fault_delay_start == true && millis() - last7 < oilPressFault_delay * 1000)
    {
      //Serial.print("Start Delay oilPress-Fault  ");
      //Serial.println((millis() - last7) / 1000);
    }

    if (millis() - last7 > oilPressFault_delay * 1000)
    {
      if (oilPress_fault_verify_start == false)
      {
        last8 = millis();
        oilPress_fault_verify_start = true;
      }
      if (oilPress_fault_verify_start == true && millis() - last8 < oilPressFault_verify)
      {
        //Serial.print("Start verify oilPress-Fault  ");
        //Serial.println((millis() - last8) / 1000);
      }
      if (millis() - last8 >= oilPressFault_verify)
      {
        bool newoilPressFault = true;
        if (newoilPressFault != oilPress_fault)
        {
          Serial.println("OilPress System Fault !");
          oilPress_fault = newoilPressFault;
          showDisplay = true;
          buzzerAlarmON = true;
          page = 0;
        }
        oilPressFault();
      }
    }
  }
  if (oilPress == false && engineRun == false && oilPress_fault_delay_start == true)
  {
    last7 = 0;
    last8 = 0;
    last9 = 0;
    oilPress_fault = false;
    oilPress_fault_verify_start = false;
    silence_alarm = false;
    //buzzerAlarmON = false;

    showDisplay = true;
    page = 1;
    oilPress_fault_delay_start = false;
    Serial.println("oilPress true");
  }
  ////////////////////////////////
}
void readCoolSys()
{
  if (digitalRead(FLOW_SW) == LOW)
  {
    bool newCoolSys = true;
    if (newCoolSys != coolSys)
    {
      coolSys = newCoolSys;
      showDisplay = true;
    }
  }
  if (digitalRead(FLOW_SW) == HIGH)
  {
    bool newCoolSys = false;
    if (newCoolSys != coolSys)
    {
      coolSys = newCoolSys;
      showDisplay = true;
    }
  }
  if (coolSys == false && engineRun == true)
  {
    if (cooling_fault_delay_start == false)
    {
      last2 = millis();
      cooling_fault_delay_start = true;
    }
    if (cooling_fault_delay_start == true && millis() - last2 < coolingFault_delay * 1000)
    {
      Serial.print("Start Delay Cooling-Fault  ");
      Serial.println((millis() - last2) / 1000);
    }

    if (millis() - last2 > coolingFault_delay * 1000)
    {
      if (cooling_fault_verify_start == false)
      {
        last3 = millis();
        cooling_fault_verify_start = true;
      }
      if (cooling_fault_verify_start == true && millis() - last3 < coolingFault_verify)
      {
        Serial.print("Start verify Cooling-Fault  ");
        Serial.println((millis() - last3) / 1000);
      }
      if (millis() - last3 >= coolingFault_verify)
      {
        bool newCoolingFault = true;
        if (newCoolingFault != cooling_fault)
        {
          Serial.println("Cooling System Fault !");
          cooling_fault = newCoolingFault;
          showDisplay = true;
          buzzerAlarmON = true;
          page = 0;
        }
        coolingFault();
      }
    }
  }
  if (cooling_fault == true && engineRun == false && cooling_fault_delay_start == true)
  {
    last2 = 0;
    last3 = 0;
    cooling_fault = false;
    cooling_fault_verify_start = false;
    silence_alarm = false;
    //buzzerAlarmON = false;

    showDisplay = true;
    page = 1;
    cooling_fault_delay_start = false;
    Serial.println("coolSys true");
  }
}
void checkEngineRun()
{
  LastTimeCycleMeasure = LastTimeWeMeasured;
  CurrentMicros = micros();
  if (CurrentMicros < LastTimeCycleMeasure)
  {
    LastTimeCycleMeasure = CurrentMicros;
  }
  FrequencyRaw = 10000000000 / PeriodAverage;
  if (PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra)
  {
    FrequencyRaw = 0;
    ZeroDebouncingExtra = 2000;
  }
  else
  {
    ZeroDebouncingExtra = 0;
  }
  FrequencyReal = FrequencyRaw / 10000;
  RPM = FrequencyRaw / PulsesPerRevolution * 60;
  RPM = RPM / 10000;
  // Smoothing RPM:
  total = total - readings[readIndex];
  readings[readIndex] = RPM;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings)
  {
    readIndex = 0;
  }
  average = total / numReadings;
  // frequency = 4000;
  if (millis() - lastmillis >= 500 && RPM > 0)
  {
    Serial.print("Period: ");
    Serial.print(PeriodBetweenPulses);
    Serial.print("\tReadings: ");
    Serial.print(AmountOfReadings);
    Serial.print("\tFrequency: ");
    Serial.print(FrequencyReal);
    Serial.print("\tRPM: ");
    Serial.print(RPM);
    Serial.print("\tTachometer: ");
    Serial.println(average);
    lastmillis = millis(); // Update lastmillis
  }
  if (RPM >= engineRPM)
  {
    bool newEngineRun = true;
    if (newEngineRun != engineRun)
    {
      engineStart = true;
      engineRun = true;
      writeDataLoger("Eneing RUN,");
      if (displayDim == true)
      {
        displayOn();
        displayDim = false;
      }
      buttonState = 1;
      showDisplay = true;
    }
  }
  if (RPM < engineRPM)
  {
    bool newEngineRun = false;
    if (newEngineRun != engineRun)
    {
      engineRun = false;
      writeDataLoger("Eneing Stop,");
      showDisplay = true;
    }
  }
  currentMillis1 = millis();
  if (engineRun == true)
  {
    if (engineStart == false)
    {
      previousMillis1 = currentMillis1;
      engineStart = true;
    }
    countSec = ((currentMillis1 - previousMillis1) / 1000);
    // Serial.print("countSec = ");
    // Serial.println(countSec);
  }
  if (engineRun == false)
  {
    if (engineRun == false && engineStart == true)
    {
      engineRunTime = engineRunTime + countSec;
      EEPROM.put(0, engineRunTime);
      Serial.print("engineRunTime = ");
      Serial.println(engineRunTime);
      engineStart = false;
    }
    previousMillis1 = 0;
    countSec = 0;
  }
}
void readDisableSw()
{
  if (digitalRead(DISABLE_SW) == LOW)
  {
    bool newArmedSw = true;
    if (newArmedSw != armedSw)
    {
      armedSw = newArmedSw;
      digitalWrite(LED_DISABLE, HIGH);
      showDisplay = true;
      buzzerAlarmON = true;
    }
    if (showDisplay == true)
    {
      silenceAlarmReset();
      digitalWrite(LED_DISABLE, HIGH);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("   !! WARNING !!    ");
      lcd.setCursor(0, 2);
      lcd.print("  SHUTOFF DISABLE   ");
      showDisplay = false;
      writeDataLoger("SHUTOFF Disable,");
    }
  }
  if (digitalRead(DISABLE_SW) == HIGH)
  {
    bool newArmedSw = false;
    if (newArmedSw != armedSw)
    {
      armedSw = newArmedSw;
      digitalWrite(LED_DISABLE, LOW);
      showDisplay = true;
      //buzzerAlarmON = false;
      writeDataLoger("SHUTOFF Enable,");
    }
  }
}
void updateMenu()
{
  switch (menu)
  {
  case 0:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SETTING Sys. ");
    lcd.write(2);
    lcd.write(3);
    lcd.print(" ");
    lcd.print("Out");
    lcd.write(7);
    lcd.setCursor(0, 1);
    lcd.print(" Over Temp:");
    lcd.print(overTemp);
    lcd.print(" C");
    lcd.print((char)223);
    lcd.setCursor(0, 2);
    lcd.print(" Cool'F Delay:");
    lcd.print(coolingFault_delay);
    lcd.print(" S.");
    lcd.setCursor(0, 3);
    lcd.print(" Sleep Mode:");
    lcd.print(dimTime);
    lcd.print(" M.");
    settingMode = false;
    break;

  case 1:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SETTING Sys. ");
    lcd.write(2);
    lcd.write(3);
    lcd.print(" ");
    lcd.print("Set");
    lcd.write(7);
    lcd.setCursor(0, 1);
    lcd.print(">Over Temp:");
    lcd.print(overTemp);
    lcd.print(" C");
    lcd.print((char)223);
    lcd.setCursor(0, 2);
    lcd.print(" Cool'F Delay:");
    lcd.print(coolingFault_delay);
    lcd.print(" S.");
    lcd.setCursor(0, 3);
    lcd.print(" Sleep Mode:");
    lcd.print(dimTime);
    lcd.print(" M.");
    break;
  case 2:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SETTING Sys. ");
    lcd.write(2);
    lcd.write(3);
    lcd.print(" ");
    lcd.print("Set");
    lcd.write(7);
    lcd.setCursor(0, 1);
    lcd.print(" Over Temp:");
    lcd.print(overTemp);
    lcd.print(" C");
    lcd.print((char)223);
    lcd.setCursor(0, 2);
    lcd.print(">Cool'F Delay:");
    lcd.print(coolingFault_delay);
    lcd.print(" S.");
    lcd.setCursor(0, 3);
    lcd.print(" Sleep Mode:");
    lcd.print(dimTime);
    lcd.print(" M.");
    break;
  case 3:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SETTING Sys. ");
    lcd.write(2);
    lcd.write(3);
    lcd.print(" ");
    lcd.print("Set");
    lcd.write(7);
    lcd.setCursor(0, 1);
    lcd.print(" Over Temp:");
    lcd.print(overTemp);
    lcd.print(" C");
    lcd.print((char)223);
    lcd.setCursor(0, 2);
    lcd.print(" Cool'F Delay:");
    lcd.print(coolingFault_delay);
    lcd.print(" S.");
    lcd.setCursor(0, 3);
    lcd.print(">Sleep Mode:");
    lcd.print(dimTime);
    lcd.print(" M.");
    break;
  case 4:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SETTING Sys. ");
    lcd.write(2);
    lcd.write(3);
    lcd.print(" ");
    lcd.print("Set");
    lcd.write(7);
    lcd.setCursor(0, 1);
    lcd.print(">");
    lcd.write(0);
    lcd.write(1);
    lcd.print(" Start: ");
    lcd.print(engineRPM);
    lcd.print(" RPM");
    lcd.setCursor(0, 2);
    lcd.print(" History");
    lcd.setCursor(0, 3);
    lcd.print(" Change Password");
    break;
  case 5:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SETTING Sys. ");
    lcd.write(2);
    lcd.write(3);
    lcd.print(" ");
    lcd.print("Set");
    lcd.write(7);
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.write(0);
    lcd.write(1);
    lcd.print(" Start: ");
    lcd.print(engineRPM);
    lcd.print(" RPM");
    lcd.setCursor(0, 2);
    lcd.print(">History");
    lcd.setCursor(0, 3);
    lcd.print(" Change Password");
    break;
  case 6:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SETTING Sys. ");
    lcd.write(2);
    lcd.write(3);
    lcd.print(" ");
    lcd.print("Set");
    lcd.write(7);
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.write(0);
    lcd.write(1);
    lcd.print(" Start: ");
    lcd.print(engineRPM);
    lcd.print(" RPM");
    lcd.setCursor(0, 2);
    lcd.print(" History");
    lcd.setCursor(0, 3);
    lcd.print(">Change Password");
    break;
  case 7:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SETTING Sys. ");
    lcd.write(2);
    lcd.write(3);
    lcd.print(" ");
    lcd.print("Set");
    lcd.write(7);
    lcd.setCursor(0, 1);
    lcd.print(">Remove All History ");
    lcd.setCursor(0, 2);
    lcd.print(" Push Button Beep");
    lcd.write(4);
    if (!noBeep)
    {
      lcd.write(5);
    }
    else
    {
      lcd.write(6);
    }
    break;
  case 8:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SETTING Sys. ");
    lcd.write(2);
    lcd.write(3);
    lcd.print(" ");
    lcd.print("Set");
    lcd.write(7);
    lcd.setCursor(0, 1);
    lcd.print(" Remove All History ");
    lcd.setCursor(0, 2);
    lcd.print(">Push Button Beep");
    lcd.write(4);
    if (!noBeep)
    {
      lcd.write(5);
    }
    else
    {
      lcd.write(6);
    }
    break;
  }
}
void history()
{
  unsigned int position[500];
  bool exit = false;
  delay(200);
  while (!exit)
  {
    if (showDisplay == true)
    {
      if (count == 0)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("  History ");
        lcd.setCursor(0, 1);
        lcd.print("Reading Data...     ");
        lcd.setCursor(0, 2);
        lcd.print("From SD Card        ");
      }

      if (count != 0)
      {
        myFile = SD.open("data.txt");
        if (myFile)
        {
          Serial.println("---------------------------------");
          Serial.println("Read Line DataLoger From SD Card..");
          if (myFile.available())
          {
            myFile.seek(position[count]);
            Serial.print("Position = ");
            Serial.println(position[count]);
            Serial.print("History = ");
            Serial.print(count);
            Serial.print("/");
            Serial.println(historyCount);
            buffer = myFile.readStringUntil('\n');
            Serial.print("read = ");
            Serial.println(buffer);
            buffer.setCharAt(buffer.length() - 1, ' ');
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("  History ");
            lcd.setCursor(0, 3);
            lcd.write(2);
            lcd.setCursor(19, 3);
            lcd.write(3);
            lcd.setCursor(10, 0);
            char buff[16];
            sprintf(buff, "(%d/%d)", count, historyCount);
            lcd.print(buff);
            lcd.setCursor(0, 1);
            char str[64];
            strcpy(str, buffer.c_str());
            constexpr const char *delim = ",";
            char *token;
            token = strtok(str, delim);
            lcd.print(token);
            while (token != NULL)
            {
              token = strtok(NULL, delim);
              lcd.setCursor(0, 2);
              lcd.print(token);
            }
            showDisplay = true;
          }
          myFile.close(); // เมื่ออ่านเสร็จ ปิดไฟล์
        }
        else
        {
          // ถ้าอ่านไม่สำเร็จ ให้แสดง error
          Serial.println("error opening SD Card");
          lcd.setCursor(0, 2);
          lcd.print("  SD Card Error !   ");
        }
      }
      showDisplay = false;
    }

    if (read == false)
    {
      myFile = SD.open("data.txt"); // สั่งให้เปิดไฟล์ชื่อ test.txt เพื่ออ่านข้อมูล
      if (myFile)
      {
        Serial.println("---------------------------------");
        Serial.println("Read All DataLoger From SD Card..");
        // อ่านข้อมูลทั้งหมดออกมา
        while (myFile.available())
        {
          if (digitalRead(MODE_BUTTON) == LOW && displayDim == false)
          {
            beep();
            buttonState = 1;
            page = 1;
            showDisplay = true;
            delay(200);
          }
          count++;
          if (count > historyCount)
          {
            count = 1;
          }
          Serial.print(count);
          Serial.print(" ");
          position[count] = myFile.position();
          buffer = myFile.readStringUntil('\n');
          Serial.println(buffer);
          Serial.println(myFile.position());
        }
        myFile.close(); // เมื่ออ่านเสร็จ ปิดไฟล์
        Serial.print("History Count = ");
        Serial.println(historyCount);
        Serial.println("---------------------------------");
        count = 1;
        read = true;
        showDisplay = true;
      }
      else
      {
        // ถ้าอ่านไม่สำเร็จ ให้แสดง error
        // Serial.println("error opening SD Card");
        lcd.setCursor(0, 2);
        lcd.print("  SD Card Error !   ");
      }
    }
    if (digitalRead(DOWN_BUTTON) == LOW && displayDim == false)
    {
      beep();
      count++;
      if (count > historyCount)
      {
        count = 1;
      }
      showDisplay = true;
      last4 = millis();
      last5 = millis();
      delay(200);
    }
    if (digitalRead(UP_BUTTON) == LOW && displayDim == false)
    {
      beep();
      count--;
      if (count < 1)
      {
        count = historyCount;
      }
      showDisplay = true;
      last4 = millis();
      last5 = millis();
      delay(200);
    }
    if (!digitalRead(MODE_BUTTON) && buttonState == 4)
    {
      beep();
      delay(200);
      break;
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
  }
}
void setOverTemp()
{
  bool exit = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  OverTemp ShutOFF  ");
  lcd.setCursor(8, 2);
  lcd.print(overTemp);
  lcd.print(" C");
  lcd.print((char)223);
  delay(1000);
  while (!exit)
  {
    if (!digitalRead(UP_BUTTON))
    {
      beep();
      overTemp++;
      if (overTemp > 180)
      {
        overTemp = 10;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  OverTemp ShutOFF  ");
      lcd.setCursor(8, 2);
      lcd.print(overTemp);
      lcd.print(" C");
      lcd.print((char)223);
      delay(200);
      while (!digitalRead(UP_BUTTON))
        ;
    }
    if (!digitalRead(DOWN_BUTTON))
    {
      beep();
      overTemp--;
      if (overTemp < 10)
      {
        overTemp = 180;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  OverTemp ShutOFF  ");
      lcd.setCursor(8, 2);
      lcd.print(overTemp);
      lcd.print(" C");
      lcd.print((char)223);
      delay(200);
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (!digitalRead(MODE_BUTTON))
    {
      beep();
      EEPROM.put(10, overTemp);
      break;
      while (!digitalRead(MODE_BUTTON))
        ;
    }
  }
}
void setCoolFault_delay()
{
  bool exit = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Cooling Fault Delay");
  lcd.setCursor(6, 2);
  lcd.print(coolingFault_delay);
  lcd.print(" Second");

  delay(1000);
  while (!exit)
  {
    if (!digitalRead(UP_BUTTON))
    {
      beep();
      coolingFault_delay = coolingFault_delay + 1;
      if (coolingFault_delay > 60)
      {
        coolingFault_delay = 0;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Cooling Fault Delay");
      lcd.setCursor(6, 2);
      lcd.print(coolingFault_delay);
      lcd.print(" Second");
      delay(200);
      while (!digitalRead(UP_BUTTON))
        ;
    }
    if (!digitalRead(DOWN_BUTTON))
    {
      beep();
      coolingFault_delay = coolingFault_delay - 1;
      if (coolingFault_delay < 0)
      {
        coolingFault_delay = 60;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Cooling Fault Delay");
      lcd.setCursor(6, 2);
      lcd.print(coolingFault_delay);
      lcd.print(" Second");
      delay(200);
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (!digitalRead(MODE_BUTTON))
    {
      beep();
      EEPROM.put(20, coolingFault_delay);
      break;
      while (!digitalRead(MODE_BUTTON))
        ;
    }
  }
}
void setDimTime()
{
  bool exit = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Sleep Mode Timer  ");
  lcd.setCursor(8, 2);
  lcd.print(dimTime);
  lcd.print(" Minute");
  delay(1000);
  while (!exit)
  {
    if (!digitalRead(UP_BUTTON))
    {
      beep();
      dimTime++;
      if (dimTime > 30)
      {
        dimTime = 1;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  Sleep Mode Timer  ");
      lcd.setCursor(8, 2);
      lcd.print(dimTime);
      lcd.print(" Minute");
      delay(200);
      while (!digitalRead(UP_BUTTON))
        ;
    }
    if (!digitalRead(DOWN_BUTTON))
    {
      beep();
      dimTime--;
      if (dimTime < 1)
      {
        dimTime = 30;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  Sleep Mode Timer  ");
      lcd.setCursor(8, 2);
      lcd.print(dimTime);
      lcd.print(" Minute");

      delay(200);
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (!digitalRead(MODE_BUTTON))
    {
      beep();
      EEPROM.put(30, dimTime);
      break;
      while (!digitalRead(MODE_BUTTON))
        ;
    }
  }
}
void setEngineRpm()
{
  bool exit = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Engine Start (RMP) ");
  lcd.setCursor(8, 2);
  lcd.print(engineRPM);
  lcd.print(" RPM");

  delay(1000);
  while (!exit)
  {
    if (!digitalRead(UP_BUTTON))
    {
      beep();
      engineRPM = engineRPM + 100;
      if (engineRPM > 30000)
      {
        engineRPM = 500;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Engine Start (RMP) ");
      lcd.setCursor(8, 2);
      lcd.print(engineRPM);
      lcd.print(" RPM");
      delay(200);
      while (!digitalRead(UP_BUTTON))
        ;
    }
    if (!digitalRead(DOWN_BUTTON))
    {
      beep();
      engineRPM = engineRPM - 100;
      if (engineRPM < 500)
      {
        engineRPM = 30000;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Engine Start (RMP) ");
      lcd.setCursor(8, 2);
      lcd.print(engineRPM);
      lcd.print(" RPM");

      delay(200);
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (!digitalRead(MODE_BUTTON))
    {
      beep();
      EEPROM.put(40, engineRPM);
      break;
      while (!digitalRead(MODE_BUTTON))
        ;
    }
  }
}
void enterPassword()
{
  int cursor = 5;
  int oldCursor;
  int currentDigit = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Enter Password   ");
  lcd.setCursor(0, 1);
  lcd.print("     0123456789     ");
  lcd.setCursor(0, 3);
  lcd.print("Reset         Cancel");
  delay(500);
  lcd.setCursor(cursor, 1);
  lcd.blink();
  while (!password.evaluate())
  {
    if (!digitalRead(DOWN_BUTTON))
    {
      beep();
      cursor++;
      if (cursor > 16)
      {
        cursor = 5;
      }
      if (cursor < 15)
      {
        lcd.setCursor(cursor, 1);
      }
      if (cursor == 15)
      {
        lcd.setCursor(0, 3);
      }
      if (cursor == 16)
      {
        lcd.setCursor(14, 3);
      }
      reset = false;
      delay(200);
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (!digitalRead(UP_BUTTON))
    {
      beep();
      cursor--;
      if (cursor < 5)
      {
        cursor = 16;
      }
      if (cursor < 15)
      {
        lcd.setCursor(cursor, 1);
      }
      if (cursor == 15)
      {
        lcd.setCursor(0, 3);
      }
      if (cursor == 16)
      {
        lcd.setCursor(14, 3);
      }
      reset = false;
      delay(200);
      while (!digitalRead(UP_BUTTON))
        ;
    }

    if (!digitalRead(MODE_BUTTON))
    {
      // beep();
      if (cursor == 5)
      {
        password.append('0');
      }
      else if (cursor == 6)
      {
        password.append('1');
      }
      else if (cursor == 7)
      {
        password.append('2');
      }
      else if (cursor == 8)
      {
        password.append('3');
      }
      else if (cursor == 9)
      {
        password.append('4');
      }
      else if (cursor == 10)
      {
        password.append('5');
      }
      else if (cursor == 11)
      {
        password.append('6');
      }
      else if (cursor == 12)
      {
        password.append('7');
      }
      else if (cursor == 13)
      {
        password.append('8');
      }
      else if (cursor == 14)
      {
        password.append('9');
      }
      lcd.setCursor(8 + currentDigit, 2);
      lcd.print("*");
      currentDigit++;

      if (cursor == 15)
      {
        password.reset();
        currentDigit = 0;
        lcd.setCursor(0, 2);
        lcd.print("                    ");
        reset = true;
        cursor = 5;
        lcd.setCursor(5, 1);
      }
      if (cursor == 16)
      {
        lcd.noBlink();
        break;
      }
      if (currentDigit == 4)
      {
        if (password.evaluate())
        {
          tone(BUZZER_PIN, 523, 0);
          delay(50);
          noTone(BUZZER_PIN);
          delay(50);
          tone(BUZZER_PIN, 523, 0);
          delay(200);
          noTone(BUZZER_PIN);
          Serial.println("Password OK");
          lcd.setCursor(0, 2);
          lcd.print("    Password OK     ");
          lcd.noBlink();
          lcd.setCursor(0, 3);
          delay(1000);
          excuteAction();
          break;
        }
        else
        {
          Serial.println("Password Wrong !");
          tone(BUZZER_PIN, 400, 0);
          delay(200);
          noTone(BUZZER_PIN);
          lcd.setCursor(0, 2);
          lcd.print("  Password Wrong !  ");
          lcd.setCursor(oldCursor, 1);
          delay(1000);
        }
      }
      else
      {
        beep();
      }

      if (currentDigit > 3)
      {
        password.reset();
        currentDigit = 0;
        lcd.setCursor(0, 2);
        lcd.print("                    ");
      }
      if (cursor != 15)
      {
        oldCursor = cursor;
        lcd.setCursor(oldCursor, 1);
      }
      delay(200);
      while (!digitalRead(MODE_BUTTON))
        ;
    }
  }
}
void changePassword()
{
  int cursor = 5;
  int oldCursor;
  int currentDigit = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Enter New Password ");
  lcd.setCursor(0, 1);
  lcd.print("     0123456789     ");
  lcd.setCursor(0, 3);
  lcd.print("OK            Cancel");
  delay(500);
  lcd.setCursor(cursor, 1);
  lcd.blink();
  while (!changePasswordDone)
  {
    if (!digitalRead(DOWN_BUTTON))
    {
      beep();
      cursor++;
      if (cursor > 16)
      {
        cursor = 5;
      }
      if (cursor < 15)
      {
        lcd.setCursor(cursor, 1);
      }
      if (cursor == 15)
      {
        lcd.setCursor(0, 3);
      }
      if (cursor == 16)
      {
        lcd.setCursor(14, 3);
      }
      delay(200);
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (!digitalRead(UP_BUTTON))
    {
      beep();
      cursor--;
      if (cursor < 5)
      {
        cursor = 16;
      }
      if (cursor < 15)
      {
        lcd.setCursor(cursor, 1);
      }
      if (cursor == 15)
      {
        lcd.setCursor(0, 3);
      }
      if (cursor == 16)
      {
        lcd.setCursor(14, 3);
      }
      delay(200);
      while (!digitalRead(UP_BUTTON))
        ;
    }

    if (!digitalRead(MODE_BUTTON))
    {
      beep();
      if (currentDigit < 5 && cursor < 15)
      {
        if (cursor == 5)
        {
          newPasswordString = newPasswordString + "0";
        }
        else if (cursor == 6)
        {
          newPasswordString = newPasswordString + "1";
        }
        else if (cursor == 7)
        {
          newPasswordString = newPasswordString + "2";
        }
        else if (cursor == 8)
        {
          newPasswordString = newPasswordString + "3";
        }
        else if (cursor == 9)
        {
          newPasswordString = newPasswordString + "4";
        }
        else if (cursor == 10)
        {
          newPasswordString = newPasswordString + "5";
        }
        else if (cursor == 11)
        {
          newPasswordString = newPasswordString + "6";
        }
        else if (cursor == 12)
        {
          newPasswordString = newPasswordString + "7";
        }
        else if (cursor == 13)
        {
          newPasswordString = newPasswordString + "8";
        }
        else if (cursor == 14)
        {
          newPasswordString = newPasswordString + "9";
        }
        Serial.print("enter = ");
        Serial.println(newPasswordString);
        lcd.setCursor(8 + currentDigit, 2);
        lcd.print("*");
        currentDigit++;
      }
      if (cursor == 15)
      {
        if (currentDigit == 4)
        {
          newPasswordString.toCharArray(newPassword, newPasswordString.length() + 1);
          password.set(newPassword);
          password.reset();
          writeStringToEEPROM(50, newPasswordString);
          Serial.print("Password changed to ");
          Serial.println(newPassword);
          lcd.setCursor(0, 2);
          lcd.print("   Change Password  ");
          changePasswordDone = true;
          currentDigit = 0;
          newPasswordString = "";
          lcd.noBlink();
          lcd.setCursor(0, 3);
          delay(1000);
          break;
        }
      }
      if (cursor == 16)
      {
        lcd.noBlink();
        break;
      }
      if (currentDigit > 4)
      {
        tone(BUZZER_PIN, 523, 0);
        delay(100);
        noTone(BUZZER_PIN);
        newPasswordString = "";
        currentDigit = 0;
        lcd.setCursor(0, 2);
        lcd.print("                    ");
      }
      if (cursor != 15)
      {
        oldCursor = cursor;
        lcd.setCursor(oldCursor, 1);
      }
      delay(200);
      while (!digitalRead(MODE_BUTTON))
        ;
    }
  }
}
void removeAllHistory()
{
  bool exit = false;
  int state = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Remove All History ?");
  lcd.setCursor(0, 2);
  lcd.print("         NO         ");
  delay(1000);
  while (!exit)
  {
    if (!digitalRead(UP_BUTTON))
    {
      beep();
      state++;
      if (state > 1)
      {
        state = 0;
      }
      delay(200);
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (!digitalRead(DOWN_BUTTON))
    {
      beep();
      state--;
      if (state < 0)
      {
        state = 1;
      }
      delay(200);
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (!digitalRead(MODE_BUTTON))
    {
      beep();
      if (SD.exists("data.txt"))
      {
        SD.remove("data.txt");
        Serial.println("Remove data.txt done");
        lcd.setCursor(0, 2);
        lcd.print("Delete All Data Done");
        delay(1000);
        writeDataLoger("Delete History,");
      }
      else
      {
        Serial.println("data.txt doesn't exist.");
        lcd.setCursor(0, 2);
        lcd.print("Cannot delete data! ");
        delay(1000);
      }
      break;
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (state == 0)
    {
      lcd.setCursor(0, 2);
      lcd.print("         NO         ");
    }
    if (state == 1)
    {
      lcd.setCursor(0, 2);
      lcd.print("        Yes!        ");
    }
  }
}
void setNoBeep()
{
  bool exit = false;
  int state = noBeep;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Push Button Beep  ");
  lcd.setCursor(0, 2);
  if (!noBeep)
  {
    lcd.print("        Yes         ");
  }
  else
  {
    lcd.print("        No          ");
  }
  delay(1000);
  while (!exit)
  {
    if (!digitalRead(UP_BUTTON))
    {
      beep();
      state++;
      if (state > 1)
      {
        state = 0;
      }
      delay(200);
      while (!digitalRead(UP_BUTTON))
        ;
    }
    if (!digitalRead(DOWN_BUTTON))
    {
      beep();
      state--;
      if (state < 0)
      {
        state = 1;
      }
      delay(200);
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (state == 0)
    {
      noBeep = false;
      lcd.setCursor(0, 2);
      lcd.print("        Yes         ");
    }
    if (state == 1)
    {
      noBeep = true;
      lcd.setCursor(0, 2);
      lcd.print("        No          ");
    }
    if (!digitalRead(MODE_BUTTON))
    {
      EEPROM.put(60, noBeep);
      break;
      while (!digitalRead(MODE_BUTTON))
        ;
    }
  }
}
void excuteAction()
{
  if (password.evaluate())
  {
    switch (menu)
    {
    case 1:
      setOverTemp();
      break;
    case 2:
      setCoolFault_delay();
      break;
    case 3:
      setDimTime();
      break;
    case 4:
      setEngineRpm();
      break;
    case 5:
      showDisplay = true;
      history();
      break;
    case 6:
      changePassword();
      changePasswordDone = false;
      break;
    case 7:
      removeAllHistory();
      break;
    case 8:
      setNoBeep();
      break;
    }
  }
  else
  {
    enterPassword();
  }
}
void page1()
{
  if (showDisplay == true)
  {
    Serial.println("show Display");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ENGINE :");
    lcd.setCursor(0, 1);
    lcd.print("OIL  PRESSURE:");
    lcd.setCursor(0, 2);
    lcd.print("COOLING SYST.:");
    lcd.setCursor(0, 3);
    lcd.write(0);
    lcd.write(1);
    lcd.print(" TEMP:");
    lcd.print(temp, 1);
    lcd.print(" C");
    lcd.print((char)223);
    if (engineRun == true)
    {
      lcd.setCursor(9, 0);
      lcd.print("RUN");
    }
    if (engineRun == false)
    {
      lcd.setCursor(9, 0);
      lcd.print("STOP");
    }
    if (oilPress == true)
    {
      lcd.setCursor(14, 1);
      lcd.print("NORMAL");
    }
    if (oilPress == false)
    {
      lcd.setCursor(14, 1);
      lcd.print("LOW");
    }
    if (coolSys == true)
    {
      lcd.setCursor(14, 2);
      lcd.print("NORMAL");
    }
    if (coolSys == false)
    {
      lcd.setCursor(14, 2);
      lcd.print("LOW");
    }
    if (cooling_fault == true && engineRun == true && coolSys == false)
    {
      lcd.setCursor(14, 2);
      lcd.print("FAULT ");
    }

    showDisplay = false;
  }
  if (newTemp != temp)
  {
    temp = newTemp;
    // Serial.println(temp);
    lcd.setCursor(0, 3);
    lcd.write(0);
    lcd.write(1);
    lcd.print(" TEMP:");
    lcd.print(temp, 1);
    lcd.print(" C");
    lcd.print((char)223);
  }
  if (displayDim == false && millis() - last4 >= (dimTime * 60000))
  {
    displayOff();
    displayDim = true;
    last4 = millis();
  }
}
void page2()
{
  DateTime now = rtc.now();
  if (showDisplay == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WATER TANK :");
    lcd.setCursor(0, 1);
    lcd.print("DATE :");
    lcd.setCursor(6, 1);
    lcd.print(now.day());
    lcd.print("-");
    if (now.month() < 10)
    {
      lcd.print("0");
    }
    lcd.print(now.month());
    lcd.print("-");
    lcd.print(now.year());

    lcd.setCursor(0, 2);
    lcd.print("TIME :");
    lcd.setCursor(0, 3);
    // lcd.print("ENGINE RUN :");
    lcd.write(0);
    lcd.write(1);
    lcd.print(" RUN : ");

    int countHour = engineRunTime / 3600;
    Serial.print("CountHour = ");
    Serial.println(countHour);
    int countMin = engineRunTime / 60;
    Serial.print("CountMin = ");
    Serial.println(countMin);

    lcd.print(countHour);
    lcd.print("H");
    lcd.print(countMin);
    lcd.print("M");
    showDisplay = false;
  }
  if (waterTank == false)
  {
    lcd.setCursor(12, 0);
    lcd.print("NORMAL");
  }
  if (waterTank == true)
  {
    lcd.setCursor(12, 0);
    lcd.print("LOW   ");
  }

  lcd.setCursor(6, 2);
  if (now.hour() < 10)
  {
    lcd.print("0");
  }
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute() < 10)
  {
    lcd.print("0");
  }
  lcd.print(now.minute());
  lcd.print(":");
  if (now.second() < 10)
  {
    lcd.print("0");
  }
  lcd.print(now.second());
}
void page3()
{

  if (showDisplay == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ENGINE RPM :");
    lcd.print(RPM);
    lcd.setCursor(0, 1);
    lcd.print("Ambient TEMP :");
    lcd.print(rtc.getTemperature(), 0.1);
    lcd.print(" C");
    lcd.print((char)223);
    showDisplay = false;
  }
  lcd.print("     ");
  lcd.setCursor(12, 0);
  lcd.print(RPM);
}
void page4()
{
  if (showDisplay == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SETTING Sys. ");
    lcd.write(2);
    lcd.write(3);
    lcd.print(" ");
    lcd.print("Out");
    lcd.write(7);

    lcd.setCursor(0, 1);
    lcd.print(" Over Temp:");
    lcd.print(overTemp);
    lcd.print(" C");
    lcd.print((char)223);
    lcd.setCursor(0, 2);
    lcd.print(" Cool'F Delay:");
    lcd.print(coolingFault_delay);
    lcd.print(" S.");
    lcd.setCursor(0, 3);
    lcd.print(" Sleep Mode:");
    lcd.print(dimTime);
    lcd.print(" M.");
    showDisplay = false;
  }
  if (!digitalRead(DOWN_BUTTON))
  {
    beep();
    menu++;
    settingMode = true;

    if (menu > 8)
    {
      menu = 0;
    }
    updateMenu();
    last4 = millis();
    last5 = millis();
    delay(200);
    while (!digitalRead(DOWN_BUTTON))
      ;
  }
  if (!digitalRead(UP_BUTTON))
  {
    beep();
    menu--;
    settingMode = true;
    if (menu < 0)
    {
      menu = 8;
    }
    updateMenu();
    last4 = millis();
    last5 = millis();
    delay(200);
    while (!digitalRead(UP_BUTTON))
      ;
  }
  if (!digitalRead(MODE_BUTTON) && settingMode == true)
  {
    beep();
    excuteAction();
    updateMenu();
    last4 = millis();
    last5 = millis();
    delay(200);
    while (!digitalRead(MODE_BUTTON))
      ;
  }
}
void readSilenceAlarmButton()
{
  if (digitalRead(SILENCE_ALARM_BUTTON) == LOW)
  {
    beep();
    silence_alarm = true;
    buzzerAlarmON = false;
    writeDataLoger("Silence Alarm,");
    if (tempFault == false || armedSw == false)
    {
      showDisplay = true;
      page = 1;
      page1();
    }
    delay(200);
  }
  buzzerAlarm();
}
void readTemp()
{
  if (millis() - timer >= measurementPeriod)
  {
    timer += measurementPeriod;
    // tempFault = true;
    newTemp = thermocouple.readCelsius();
    // delay(500);

    // if (temp < 0)
    // {
    //   temp = 0;
    // }
    if (temp >= overTemp)
    {
      bool newOverTemp = true;
      if (newOverTemp != tempFault)
      {
        tempFault = newOverTemp;
        showDisplay = true;
        buzzerAlarmON = true;
        last6 = millis();
      }
      page = 0;
      engineOverTemp();
    }
  }

  // if (temp < overTemp)
  // {
  //   bool newOverTemp = false;
  //   if (newOverTemp != tempFault)
  //   {
  //     tempFault = newOverTemp;
  //     buzzerAlarmON = false;
  //     //last6 = millis();
  //   }
  // }
}
void checkModePageButton()
{
  if (digitalRead(MODE_BUTTON) == LOW && displayDim == false && settingMode == false)
  {
    beep();
    read = false;
    buttonState++;
    count = 0;
    if (buttonState > 4)
    {
      buttonState = 1;
    }
    Serial.println(buttonState);
    showDisplay = true;
    last4 = millis();
    last5 = millis();
    delay(200);
  }

  if (buttonState == 1 && displayDim == false)
  {
    page1();
  }
  if (buttonState == 2)
  {
    page = 2;
    page2();
  }
  if (buttonState == 3)
  {
    page = 3;
    page3();
  }
  if (buttonState == 4)
  {
    page = 4;
    page4();
  }
  if (digitalRead(UP_BUTTON) == LOW || digitalRead(DOWN_BUTTON) == LOW || digitalRead(MODE_BUTTON) == LOW || showDisplay == true)
  {
    last4 = millis();
    if (displayDim == true)
    {
      displayOn();
      displayDim = false;
      delay(200);
    }
  }
  if (buttonState != 1 && millis() - last5 > pageOtherTime)
  {
    page = 1;
    buttonState = 1;
    count = 0;
    read = false;
    last4 = millis();
    last5 = millis();
    showDisplay = true;
    settingMode = false;
    page1();
  }
}
void readResetButton()
{
  if (digitalRead(RESET_BUTTON) == LOW)
  {
    beep();
    if (displayDim == true)
    {
      displayOn();
      displayDim = false;
    }
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("        RESET       ");
    noTone(BUZZER_PIN);
    writeDataLoger("User Reset,");
    delay(5000);

    wdt_enable(WDTO_15MS);
    while (1)
    {
    }
  }
}
void readAcc()
{
  if (digitalRead(FIREPUMP_ACC) == LOW && engineShutOff == true)
  {
    if (displayDim == true)
    {
      displayOn();
      displayDim = false;
    }
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("        RESET       ");
    noTone(BUZZER_PIN);
    writeDataLoger("Firepump Reset,");
    delay(5000);
    wdt_enable(WDTO_15MS);
    while (1)
    {
    }
  }
}
void Pulse_Event() // The interrupt runs this to calculate the period between pulses:
{
  PeriodBetweenPulses = micros() - LastTimeWeMeasured; // Current "micros" minus the old "micros" when the last pulse happens.
                                                       // This will result with the period (microseconds) between both pulses.
                                                       // The way is made, the overflow of the "micros" is not going to cause any issue.

  LastTimeWeMeasured = micros(); // Stores the current micros so the next time we have a pulse we would have something to compare with.

  if (PulseCounter >= AmountOfReadings) // If counter for amount of readings reach the set limit:
  {
    PeriodAverage = PeriodSum / AmountOfReadings; // Calculate the final period dividing the sum of all readings by the
                                                  // amount of readings to get the average.
    PulseCounter = 1;                             // Reset the counter to start over. The reset value is 1 because its the minimum setting allowed (1 reading).
    PeriodSum = PeriodBetweenPulses;              // Reset PeriodSum to start a new averaging operation.

    // Change the amount of readings depending on the period between pulses.
    // To be very responsive, ideally we should read every pulse. The problem is that at higher speeds the period gets
    // too low decreasing the accuracy. To get more accurate readings at higher speeds we should get multiple pulses and
    // average the period, but if we do that at lower speeds then we would have readings too far apart (laggy or sluggish).
    // To have both advantages at different speeds, we will change the amount of readings depending on the period between pulses.
    // Remap period to the amount of readings:
    int RemapedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10); // Remap the period range to the reading range.
    // 1st value is what are we going to remap. In this case is the PeriodBetweenPulses.
    // 2nd value is the period value when we are going to have only 1 reading. The higher it is, the lower RPM has to be to reach 1 reading.
    // 3rd value is the period value when we are going to have 10 readings. The higher it is, the lower RPM has to be to reach 10 readings.
    // 4th and 5th values are the amount of readings range.
    RemapedAmountOfReadings = constrain(RemapedAmountOfReadings, 1, 10); // Constrain the value so it doesn't go below or above the limits.
    AmountOfReadings = RemapedAmountOfReadings;                          // Set amount of readings as the remaped value.
  }
  else
  {
    PulseCounter++;                              // Increase the counter for amount of readings by 1.
    PeriodSum = PeriodSum + PeriodBetweenPulses; // Add the periods so later we can average.
  }
}
void checkPowerOff()
{
  float volt = analogRead(A0) * 6.6 / 1023; // อ่านค่าแรงดัน VCC
  if (volt < 4.2 && powerOff == false)
  {
    myFile = SD.open("data.txt", FILE_WRITE); // เปิดไฟล์เพื่อเขียนข้อมูล โหมด FILE_WRITE
    // ถ้าเปิดไฟล์สำเร็จ ให้เขียนข้อมูลเพิ่มลงไป
    if (myFile)
    {
      DateTime now = rtc.now();
      myFile.print("Power OFF,"); // สั่งให้เขียนข้อมูล
      myFile.print(now.day());
      myFile.print("-");
      if (now.month() < 10)
      {
        myFile.print("0");
      }
      myFile.print(now.month());
      myFile.print("-");
      myFile.print(now.year());
      myFile.print(" ");
      myFile.print(now.hour());
      myFile.print(":");
      if (now.minute() < 10)
      {
        myFile.print("0");
      }
      myFile.print(now.minute());
      myFile.print(":");
      if (now.second() < 10)
      {
        myFile.print("0");
      }
      myFile.println(now.second());

      myFile.close(); // ปิดไฟล์
      Serial.println("Power-OFF Save done.");
    }
    else
    {
      // ถ้าเปิดไฟล์ไม่สำเร็จ ให้แสดง error
      Serial.println("Power-OFF error opening File");
    }
    powerOff = true;
  }
}
void setup()
{
  // Button INPUT_PULLUP ใช้เฉพาะ Simulator**
  pinMode(UP_BUTTON, INPUT);
  pinMode(DOWN_BUTTON, INPUT);
  pinMode(MODE_BUTTON, INPUT);
  pinMode(ABORT_BUTTON, INPUT);
  pinMode(RESET_BUTTON, INPUT);
  pinMode(SILENCE_ALARM_BUTTON, INPUT);
  pinMode(DISABLE_SW, INPUT); // Sw Armed-Disable
  // Software INPUT_PULLUP ใช้เฉพาะ Simulator**
  pinMode(FLOW_SW, INPUT);      // Flow Sw ระบบหล่อเย็น Narmal Close
  pinMode(OIL_PRES_SW, INPUT);  // Oil Pressor ON=12/24V.
  pinMode(WATER_TANK, INPUT);   // เช็คระดับน้ำในบ่อ Normal Close
  pinMode(FIREPUMP_ACC, INPUT); // ไฟจากตู้ FIREPUMP ACC
  pinMode(PULSEPIN, INPUT);

  pinMode(LED_YELLOW, OUTPUT);           // LED Fault
  pinMode(LED_SHUTOFF, OUTPUT);          // LED ShutOFF
  pinMode(LED_DISABLE, OUTPUT);          // LED Disable-Armed
  pinMode(BUZZER_PIN, OUTPUT);           // Buzzer Alarm
  pinMode(BELL_PIN, OUTPUT);             // Bell Alarm
  pinMode(ENGINE_RELAY_SHUTOFF, OUTPUT); // Relay ShutOFF

  pinMode(chipSelect, OUTPUT);
  analogReference(INTERNAL1V1);                                    // เลือกใช้แรงดันอ้างอิงจากภายใน 1.1V
  attachInterrupt(digitalPinToInterrupt(18), Pulse_Event, RISING); // Enable interrupt LOW to HIGH.

  digitalWrite(BELL_PIN, HIGH);
  digitalWrite(ENGINE_RELAY_SHUTOFF, HIGH);
  Serial.begin(115200);
  Serial.println("---------------------");
  Serial.println("Safety Engine Control");
  Serial.print("Ver.  ");
  Serial.println(version);

  lcd.init();
  lcd.createChar(0, engineChar0);
  lcd.createChar(1, engineChar1);
  lcd.createChar(2, upChar);
  lcd.createChar(3, downChar);
  lcd.createChar(4, spkChar);
  lcd.createChar(5, soundChar);
  lcd.createChar(6, muteChar);
  lcd.createChar(7, enterChar);

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("--------------------");
  lcd.setCursor(0, 1);
  lcd.print("SafetyEngine Control");
  lcd.setCursor(0, 2);
  lcd.print(" Verifying Systems  ");
  lcd.setCursor(0, 3);
  lcd.print("--------------------");
  delay(500);

  // Test LED Boot
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_SHUTOFF, HIGH);
  digitalWrite(LED_DISABLE, HIGH);
  delay(500);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_SHUTOFF, LOW);
  digitalWrite(LED_DISABLE, LOW);
  delay(500);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_SHUTOFF, HIGH);
  digitalWrite(LED_DISABLE, HIGH);
  delay(500);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_SHUTOFF, LOW);
  digitalWrite(LED_DISABLE, LOW);

  readEeprom();
  timer = millis();

  // ts.setOffset(0);
  //  set offset for temperature measurement.
  //  1 stannds for 0.25 Celsius

  if (!rtc.begin())
  {
    lcd.setCursor(0, 2); //
    lcd.print(" Couldn't find RTC  ");
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }
  else
  {
    lcd.setCursor(0, 2); //
    lcd.print("  Date Time is OK   ");
    Serial.println("Date Time is OK");
    delay(1000);
  }
  // rtc.adjust(DateTime(__DATE__, __TIME__)); // ตั้งค่าเวลาให้ตรงกับคอมพิวเตอร์
  showTimeNow();
  verifySD();
  writeDataLoger("Power ON,");

  tone(BUZZER_PIN, 523, 0);
  delay(80);
  noTone(BUZZER_PIN);
  delay(200);
  // startTone();
  lcd.clear();
  showDisplay = true;
}
void loop()
{
  readTemp();
  readWaterTank();
  readOilPressSw();
  readCoolSys();
  readDisableSw();
  checkEngineRun();
  readSilenceAlarmButton();
  checkModePageButton();
  readResetButton();
  readAcc();
  checkPowerOff();
}
