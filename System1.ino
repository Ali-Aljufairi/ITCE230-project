#include <LiquidCrystal.h>// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
#include <Servo.h> //includes the servo library

#include "SPI.h"
#include "MFRC522.h"
#include "DHT.h"


#define DHTPIN 7
#define SS_PIN 10
#define RST_PIN 9
#define DHTTYPE DHT11

Servo myservo1;

int ir_s1 = 2;
int ir_s2 = 4;

int pos = 0 ;

int REDled = 6;
int greenled = 5;
int helloworld ;

int buzzer = 8 ;
int disabledht = 0 ;

int Total = 5;
int Space;

int flag1 = 0;
int flag2 = 0;

DHT dht(DHTPIN, DHTTYPE);
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void setup() {

  dht.begin();
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(ir_s1, INPUT);
  pinMode(ir_s2, INPUT);
  pinMode(greenled, OUTPUT);
  pinMode(REDled, OUTPUT) ;

  myservo1.attach(3);
  myservo1.write(pos = 90);

  lcd.begin(16, 2);
  lcd.setCursor (0, 0);
  lcd.print(" Private Car");
  lcd.setCursor (0, 1);
  lcd.print(" Parking System ");
  delay (2000);
  lcd.clear();


  Space = Total;
}

void loop() {

  float t = dht.readTemperature();
  Serial.println (t);
  Serial.print("");
  if (t >= 29)
  {

    digitalWrite(REDled, HIGH);
    tone(buzzer, 1000); // Send 1KHz sound signal...
    delay(1000);     // ...for 1 sec
    noTone(buzzer);  // Stop sound...
    delay(1000);
    myservo1.write(pos = 0);

    disabledht = 77 ;

    
  
  }

if (t < 29 && disabledht == 77 )
  {

    digitalWrite(REDled, LOW);
    myservo1.write(pos = 90);
    disabledht=0 ;
  

  }



  if (digitalRead (ir_s1) == LOW && flag1 == 0) {
    if (Space > 0) {
      flag1 = 1;


      if (flag2 == 0) {
        myservo1.write(pos = 0);
        Space = Space - 1;
      }
    } else {
      lcd.setCursor (0, 0);
      lcd.print(" Sorry no Parks");
      lcd.setCursor (0, 1);
      lcd.print("    Available    ");
      delay (1000);
      lcd.clear();
    }

    if (pos == 0) {
      digitalWrite(greenled, HIGH);
      digitalWrite(REDled, LOW);
    }
    else {
      digitalWrite(greenled, LOW);
      digitalWrite(REDled, HIGH);
    }

  }

  if (digitalRead (ir_s2) == LOW && flag2 == 0) {
    flag2 = 1;
    if (flag1 == 0) {
      myservo1.write(pos = 0);
      Space = Space + 1;
    }

    if (pos == 0) {
      digitalWrite(greenled, HIGH);
      digitalWrite(REDled, LOW);
    }
    else {
      digitalWrite(greenled, LOW);
      digitalWrite(REDled, HIGH);
    }







  }

  if (flag1 == 1 && flag2 == 1) {
    delay (1000);
    myservo1.write(pos = 90);
    flag1 = 0, flag2 = 0;

    if (pos == 0) {
      digitalWrite(greenled, HIGH);
      digitalWrite(REDled, LOW);
    }
    else {
      digitalWrite(greenled, LOW);
      digitalWrite(REDled, HIGH);
    }



  }

  lcd.setCursor (0, 0);
  lcd.print("Total Parks:");
  lcd.print(Total);

  lcd.setCursor (0, 1);
  lcd.print("Left Parks:");
  lcd.print(Space);


  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {

    return;



  } String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(rfid.uid.uidByte[i], HEX) +
      (i != 3 ? ":" : "");
  }
  strID.toUpperCase();
  //Serial.print("Tap card key: ");
  //Serial.println(strID);
  delay(500);


  if (pos == 0) {
    digitalWrite(greenled, HIGH);
    digitalWrite(REDled, LOW);
  }
  else {
    digitalWrite(greenled, LOW);
    digitalWrite(REDled, HIGH);
  }

  if (strID.indexOf("C4:D1:F1:33") >= 0) {  //put your own tap card key;   put your own rfid code of the tag in " C6:05:DA:2B "



    //(Authorised Access)

    digitalWrite(greenled, HIGH);
    digitalWrite(REDled, LOW);
    digitalWrite(buzzer, LOW);
    myservo1.write(pos = 0);
    delay(2000);
    myservo1.write(pos = 90);
    digitalWrite(REDled, HIGH);
    digitalWrite(greenled, LOW);


  }
  else
  {

    //Access denied
    digitalWrite(REDled, HIGH);
    digitalWrite(greenled, LOW);
    myservo1.write(pos = 90);
    tone(buzzer, 2000); // Send 1KHz sound signal...
    delay(1000);     // ...for 1 sec
    noTone(buzzer);  // Stop sound...
    delay(1000);     // ...for 1sec

    delay(2000);


  }
}