
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>           
#include <LiquidCrystal_I2C.h>  



#define SS_PIN D4 // set pins
#define RST_PIN D3 // set pins



// create instances of our hardware components
MFRC522 rfid(SS_PIN, RST_PIN); 
LiquidCrystal_I2C lcd(0x27,16,2); 

// set string for ID 
String cardUID = "";
boolean cardRFID();

void setup() { 
  // Init Serial 
  SPI.begin();
  Serial.begin(9600);
  
  //Init rfid
  rfid.PCD_Init();

    // set up LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("RFID Card Reader");
  lcd.setCursor(4,1);
  lcd.print("Program");
  delay(5000);
  lcd.clear();

  }
  
void loop() {


    lcd.setCursor(0,0);
    lcd.print("Scan Card Below");

    if (cardRFID()){
      // run the getID fucntion to get the HEX value for card
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Card UID is: ");
      lcd.setCursor(4, 1);
      lcd.print(cardUID);
      delay(10000);
      lcd.clear();

    
    }
}
boolean cardRFID(){
  // check if theres a card
  if (!rfid.PICC_IsNewCardPresent()){
    return false;
  }
  // check if it has an approved HEX value
  if (!rfid.PICC_ReadCardSerial()){
    return false;
  }
  // concat string with HEX value to get a good formatted string
  cardUID = "";
  for (uint8_t i = 0; i < 4; i++){
    cardUID.concat(String(rfid.uid.uidByte[i], HEX));
  }
  cardUID.toUpperCase();
  rfid.PICC_HaltA();
  return true;

}
