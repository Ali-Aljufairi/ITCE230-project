#include <LiquidCrystal.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <DHT.h>

// Pin definitions
#define DHTPIN 7
#define SS_PIN 10
#define RST_PIN 9
#define IR_S1 2
#define IR_S2 4
#define SERVO_PIN 3
#define RED_LED 6
#define GREEN_LED 5
#define BUZZER_PIN 8

// Constants
#define DHTTYPE DHT11
#define TOTAL_PARKS 5
#define RFID_KEY "C4:D1:F1:33"

// Variables
DHT dht(DHTPIN, DHTTYPE);
MFRC522 rfid(SS_PIN, RST_PIN);
Servo myservo1;

int pos = 0;
int disabledht = 0;
int space = TOTAL_PARKS;
int flag1 = 0;
int flag2 = 0;

void setup()
{
  // Initialize components
  dht.begin();
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  myservo1.attach(SERVO_PIN);

  // Initialize pins
  pinMode(IR_S1, INPUT);
  pinMode(IR_S2, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // Initialize LCD
  LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print(" Private Car");
  lcd.setCursor(0, 1);
  lcd.print(" Parking System ");
  delay(2000);
  lcd.clear();

  space = TOTAL_PARKS;
}

void loop()
{
  float temperature = dht.readTemperature();
  Serial.println(temperature);

  if (temperature >= 29)
  {
    handleHighTemperature();
  }
  else if (disabledht == 77)
  {
    handleNormalTemperature();
  }

  detectIRSensor();
  checkRFIDCard();
  updateLCD();
}

void handleHighTemperature()
{
  digitalWrite(RED_LED, HIGH);
  tone(BUZZER_PIN, 1000);
  delay(1000);
  noTone(BUZZER_PIN);
  delay(1000);
  myservo1.write(pos = 0);
  disabledht = 77;
}

void handleNormalTemperature()
{
  digitalWrite(RED_LED, LOW);
  myservo1.write(pos = 90);
  disabledht = 0;
}

void detectIRSensor()
{
  if (digitalRead(IR_S1) == LOW && flag1 == 0)
  {
    handleIRSensor(1);
  }

  if (digitalRead(IR_S2) == LOW && flag2 == 0)
  {
    handleIRSensor(2);
  }
}

void handleIRSensor(int sensorNumber)
{
  if (space > 0)
  {
    flag1 = (sensorNumber == 1) ? 1 : flag1;
    flag2 = (sensorNumber == 2) ? 1 : flag2;

    if (flag1 == 1 && flag2 == 1)
    {
      delay(1000);
      myservo1.write(pos = 90);
      flag1 = 0;
      flag2 = 0;
    }
    else
    {
      myservo1.write(pos = 0);
      space = (sensorNumber == 1) ? space - 1 : space + 1;
    }
  }
  else
  {
    displayNoParksMessage();
  }

  updateLEDs();
}

void displayNoParksMessage()
{
  lcd.setCursor(0, 0);
  lcd.print(" Sorry no Parks");
  lcd.setCursor(0, 1);
  lcd.print("    Available    ");
  delay(1000);
  lcd.clear();
}

void updateLEDs()
{
  digitalWrite(GREEN_LED, (pos == 0) ? HIGH : LOW);
  digitalWrite(RED_LED, (pos == 0) ? LOW : HIGH);
}

void checkRFIDCard()
{
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
  {
    return;
  }

  String cardID = getRFIDCardID();

  if (cardID.equals(RFID_KEY))
  {
    handleAuthorizedAccess();
  }
  else
  {
    handleAccessDenied();
  }

  delay(500);
  updateLEDs();
}

String getRFIDCardID()
{
  String cardID = "";
  for (byte i = 0; i < 4; i++)
  {
    cardID += (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
              String(rfid.uid.uidByte[i], HEX) +
              (i != 3 ? ":" : "");
  }
  cardID.toUpperCase();
  return cardID;
}

void handleAuthorizedAccess()
{
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  myservo1.write(pos = 0);
  delay(2000);
  myservo1.write(pos = 90);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
}

void handleAccessDenied()
{
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  myservo1.write(pos = 90);
  tone(BUZZER_PIN, 2000);
  delay(1000);
  noTone(BUZZER_PIN);
  delay(1000);
  delay(2000);
}

void updateLCD()
{
  lcd.setCursor(0, 0);
  lcd.print("Total Parks:");
  lcd.print(TOTAL_PARKS);

  lcd.setCursor(0, 1);
  lcd.print("Left Parks:");
  lcd.print(space);
}
