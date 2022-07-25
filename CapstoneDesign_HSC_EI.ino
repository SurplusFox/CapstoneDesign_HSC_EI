#define relayPin 2
#define trigPin 4
#define echoPin 6
#define soundPin 8
#define distanceLedPin 10
#define enginePin 13
#define alcoholPin A5

static bool pass = false;

static int list[10] = {};
static int last = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(distanceLedPin, OUTPUT);
  pinMode(enginePin, INPUT);

  digitalWrite(relayPin, HIGH);

  resetListAndLast();
}

void loop()
{
  if (digitalRead(enginePin) == HIGH)
  {
    if (pass == true)
    {
      Serial.println("엔진을 끔 테스트 리셋");
      pass = false;
    }
    Serial.println("엔진이 꺼져있음");
    digitalWrite(relayPin, HIGH);
    digitalWrite(distanceLedPin, LOW);
    delay(1000);
    return;
  }
  if (pass)
  {
    Serial.println("테스트 통과함");
    delay(1000);
    return;
  }

  unsigned long duration, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH); // ms
  distance = duration / 58.2;        // cm

  if (distance <= 10)
  {
    digitalWrite(distanceLedPin, HIGH);
    list[last] = analogRead(alcoholPin);
    last += 1;
    Serial.print(". ");
    if (last == 10)
    {
      Serial.println("");
      last = 0;

      int result = 0;
      for (int i = 0; i < 10; i++)
      {
        Serial.print(i + 1);
        Serial.print("회차 값 : ");
        Serial.println(list[i]);
        result += list[i];
      }
      result = result / 10;
      if (result > 0)
      {
        if (result < 400)
        {
          Serial.print("테스트 통과 10회 평균값 : ");
          Serial.println(result);
          digitalWrite(relayPin, LOW);
          digitalWrite(distanceLedPin, LOW);
          pass = true;
        }
        else
        {
          Serial.print("??? 동작그만! 10회 평균값 : ");
          tone(soundPin, 830, 5000);
          delay(5000);
          noTone(soundPin);
          return;
        }
      }
    }
    delay(400);
    return;
  }
  else
  {
    Serial.println("거리가 멀어 테스트 중지");

    digitalWrite(distanceLedPin, LOW);
    resetListAndLast();
  }
  delay(1000);
}

void resetListAndLast()
{
  for (int i = 0; i < 10; i++)
  {
    list[i] = -32768;
  }
  last = 0;
}