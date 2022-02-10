/*
   Safety Engine Control (PT&E) **พี่อมร
   By Arthit_N ... Tel.0959058972  E-Mail:tgbroadcast@gmail.com
   Kickoff 26/01/2022
   Ver.  0.9.0
   put on github 26/01/2022
*/

//**Mega : MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 4(CS pin can be changed) and pin #52(SS)must be an output

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define SILENCE_ALARM 9
#define TEST_PIN 10
#define ARMED_PIN 11

#define LED_YELLOW 15
#define LED_RED 16
#define BUZZER_PIN 17
#define FLOW_SW 18
#define OIL_PRES_SW 19
#define WATER_TANK 12

#define UP_BUTTON 38
#define DOWN_BUTTON 39
#define MODE_BUTTON 40

unsigned long previousMillis = 0;
unsigned long last1 = 0, last2 = 0, last3 = 0, last4 = 0, last5,last6;
unsigned long waterTankFalut_verify = 15000;
unsigned long coolingfalut_verify = 5000;
unsigned long coolingfalut_delay = 3000;
unsigned long dimTime = 30000;
unsigned long engineRunTime;
unsigned long eneineRunAll;

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

bool showDisplay = false;
bool displayDim = false;
bool silence_alarm = false;
int page = 1;
float temp = 35.0;

int buttonState = 1;

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

File myFile; // สร้างออฟเจก File สำหรับจัดการข้อมูล
const int chipSelect = 53;

void writeDataLoger(String event)
{
  myFile = SD.open("data.txt", FILE_WRITE); // เปิดไฟล์ที่ชื่อ test.txt เพื่อเขียนข้อมูล โหมด FILE_WRITE

  // ถ้าเปิดไฟล์สำเร็จ ให้เขียนข้อมูลเพิ่มลงไป
  if (myFile)
  {
    DateTime now = rtc.now();

    Serial.print("Writing to SD Card ");
    myFile.print(event); // สั่งให้เขียนข้อมูล
    myFile.print(now.day());
    myFile.print("-");
    myFile.print(now.month());
    myFile.print("-");
    myFile.print(now.year());
    myFile.print(", ");
    myFile.print(now.hour());
    myFile.print(":");
    myFile.print(now.minute());
    myFile.print(":");
    myFile.println(now.second());

    myFile.close(); // ปิดไฟล์
    Serial.println("done.");
  }
  else
  {
    // ถ้าเปิดไฟลืไม่สำเร็จ ให้แสดง error
    Serial.println("error opening File");
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

  myFile = SD.open("data.txt", FILE_WRITE); // เปิดไฟล์ที่ชื่อ test.txt เพื่อเขียนข้อมูล โหมด FILE_WRITE

  // ถ้าเปิดไฟล์สำเร็จ ให้เขียนข้อมูลเพิ่มลงไป
  if (myFile)
  {
    DateTime now = rtc.now();

    Serial.print("Writing to SD Card ");
    myFile.print("Power ON, "); // สั่งให้เขียนข้อมูล
    myFile.print(now.day());
    myFile.print("-");
    myFile.print(now.month());
    myFile.print("-");
    myFile.print(now.year());
    myFile.print(", ");
    myFile.print(now.hour());
    myFile.print(":");
    myFile.print(now.minute());
    myFile.print(":");
    myFile.println(now.second());

    myFile.close(); // ปิดไฟล์
    Serial.println("done.");
  }
  else
  {
    // ถ้าเปิดไฟลืไม่สำเร็จ ให้แสดง error
    Serial.println("error opening File");
  }

  // เปิดไฟล์เพื่ออ่าน
  myFile = SD.open("data.txt"); // สั่งให้เปิดไฟล์ชื่อ test.txt เพื่ออ่านข้อมูล
  if (myFile)
  {
    Serial.println("---------------------------------");
    Serial.println("Read All DataLoger From SD Card..");
    // อ่านข้อมูลทั้งหมดออกมา
    while (myFile.available())
    {
      Serial.write(myFile.read());
    }
    myFile.close(); // เมื่ออ่านเสร็จ ปิดไฟล์
    Serial.println("---------------------------------");
  }
  else
  {
    // ถ้าอ่านไม่สำเร็จ ให้แสดง error
    Serial.println("error opening SD Card");
  }
}

void readDateTime()
{
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
  delay(100);
  noTone(BUZZER_PIN);
  delay(100);
}

void displayOff()
{
  lcd.noBacklight();
}
void displayOn()
{
  lcd.backlight();
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

  pinMode(chipSelect, OUTPUT);
  // pinMode(52, OUTPUT);

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
  if (silence_alarm == false)
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
    writeDataLoger("Cooling System Fault, ");
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
    writeDataLoger("SHUTOFF DISABLE, ");
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
void readArmedSw()
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
    lcd.print("ENGINE TEMP :");

    lcd.setCursor(13, 3);
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
        writeDataLoger("Engine Run, ");
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
        writeDataLoger("Engine Stop, ");
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
  if (displayDim == false && millis() - last4 >= dimTime)
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
    lcd.print(now.month());
    lcd.print("-");
    lcd.print(now.year());

    lcd.setCursor(0, 2);
    lcd.print("TIME :");
    lcd.setCursor(0, 3);
    lcd.print("ENGINE RUN :");

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
    writeDataLoger("Water tank low, ");
  }
}

void readTemp()
{
  float newTemp = 36.0;
  if (newTemp != temp)
  {
    temp = newTemp;
    showDisplay = true;
  }
}

void loop()
{

  readTemp();
  readWaterTank();
  readOilPressSw();
  readFlowSw();
  readTestSw();
  readArmedSw();

  if (engineRun == true)
  {
    if (engineStart == false)
    {
      last5 = millis();
      last6 = millis();
      engineStart = true;
    }
    engineRunTime = engineRunTime + (millis() - last5);
    engineRunTime = engineRunTime / 1000;
    eneineRunAll = eneineRunAll + engineRunTime;
    Serial.println(engineRunTime);
    Serial.println(eneineRunAll);
  }
  if (engineRun==false)
  {
    engineStart = false;
  }
  

  if (waterTank == true)
  {
    waterTank_fault();
    page = 0;
  }
  if (armedSw == false)
  {
    disable_fault();
    page = 0;
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
    showDisplay = true;
    page = 1;
    writeDataLoger("silence Alarm, ");
    delay(200);
  }

  if (waterTank != true && armedSw != false && cooling_falut != true)
  {
    noTone(BUZZER_PIN);
    page = 1;
  }
  if (digitalRead(UP_BUTTON) == LOW && displayDim == false)
  {
    buttonState++;
    if (buttonState > 2)
    {
      buttonState = 1;
    }
    Serial.println(buttonState);
    showDisplay = true;
    delay(200);
  }
  if (digitalRead(DOWN_BUTTON) == LOW && displayDim == false)
  {
    buttonState--;
    if (buttonState < 1)
    {
      buttonState = 2;
    }
    showDisplay = true;
    Serial.println(buttonState);
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
}