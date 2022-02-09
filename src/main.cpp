/*
   Safety Engine Control (PT&E) **พี่อมร
   By Arthit_N ... Tel.0959058972  E-Mail:tgbroadcast@gmail.com
   Kickoff 26/01/2022
   Ver.  0.9.0
   put on github 26/01/2022
*/
#include <Arduino.h>
//#include <virtuabotixRTC.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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

// virtuabotixRTC myRTC(7, 6, 5); //  Pin RTC (CLK,DAT,RST)

unsigned long previousMillis = 0;
unsigned long last1 = 0, last2 = 0, last3 = 0;
unsigned long waterTankFalut_verify = 15000;
unsigned long coolingfalut_verify = 5000;
unsigned long coolingfalut_delay = 20000;

bool goodDateTime;
bool testSw = false;
bool armedSw = false;
bool engineRun = false;
bool coolSys = false;
bool oilPress = false;
bool waterTank = false;
bool warning = false;
bool cooling_falut = false;

bool showDisplay = false;
bool silence_alarm = false;
int page = 1;
float temp = 35.0;

void readDateTime()
{
  // myRTC.updateTime();
  // Serial.println("------------------------------------------------");
  // if (myRTC.year < 2021 || myRTC.year > 2052)
  // {
  //   goodDateTime = false;
  //   Serial.println("Date Time Fault");
  // }
  // else
  // {
  //   goodDateTime = true;
  //   Serial.println("Date Time is OK");
  // }
  // Serial.print("Current Date / Time: ");
  // Serial.print(myRTC.dayofmonth);
  // Serial.print(" / ");
  // Serial.print(myRTC.month);
  // Serial.print(" / ");
  // Serial.print(myRTC.year);
  // Serial.print("  ");
  // Serial.print(myRTC.hours);
  // Serial.print(": ");
  // Serial.print(myRTC.minutes);
  // Serial.print(": ");
  // Serial.println(myRTC.seconds);
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
  lcd.print("  Date-Time is OK   ");
  lcd.setCursor(0, 3); //
  lcd.print("--------------------");
  // delay(1000);
  //  lcd.noBacklight();

  pinMode(TEST_PIN, INPUT_PULLUP);
  pinMode(ARMED_PIN, INPUT_PULLUP);
  pinMode(FLOW_SW, INPUT_PULLUP);
  pinMode(OIL_PRES_SW, INPUT_PULLUP);
  pinMode(WATER_TANK, INPUT_PULLUP);
  pinMode(SILENCE_ALARM, INPUT_PULLUP);

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
  // startTone();
  lcd.clear();
  showDisplay = true;
  // seconds, minutes, hours, day of the week, day of the month, month, year  //  Set วันเวลา RTC
  // myRTC.setDS1302Time(0, 20, 11, 3, 26, 1, 2022);                          //  Set วันเวลา RTC
  // readDateTime();
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
    }
    if (oilPress == false)
    {
      lcd.setCursor(9, 0);
      lcd.print("STOP");
      lcd.setCursor(14, 1);
      lcd.print("LOW");
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
  if (page == 1)
  {
    page1();
  }
  readTemp();
  readWaterTank();
  readOilPressSw();
  readFlowSw();
  readTestSw();
  readArmedSw();

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
    last3++;
    Serial.println(last3);
    if (last3 >= coolingfalut_delay)
    {
      last2++;
      Serial.println(last2);
      if (last2 >= coolingfalut_verify)
      {

        bool newCoolingFault = true;
        if (newCoolingFault != cooling_falut)
        {

          cooling_falut = newCoolingFault;
          showDisplay = true;
          page = 0;
          cooling_fault();
        }
      }
    }
  }
  if (coolSys == true)
  {
    last2 = 0;
    last3 = 0;
    cooling_falut = false;

    showDisplay = true;
    page = 1;
  }

  if (digitalRead(SILENCE_ALARM) == LOW)
  {
    silence_alarm = true;
    noTone(BUZZER_PIN);
    showDisplay = true;
    page = 1;
    last1 = 0;
    // last2 = 0;
    // last3 = 0;
  }

  if (waterTank != true && armedSw != false && cooling_falut != true)
  {
    noTone(BUZZER_PIN);
    page = 1;
  }
}