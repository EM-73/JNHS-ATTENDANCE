#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <RtcDS1302.h>
#include <SD.h>

#define RST_PIN 9
#define SS_PIN 10

#define LCDI2C_POWER A3

#define BUZZER 7
#define LED_R 8
#define LED_G 2

File myFile;

ThreeWire myWire(A2, A1, A0);  // DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);

MFRC522 mfrc522(SS_PIN, RST_PIN);  //Create MFRC52 instance
MFRC522::MIFARE_Key key;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust the address and size according to your display

byte sectorfName = 1;
byte blockfName = 4;

byte sectorlName = 2;
byte blocklName = 8;

byte sectorStrand = 3;
byte blockStrand = 12;

byte sectorLrn = 4;
byte blockLrn = 16;

String asciiDatafName = " ";
String asciiDatalName = " ";
String asciiDataStrand = " ";
String asciiDataLrn = " ";

void setup() {
  pinMode(LCDI2C_POWER, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);

  digitalWrite(LCDI2C_POWER, HIGH);

  Serial.begin(9600);  //Intitialize serial  communications with pc
  while (!Serial);     //Do nothing if no serial port is opened
  SPI.begin();         //Initialize SPI bus

  

  if (!SD.begin(4)) {
    Serial.println("Initialization failed!");
  }
  Serial.println("Initialization done.");

  mfrc522.PCD_Init();  //Init MFRC522 card

  //RTC
  Rtc.Begin();
  RtcDateTime currentTime = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(currentTime);
  printDateTime(currentTime);
  Serial.println();

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  lcd.begin(16, 2);  // Initialize the LCD display
  lcd.init();
  lcd.backlight();  // Turn on the backlight
  lcd.clear();
  Serial.println("Student Data Reader");
  Serial.println("Swipe card");
}

void loop() {
  lcd.setCursor(2, 0);
  lcd.print("CARD  READER");
  Clock();
  readCard();
  delay(200);
}

void Clock(){
  RtcDateTime now = Rtc.GetDateTime();
  lcd.setCursor(4, 1);

  int hour = now.Hour();
  int minute = now.Minute();
  if (hour >= 12) {
    lcd.print(now.Hour() % 12);
    lcd.print(":");
    if (minute < 10) {
      lcd.print("0");
      lcd.print(now.Minute());
      lcd.print(" PM");
    }
    else{
      lcd.print(now.Minute());
      lcd.print(" PM");
    }
  } else {
    lcd.print(now.Hour() % 12);
    lcd.print(":");
    if (minute < 10) {
      lcd.print("0");
      lcd.print(now.Minute());
      lcd.print(" AM");
    }
    else{
      lcd.print(now.Minute());
      lcd.print(" AM");
    }
  }
  delay(1000);
}


void readCard(){
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()){
    Serial.println("Card Present");
    // Authenticate with the default key for the specified sector
    if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, sectorfName * 4 + 3, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) {

      // Read data from the specified block
      byte buffer[18];
      byte bufferSize = sizeof(buffer);
      MFRC522::StatusCode status = mfrc522.MIFARE_Read(blockfName, buffer, &bufferSize);
      if (status == MFRC522::STATUS_OK) {
        // Translate hexadecimal data to ASCII string
        asciiDatafName = "";

        for (byte i = 0; i < bufferSize; i++) {
          // Check if the byte is an alphabet character (A-Z or a-z)
          if ((buffer[i] >= 'A' && buffer[i] <= 'Z') || (buffer[i] >= 'a' && buffer[i] <= 'z' || buffer[i] == ' ')) {
            asciiDatafName += (char)buffer[i];
          } 
        }
        if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, sectorlName * 4 + 3, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) {
          // Read data from the specified block
          byte buffer[18];
          byte bufferSize = sizeof(buffer);
          MFRC522::StatusCode status = mfrc522.MIFARE_Read(blocklName, buffer, &bufferSize);
          if (status == MFRC522::STATUS_OK) {
            // Translate hexadecimal data to ASCII string
            asciiDatalName = "";

            for (byte i = 0; i < bufferSize; i++) {
              // Check if the byte is an alphabet character (A-Z or a-z)
              if ((buffer[i] >= 'A' && buffer[i] <= 'Z') || (buffer[i] >= 'a' && buffer[i] <= 'z' || buffer[i] == ' ')) {
                asciiDatalName += (char)buffer[i];
              } 
            }
            if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, sectorStrand * 4 + 3, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) {
              // Read data from the specified block
              byte buffer[18];
              byte bufferSize = sizeof(buffer);
              MFRC522::StatusCode status = mfrc522.MIFARE_Read(blockStrand, buffer, &bufferSize);
              if (status == MFRC522::STATUS_OK) {
                // Translate hexadecimal data to ASCII string
                asciiDataStrand = "";
                  
                for (byte i = 0; i < bufferSize; i++) {
                  // Check if the byte is an alphabet character (A-Z or a-z)
                  if ((buffer[i] >= '0' && buffer[i] <= '9' || buffer[i] >= 'A' && buffer[i] <= 'Z') || (buffer[i] >= 'a' && buffer[i] <= 'z' || buffer[i] == ' ' || buffer[i] == '-')) {
                    asciiDataStrand += (char)buffer[i];
                  } 
                }
                if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, sectorLrn * 4 + 3, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) {
                  // Read data from the specified block
                  byte buffer[18];
                  byte bufferSize = sizeof(buffer);
                  MFRC522::StatusCode status = mfrc522.MIFARE_Read(blockLrn, buffer, &bufferSize);
                  if (status == MFRC522::STATUS_OK) {
                    // Translate hexadecimal data to ASCII string
                    asciiDataLrn = "";
                  
                    for (byte i = 0; i < 12; i++) {
                      asciiDataLrn += (char)buffer[i];
                      asciiDataLrn.trim();
                    }
                    delay(200);
                    Serial.println("Data Available:");
                    Serial.println(asciiDatafName);
                    Serial.println(asciiDatalName);
                    Serial.println(asciiDataStrand);
                    Serial.println(asciiDataLrn);
                    delay(500);
                    SortStudent();
                    delay(100);
                    Indicator();
                  }
                }
              }
            }
          }
        }
      }
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    }
  }
}

void Indicator(){
  digitalWrite(BUZZER, HIGH);
  // Display translated data on LCD
  lcd.clear();
  lcd.print(asciiDatafName.substring(0,16));
  lcd.setCursor(0, 1);
  lcd.print(asciiDatalName.substring(0, 16));
  digitalWrite(BUZZER, LOW);
  delay(1000);
  lcd.clear();
  lcd.print(" Please proceed");
  lcd.setCursor(0, 1);
  lcd.print("  to your room");
  delay(650);
  lcd.clear();
  lcd.print("   Thank You!");
  delay(600);
  lcd.clear();
}

void SortStudent(){
  asciiDataStrand;
  asciiDataLrn;
  if (asciiDataStrand == "12-STEM") {
    myFile = SD.open("JNHS/12-STEM/12-STEM.txt", FILE_WRITE);
    if (myFile) {
      Serial.println("Writing to 12-STEM folder");
      RtcDateTime now = Rtc.GetDateTime();
      myFile.print(asciiDataLrn);
      delay(100);
      myFile.print(now.Year(), DEC);
      myFile.print('/');
      myFile.print(now.Month(), DEC);
      myFile.print('/');
      myFile.print(now.Day(), DEC);
      myFile.print(',');
      myFile.print(now.Hour(), DEC);
      myFile.print(':');
      myFile.println(now.Minute(), DEC);
      myFile.close();
      Serial.println("Wrote to 12-STEM folder successfully.");
    }
  } else if (asciiDataStrand == "12-ABM") {
    myFile = SD.open("JNHS/12-STEM/12-ABM.txt", FILE_WRITE);
    if (myFile) {
      Serial.println("Writing to 12-ABM folder...");
      myFile.print(asciiDataLrn);
      delay(100);
      RtcDateTime now = Rtc.GetDateTime();
      myFile.print(now.Year(), DEC);
      myFile.print('/');
      myFile.print(now.Month(), DEC);
      myFile.print('/');
      myFile.print(now.Day(), DEC);
      myFile.print(',');
      myFile.print(now.Hour(), DEC);
      myFile.print(':');
      myFile.println(now.Minute(), DEC);
      myFile.close();
      Serial.println("Wrote to 12-ABM folder successfully.");
    }
  } else if (asciiDataStrand == "12-GAS") {
    myFile = SD.open("JNHS/12-GAS/12-GAS.txt", FILE_WRITE);
    if (myFile) {
      Serial.println("Writing to 12-GAs folder...");
      myFile.print(asciiDataLrn);
      delay(100);
      RtcDateTime now = Rtc.GetDateTime();
      myFile.print(now.Year(), DEC);
      myFile.print('/');
      myFile.print(now.Month(), DEC);
      myFile.print('/');
      myFile.print(now.Day(), DEC);
      myFile.print(',');
      myFile.print(now.Hour(), DEC);
      myFile.print(':');
      myFile.println(now.Minute(), DEC);
      myFile.close();
      Serial.println("Wrote to 12-GAS folder successfully.");
    }
  } else if (asciiDataStrand == "12-HE") {
    myFile = SD.open("JNHS/12-HE/12-HE.txt", FILE_WRITE);
    if (myFile) {
      Serial.println("Writing to 12-HE folder...");
      myFile.print(asciiDataLrn);
      delay(100);
      RtcDateTime now = Rtc.GetDateTime();
      myFile.print(now.Year(), DEC);
      myFile.print('/');
      myFile.print(now.Month(), DEC);
      myFile.print('/');
      myFile.print(now.Day(), DEC);
      myFile.print(',');
      myFile.print(now.Hour(), DEC);
      myFile.print(':');
      myFile.println(now.Minute(), DEC);
      myFile.close();
      Serial.println("Wrote to 12-HE folder successfully.");
    }
  } else if (asciiDataStrand == "12-ICT") {
    myFile = SD.open("JNHS/12-ICT/12-ICT.txt", FILE_WRITE);
    if (myFile) {
      Serial.println("Writing to 12-ICT folder...");
      myFile.print(asciiDataLrn);
      delay(100);
      RtcDateTime now = Rtc.GetDateTime();
      myFile.print(now.Year(), DEC);
      myFile.print('/');
      myFile.print(now.Month(), DEC);
      myFile.print('/');
      myFile.print(now.Day(), DEC);
      myFile.print(',');
      myFile.print(now.Hour(), DEC);
      myFile.print(':');
      myFile.println(now.Minute(), DEC);
      myFile.close();
      Serial.println("Wrote to 12-HE folder successfully.");
    }
  } else {
    Serial.println("No folder, unsuccessful.");
  }
}


#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt) {
  char datestring[26];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
}

