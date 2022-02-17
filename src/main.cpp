/*
   Safety Engine Control (PT&E) **พี่อมร
   By Arthit_N ... Tel.0959058972  E-Mail:tgbroadcast@gmail.com
   Kickoff 26/01/2022
   Ver.  0.9.0
   put on github 26/01/2022
*/

//**Mega : MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 53(pinMode OUTPUT**)
#include <avr/wdt.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <stdio.h>
#include <string.h>
#include <Password.h>

#define SILENCE_ALARM 9
#define TEST_PIN 10
#define ARMED_PIN 11
#define LED_YELLOW 15
#define LED_RED 16
#define BUZZER_PIN 17
#define FLOW_SW 18
#define OIL_PRES_SW 19
#define WATER_TANK 12

#define RESET_BUTTON 36
#define UP_BUTTON 38
#define DOWN_BUTTON 39
#define MODE_BUTTON 40
#define ABORT_BUTTON 41

LiquidCrystal_I2C lcd(0x27, 20, 4);
Password password = Password("0000");
unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;
unsigned long currentMillis;
unsigned long currentMillis1;
unsigned long countSec;
unsigned long last1 = 0,
              last2 = 0, last3 = 0, last4 = 0, last5, last6;
unsigned long waterTankFalut_verify = 15000;
unsigned long coolingfalut_verify = 5000;
unsigned long coolingfalut_delay;
unsigned long dimTime;
unsigned long pageOtherTime = 60000;
unsigned long engineRunTime;

bool engineShutOff = false;
bool goodDateTime;
bool testSw = false;
bool armedSw = false;
bool engineRun = false;
bool engineStart = false;
bool coolSys = false;
bool oilPress = false;
bool waterTank = false;
bool warning = false;
bool cooling_falut = false;
bool cooling_fault_delay_start = false;
bool cooling_fault_verify_start = false;
bool settingMode = false;
bool showDisplay = false;
bool displayDim = false;
bool silence_alarm = false;
bool tempFault = false;
int page = 1;
float temp = 35.0;
int overTemp;
int historyCount = 0;
String buffer;
bool read = false;
unsigned long position[1000];
int count = 0;
int buttonState = 1;
int menu = 0;
int countDown = 60;

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
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

File myFile; // สร้างออฟเจก File สำหรับจัดการข้อมูล
const int chipSelect = 53;

void writeDataLoger(String event)
{
  myFile = SD.open("data.txt", FILE_WRITE); // เปิดไฟล์เพื่อเขียนข้อมูล โหมด FILE_WRITE

  // ถ้าเปิดไฟล์สำเร็จ ให้เขียนข้อมูลเพิ่มลงไป
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

    myFile.close(); // ปิดไฟล์
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
      // Serial.write(myFile.read());
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
    lcd.setCursor(0, 2); //
    lcd.print("  SD Card Failed!   ");
    return;
  }
  Serial.println("initialization done.");
  lcd.setCursor(0, 2); //
  lcd.print("   SD Card is OK    ");
  delay(1000);

  // SD.remove("data.txt");
}
void readEeprom()
{
  EEPROM.begin();
  engineRunTime = EEPROM.read(0);
  overTemp = EEPROM.read(10);
  coolingfalut_delay = EEPROM.read(20);
  dimTime = EEPROM.read(30);

  if (engineRunTime < 0 || engineRunTime > 4294967294)
  {
    engineRunTime = 0;
  }
  if (overTemp < 10 || overTemp > 180)
  {
    overTemp = 95;
  }
  if (coolingfalut_delay < 1000 || coolingfalut_delay > 30000)
  {
    coolingfalut_delay = 5000;
  }
  if (dimTime < 1 || dimTime > 30)
  {
    dimTime = 15;
  }

  Serial.print("EngineRunTime = ");
  Serial.println(engineRunTime);
  Serial.print("Over Temp = ");
  Serial.println(overTemp);
  Serial.print("Cooling Fault Delay = ");
  Serial.print(coolingfalut_delay / 1000);
  Serial.println(" S.");
  Serial.print("Dimer LCD Time = ");
  Serial.print(dimTime);
  Serial.println(" M.");
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
  tone(BUZZER_PIN, 523, 0);
  delay(40);
  noTone(BUZZER_PIN);
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

  Serial.println(daysOfTheWeek[now.dayOfTheWeek()]);
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

void setup()
{
  
  Serial.begin(115200);
  Serial.println("------------------------------------------------");
  Serial.println("Safety Engine Control");
  Serial.println("Ver.  0.9.0");

  lcd.init(); // initialize the lcd
  lcd.createChar(0, engineChar0);
  lcd.createChar(1, engineChar1);
  lcd.createChar(2, upChar);
  lcd.createChar(3, downChar);

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0); //
  lcd.print("--------------------");
  lcd.setCursor(0, 1); //
  lcd.print("SafetyEngine Control");
  lcd.setCursor(0, 2); //
  lcd.print(" Verifying Systems  ");
  lcd.setCursor(0, 3); //
  lcd.print("--------------------");
  // delay(1000);

  pinMode(TEST_PIN, INPUT_PULLUP);
  pinMode(ARMED_PIN, INPUT_PULLUP);
  pinMode(FLOW_SW, INPUT_PULLUP);
  pinMode(OIL_PRES_SW, INPUT_PULLUP);
  pinMode(WATER_TANK, INPUT_PULLUP);
  pinMode(SILENCE_ALARM, INPUT_PULLUP);

  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(ABORT_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);

  pinMode(chipSelect, OUTPUT);

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

  readEeprom();

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
  // rtc.adjust(DateTime(__DATE__, __TIME__));
  showTimeNow();

  verifySD();
  writeDataLoger("Power ON,");

  beep();
  delay(200);
  // startTone();
  lcd.clear();
  showDisplay = true;
}

void silenceAlarmReset()
{
  bool newsilenceAlarm = false;
  if (newsilenceAlarm != silence_alarm)
  {
    silence_alarm = newsilenceAlarm;
  }
}
void buzzerAlarm()
{
  if (silence_alarm == false && engineShutOff == false)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 500)
    {
      noTone(BUZZER_PIN);
      if (currentMillis - previousMillis >= 1000)
      {
        tone(BUZZER_PIN, 800, 0);
        previousMillis = currentMillis;
      }
    }
  }
}
void cooling_fault()
{
  digitalWrite(LED_YELLOW, HIGH);
  buzzerAlarm();
  if (showDisplay == true && silence_alarm == false)
  {
    silenceAlarmReset();

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("   !! WARNING !!    ");
    lcd.setCursor(0, 2);
    lcd.print("COOLING SYSTEM FALUT");
    showDisplay = false;
    writeDataLoger("Cooling System Fault,");
  }
}
void engineOverTemp()
{
  digitalWrite(LED_YELLOW, HIGH);
  buzzerAlarm();
  if (showDisplay == true && silence_alarm == false)
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
  if (millis() - last6 >= 1000 && countDown > 0 && armedSw == true && (digitalRead(ABORT_BUTTON) != LOW))
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

    writeDataLoger("Engine ShutOFF,");
    engineShutOff = true;
  }
  
}
void disable_fault()
{
  digitalWrite(LED_YELLOW, HIGH);
  buzzerAlarm();
  if (showDisplay == true)
  {
    silenceAlarmReset();

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("   !! WARNING !!    ");
    lcd.setCursor(0, 2);
    lcd.print("  SHUTOFF DISABLE   ");
    showDisplay = false;
    writeDataLoger("SHUTOFF DISABLE,");
    showDisplay = false;
  }
}

void readWaterTank()
{
  if (digitalRead(WATER_TANK) == LOW)
  {
    bool newWaterTank = true;
    if (newWaterTank != waterTank)
    {
      last1++;
      Serial.println(last1);
      if (last1 >= waterTankFalut_verify)
      {
        waterTank = newWaterTank;
        showDisplay = true;
      }
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
}
void readFlowSw()
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
}

void readTestSw()
{
  if (digitalRead(TEST_PIN) == LOW)
  {
    testSw = true;
    Serial.println("Test Mode !");
    return;
    digitalWrite(LED_YELLOW, HIGH);
  }
  if (digitalRead(TEST_PIN) == HIGH)
  {
    testSw = false;
    // Serial.println("Normal Mode");
    // return;
    digitalWrite(LED_YELLOW, LOW);
  }
}
void readDisableSw()
{
  if (digitalRead(ARMED_PIN) == LOW)
  {
    bool newArmedSw = false;
    if (newArmedSw != armedSw)
    {
      armedSw = newArmedSw;
      showDisplay = true;
    }
  }
  if (digitalRead(ARMED_PIN) == HIGH)
  {
    bool newArmedSw = true;
    if (newArmedSw != armedSw)
    {
      armedSw = newArmedSw;
      showDisplay = true;
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
    lcd.print("  Systems  Setting  ");
    lcd.setCursor(0, 1);
    lcd.print(" Over Temp:");
    lcd.print(overTemp);
    lcd.print(" C");
    lcd.print((char)223);
    lcd.setCursor(0, 2);
    lcd.print(" Cool'F Delay:");
    lcd.print(coolingfalut_delay / 1000);
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
    lcd.print("  Systems  Setting  ");
    lcd.setCursor(0, 1);
    lcd.print(">Over Temp:");
    lcd.print(overTemp);
    lcd.print(" C");
    lcd.print((char)223);
    lcd.setCursor(0, 2);
    lcd.print(" Cool'F Delay:");
    lcd.print(coolingfalut_delay / 1000);
    lcd.print(" S.");
    lcd.setCursor(0, 3);
    lcd.print(" Sleep Mode:");
    lcd.print(dimTime);
    lcd.print(" M.");
    break;
  case 2:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Systems  Setting  ");
    lcd.setCursor(0, 1);
    lcd.print(" Over Temp:");
    lcd.print(overTemp);
    lcd.print(" C");
    lcd.print((char)223);
    lcd.setCursor(0, 2);
    lcd.print(">Cool'F Delay:");
    lcd.print(coolingfalut_delay / 1000);
    lcd.print(" S.");
    lcd.setCursor(0, 3);
    lcd.print(" Sleep Mode:");
    lcd.print(dimTime);
    lcd.print(" M.");
    break;
  case 3:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Systems  Setting  ");
    lcd.setCursor(0, 1);
    lcd.print(" Over Temp:");
    lcd.print(overTemp);
    lcd.print(" C");
    lcd.print((char)223);
    lcd.setCursor(0, 2);
    lcd.print(" Cool'F Delay:");
    lcd.print(coolingfalut_delay / 1000);
    lcd.print(" S.");
    lcd.setCursor(0, 3);
    lcd.print(">Sleep Mode:");
    lcd.print(dimTime);
    lcd.print(" M.");
    break;
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
      // beep();
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
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (!digitalRead(DOWN_BUTTON))
    {
      // beep();
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
      // beep();
      EEPROM.put(10, overTemp);
      break;
      while (!digitalRead(DOWN_BUTTON))
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
  lcd.print(coolingfalut_delay / 1000);
  lcd.print(" Second");

  delay(1000);
  while (!exit)
  {
    if (!digitalRead(UP_BUTTON))
    {
      // beep();
      coolingfalut_delay = coolingfalut_delay + 1000;
      if (coolingfalut_delay > 30000)
      {
        coolingfalut_delay = 1000;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Cooling Fault Delay");
      lcd.setCursor(6, 2);
      lcd.print(coolingfalut_delay / 1000);
      lcd.print(" Second");
      delay(200);
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (!digitalRead(DOWN_BUTTON))
    {
      // beep();
      coolingfalut_delay = coolingfalut_delay - 1000;
      if (coolingfalut_delay < 1000)
      {
        coolingfalut_delay = 30000;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Cooling Fault Delay");
      lcd.setCursor(6, 2);
      lcd.print(coolingfalut_delay / 1000);
      lcd.print(" Second");
      delay(200);
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (!digitalRead(MODE_BUTTON))
    {
      // beep();
      EEPROM.put(20, coolingfalut_delay);
      break;
      while (!digitalRead(DOWN_BUTTON))
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
      // beep();
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
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (!digitalRead(DOWN_BUTTON))
    {
      // beep();
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
      // beep();
      EEPROM.put(30, dimTime);
      break;
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
  }
}
void enterPassword()
{
  int cursor = 5;
  int oldCursor;
  bool reset = false;
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

      lcd.setCursor(cursor, 1);

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

      lcd.setCursor(cursor, 1);

      reset = false;
      delay(200);
      while (!digitalRead(DOWN_BUTTON))
        ;
    }
    if (cursor == 15 && reset == false)
    {
      lcd.setCursor(0, 3);
    }
    if (cursor == 16)
    {
      lcd.setCursor(14, 3);
    }
    if (!digitalRead(MODE_BUTTON))
    {

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
          delay(1000);
          lcd.noBlink();
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
      while (!digitalRead(DOWN_BUTTON))
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

    if (oilPress == true)
    {
      lcd.setCursor(9, 0);
      lcd.print("RUN");
      lcd.setCursor(14, 1);
      lcd.print("NORMAL");
      if (engineRun == false)
      {
        engineRun = true;
        writeDataLoger("Engine Run,");
      }
    }
    if (oilPress == false)
    {
      lcd.setCursor(9, 0);
      lcd.print("STOP");
      lcd.setCursor(14, 1);
      lcd.print("LOW");
      if (engineRun == true)
      {
        engineRun = false;
        writeDataLoger("Engine Stop,");
      }
    }

    if (coolSys == true)
    {
      lcd.setCursor(14, 2);
      lcd.print("NORMAL");
      cooling_falut = false;
    }
    if (coolSys == false)
    {
      lcd.setCursor(14, 2);
      lcd.print("LOW");
      cooling_falut = false;
    }
    if (cooling_falut == true && oilPress == true)
    {
      lcd.setCursor(14, 2);
      lcd.print("FAULT");
    }

    showDisplay = false;
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
    if (count == 0)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  History ");
      lcd.setCursor(0, 1);
      lcd.print("Reading Data...     ");
      lcd.setCursor(0, 2);
      lcd.print("From SD Card        ");
      // delay(500);
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
            // Serial.println(token);
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
        // Serial.println(myFile.position());
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
      Serial.println("error opening SD Card");
      lcd.setCursor(0, 2);
      lcd.print("  SD Card Error !   ");
    }
  }
  if (digitalRead(DOWN_BUTTON) == LOW && displayDim == false)
  {
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
}
void page4()
{
  if (showDisplay == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Systems  Setting  ");
    lcd.setCursor(0, 1);
    lcd.print(" Over Temp:");
    lcd.print(overTemp);
    lcd.print(" C");
    lcd.print((char)223);
    lcd.setCursor(0, 2);
    lcd.print(" Cool'F Delay:");
    lcd.print(coolingfalut_delay / 1000);
    lcd.print(" S.");
    lcd.setCursor(0, 3);
    lcd.print(" Sleep Mode:");
    lcd.print(dimTime);
    lcd.print(" M.");

    showDisplay = false;
  }
  if (!digitalRead(DOWN_BUTTON))
  {
    menu++;
    settingMode = true;

    if (menu > 3)
    {
      menu = 0;
    }

    updateMenu();

    // showDisplay = true;
    last4 = millis();
    last5 = millis();
    delay(200);
    while (!digitalRead(DOWN_BUTTON))
      ;
  }
  if (!digitalRead(UP_BUTTON))
  {
    menu--;
    settingMode = true;
    if (menu < 0)
    {
      menu = 3;
    }
    updateMenu();

    // showDisplay = true;
    last4 = millis();
    last5 = millis();
    delay(200);
    while (!digitalRead(UP_BUTTON))
      ;
  }
  if (!digitalRead(MODE_BUTTON))
  {
    // settingMode = true;
    excuteAction();
    updateMenu();

    last4 = millis();
    last5 = millis();
    delay(200);
    while (!digitalRead(MODE_BUTTON))
      ;
  }
}

void waterTank_fault()
{
  digitalWrite(LED_YELLOW, HIGH);
  buzzerAlarm();
  if (showDisplay == true)
  {
    // Serial.println("Water Tank Low");
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

void readTemp()
{
  tempFault = true;
  float newTemp = 45.0;
  if (newTemp != temp)
  {
    temp = newTemp;
  }
  if (temp >= overTemp)
  {
    bool newOverTemp = true;
    if (newOverTemp != tempFault)
    {
      tempFault = newOverTemp;
      showDisplay = true;
      last6 = millis();
    }
    page = 0;
    engineOverTemp();
  }
}

void loop()
{
  currentMillis = millis();
  currentMillis1 = millis();

  readTemp();
  readWaterTank();
  readOilPressSw();
  readFlowSw();
  readTestSw();
  readDisableSw();

  if (engineRun == true)
  {
    if (engineStart == false)
    {
      previousMillis1 = currentMillis1;
      engineStart = true;
    }
    countSec = ((currentMillis1 - previousMillis1) / 1000);
    Serial.print("countSec = ");
    Serial.println(countSec);
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

  if (waterTank == true)
  {
    waterTank_fault();
    page = 0;
  }
  if (armedSw == false)
  {
    if (tempFault == false)
    {
      disable_fault();
    }

    // page = 0;
  }
  if (coolSys == false && oilPress == true)
  {
    if (cooling_fault_delay_start == false)
    {
      last2 = millis();
      cooling_fault_delay_start = true;
    }
    if (cooling_fault_delay_start == true && millis() - last2 < coolingfalut_delay)
    {
      Serial.print("Start Delay Cooling-Fault  ");
      Serial.println((millis() - last2) / 1000);
    }

    if (millis() - last2 > coolingfalut_delay)
    {
      if (cooling_fault_verify_start == false)
      {
        last3 = millis();
        cooling_fault_verify_start = true;
      }
      if (cooling_fault_verify_start == true && millis() - last3 < coolingfalut_verify)
      {
        Serial.print("Start verify Cooling-Fault  ");
        Serial.println((millis() - last3) / 1000);
      }
      if (millis() - last3 >= coolingfalut_verify)
      {
        cooling_fault();
        bool newCoolingFault = true;
        if (newCoolingFault != cooling_falut)
        {
          Serial.println("Cooling System Fault !");
          cooling_falut = newCoolingFault;
          showDisplay = true;
          page = 0;
        }
      }
    }
  }
  if (coolSys == false && oilPress == false && cooling_fault_delay_start == true)
  {
    last2 = 0;
    last3 = 0;
    cooling_falut = false;
    cooling_fault_verify_start = false;
    silence_alarm = false;

    showDisplay = true;
    page = 1;
    cooling_fault_delay_start = false;
    Serial.println("coolSys true");
  }

  if (digitalRead(SILENCE_ALARM) == LOW)
  {
    silence_alarm = true;
    noTone(BUZZER_PIN);
    writeDataLoger("Silence Alarm,");
    if (tempFault == false || armedSw == false)
    {
      showDisplay = true;
      page = 1;
      page1();
    }
    delay(200);
  }

  if (waterTank != true && armedSw != false && cooling_falut != true && tempFault != true)
  {
    noTone(BUZZER_PIN);
    page = 1;
  }
  if (digitalRead(MODE_BUTTON) == LOW && displayDim == false && settingMode == false)
  {
    read = false;
    buttonState++;
    count = 0;
    if (buttonState > 4)
    {
      buttonState = 1;
    }
    // Serial.println(buttonState);
    showDisplay = true;
    last4 = millis();
    last5 = millis();
    delay(200);
  }

  if (page == 1 && buttonState == 1 && displayDim == false)
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
  if (digitalRead(RESET_BUTTON) == LOW)
  {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("        RESET       ");
    noTone(BUZZER_PIN);
    writeDataLoger("Reset,");
    delay(5000);

    wdt_enable(WDTO_15MS);
    while (1)
    {
    }
    // page = 1;
    // buttonState = 1;
    // count = 0;
    // read = false;
    // last4 = millis();
    // last5 = millis();
    // showDisplay = true;
    // engineShutOff = false;
    // countDown = 60;
    // page1();
  }
}
