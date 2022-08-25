#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

#define lockRelayPin 12 // lock: LOW, unlock: HIGH
#define enginePin 13    // on: 0, off: 1
#define alcoholPin 14
#define bottonPin 15
#define redLampRelayPin 16
#define greenLampRelayPin 17

#define soundPin 18

#define returnDelay 1000

#define testTime 4000

#define alcoholLimit 0.8

#define count 32
static double list[count] = {};
static int last = 0;

static bool pass = false;
static bool message_FirstPass = true;
static bool message_FirstEengine = true;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);

  pinMode(lockRelayPin, OUTPUT);
  pinMode(enginePin, INPUT_PULLUP);
  pinMode(alcoholPin, INPUT);
  pinMode(bottonPin, INPUT_PULLUP);
  pinMode(redLampRelayPin, OUTPUT);
  pinMode(greenLampRelayPin, OUTPUT);
  pinMode(soundPin, OUTPUT);

  digitalWrite(lockRelayPin, LOW);
  digitalWrite(redLampRelayPin, LOW);
  digitalWrite(greenLampRelayPin, LOW);

  init_List();

  lcd.init();
  lcd.backlight();
}

void loop()
{
  // debug_State();
  //====================================================================================================//
  if (digitalRead(enginePin) == HIGH) // 엔진이 꺼져있는 상태
  {
    if (message_FirstEengine == true) // 화면 깜박이는 현상을 방지하기 위해 1회만 출력
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Engine Off");
      message_FirstEengine = false;
    }
    //====================================================================================================//
    if (pass == true) // 만약 테스트를 통과한 상태면 테스트 결과를 리셋하고 핸들을 잠금
    {
      pass = false;
      digitalWrite(lockRelayPin, LOW);
      digitalWrite(redLampRelayPin, LOW);
      digitalWrite(greenLampRelayPin, LOW);
      lcd.setCursor(0, 1);
      lcd.print("Test Reset");
      message_FirstPass = true; // message_FirstPass 초기화
    }
    //====================================================================================================//
    delay(returnDelay);
    return;
  }
  //====================================================================================================//
  if (message_FirstEengine == false) // message_FirstEengine 초기화 하는 겸 Engine On 메세지도 출력
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Engine On");
    message_FirstEengine = true;
    delay(returnDelay * 2);
  }
  //====================================================================================================//
  if (pass == true) // 엔진이 켜져있고 테스트를 이미 통과한 상태면 메세지 출력 후 대기
  {
    if (message_FirstPass == true) // 화면 깜박이는 현상을 방지하기 위해 1회만 출력
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Test Passed");
      message_FirstPass = false;
    }
    //====================================================================================================//
    delay(returnDelay);
    return;
  }
  //====================================================================================================//
  if (digitalRead(bottonPin) == LOW) // 버튼을 눌러 테스트 시작함
  {
    //====================================================================================================//
    list[last] = analogRead(alcoholPin) / (double)4095; // 0.00 ~ 1.00 //
    //====================================================================================================//
    // LCD 최초 1회 초기화 후 테스트 진행도 표시
    if (last == 0)
    {
      lcd.clear();
    }
    if (last < 16)
    {
      lcd.setCursor(last, 0);
      lcd.print(".");
    }
    else
    {
      lcd.setCursor(last - 16, 1);
      lcd.print(".");
    }
    last += 1;
    //====================================================================================================//
    if (last != count) // 테스트를 전부 완료하지 못함
    {
      delay(testTime / count);
      return;
    }
    //====================================================================================================//
    else // 테스트를 전부 완료함
    {
      double result = 0;
      for (int i = 0; i < count; i++)
      {
        result += list[i];
      }

      result = result / count;

      lcd.clear();
      lcd.setCursor(0, 0);
      //====================================================================================================//
      if (result < alcoholLimit) // 테스트 통과함
      {
        pass = true;
        digitalWrite(lockRelayPin, HIGH);
        digitalWrite(redLampRelayPin, LOW);
        digitalWrite(greenLampRelayPin, HIGH);

        lcd.print("Test Pass");
      }
      //====================================================================================================//
      else // 테스트에 통과하지 못함
      {
        digitalWrite(redLampRelayPin, HIGH);
        digitalWrite(greenLampRelayPin, LOW);
        tone(soundPin, 800, 2000);
        lcd.print("Test Failure");
      }
      //====================================================================================================//
      // 테스트 결과 출력
      lcd.setCursor(0, 1);
      lcd.print("Result:");
      lcd.setCursor(7, 1);
      lcd.print(result * 100);
      lcd.setCursor(12, 1);
      lcd.print("%");
      init_List();
      delay(returnDelay * 2);
      //====================================================================================================//
    }
  }
  //====================================================================================================//
  else if (last != 0) // 테스트 도중 중지함
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Stop Testing");
    init_List();
  }
  //====================================================================================================//
  else // 테스트 대기중
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wating For Test");
  }
  //====================================================================================================//
  delay(returnDelay);
  return;
}

void init_List()
{
  for (int i = 0; i < count; i++)
  {
    list[i] = -1 * count;
  }
  last = 0;
}

void debug_State()
{
  Serial.print("lockRelayPin : ");
  Serial.println(digitalRead(lockRelayPin));
  Serial.print("enginePin : ");
  Serial.println(digitalRead(enginePin));
  Serial.print("alcoholPin : ");
  Serial.println(analogRead(alcoholPin) / (double)4095);
  Serial.print("bottonPin : ");
  Serial.println(digitalRead(bottonPin));
  Serial.print("redLampRelayPin : ");
  Serial.println(digitalRead(redLampRelayPin));
  Serial.print("greenLampRelayPin : ");
  Serial.println(digitalRead(greenLampRelayPin));
  Serial.print("soundPin : ");
  Serial.println(digitalRead(soundPin));
  Serial.println("==================================================");
}