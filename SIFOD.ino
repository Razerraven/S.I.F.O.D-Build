/*
 
 * -----------------------------------
 *             MFRC522      Arduino
 *             Reader/PCD   NodeMCU
 * Signal      Pin          Pin          
 * ------------------------------------
 * RST/Reset   RST      D3         
 * SPI SS      SDA(SS)  D4        
 * SPI MOSI    MOSI     D7
 * SPI MISO    MISO     D6   
 * SPI SCK     SCK      D5
 * 
 *             LCD  
 * Signal      Pin          Pin          
 * ------------------------------------
 * SDA        D2
 * SCL        D1  

 * General
 * GND - GND
 * VCL/VIN - 5V
 * 3v - 3.3V/3V
 */

#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <Wire.h>           
#include <LiquidCrystal_I2C.h>  
#include <ESP_Google_Sheet_Client.h>


#define SS_PIN D4 // set pins
#define RST_PIN D3 // set pins

#define PROJECT_ID "salam-pantry-rfid-project"
#define CLIENT_EMAIL "salam-pantry-sa@salam-pantry-rfid-project.iam.gserviceaccount.com"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDcnB2MRgQ/+/oW\nMkcukcCxYf9KkDx9JoBVjnY7o645N6d12wGmF+1QcOkJ+yhgPExC4sDWB9UoaJD8\n3u69TAVQZ4Qiat1rGkphAjw4P0kW3GpN6/sGzH60KEEoLVq4TXWEtJuWJpbN4f3d\ndTdsKbck+MAfpVbDf0b2a6gwcHKmP1KTSne4IUP2lXlJemvUmbh4MPup/Zeri+B3\nCEaXUmVuYNw1skrmWkPz49ZtYFdjIJjQC9+T5sRPBhUJSP0ZpkT5XcYtj4+P+xB1\nWB8SnDuLmmEASvxa5dm9lNPK+Z48HUuiiCjVE61pXgMttLkW0Nb2ouTx3QhpX1jr\n8OyukB4vAgMBAAECggEAAe/VEFgQ6CIUeL89fhmtFHKLbXNdu6WNrrZgv2t5M6r8\nzU5abhZ/R0UFGCrWxdI7HYagxNjBDU6ua6l1R+Tx/5Y3qbyiZH5M+rIsiKVMauNJ\nv/5Et5y18wIWaJf7OgICcOrbA6mNAt2NRsRwqFUu9KfRX8Lt4bKD7ZjOUBElnE1h\nqRLxEArfYokd36DfTZUmTu2/QESOF9tEYn/l5pz/plduJDPN97nm96+MZnueeR1e\ndMsFBLtwxi6spo6MA5KJRp3iKGGdcrnz9G87k/p/uek7AhEh+JPoWoGKlMlour5l\nDf97jvAk5ObFI2OqK09rho/9xeB3gFde2rjyfsxFIQKBgQD/cYNCHoK6Lor/r0YL\nqwzmtyy/ct8N9vGxgPAO2448o4UEapXiVaNYAfa+5gz4mU8c/DAgSMvSCuO9Ut+I\njcv1/8OSJFK+dqRGYkMWRb/58B1CEdfF2tBjI8JQdA5STjmNRrMk7eQW9H9rfLWT\nODgRNumlBwYHJf2JVnNEPUWnzwKBgQDdFywh9xTkuu2YdGGu9awd+IdQxEWYiCwa\nGffZIVg9jWkxDxcZMtQb05lUcViE//xcY3YFx6mKd1y9rg3BwKUuhO+VqDcEX/E9\njjqZNtJ38OFvLEH3fVMJdqDxqjp8RB96m7bNCvo4LaXzu3l9CG6aptOgKFd8BB3E\nwoDLFvVboQKBgBTKUlqRTSaGHz4ML/FHLLeidPvoB4uFa8p8qg0rhXod/PeNDUHs\nomzJTCHha3DYWX4McIZQJnaj+EEye4sLr2wIMwyz9CAYW+p2mOs5JhTbnCYCecNa\nudc309rkdz3vq/N5q5731bagk7ZV4hlnW/jatU1/MthmGSqSZLNLIV41AoGBAMj2\nzjvVbt12eI/SghRZ5WicxssIjUZXyNa08hAg+ieBCbP5RoK2LA1bKo2SNec3FHoa\nQmaAlcByPQ+BTannZX5CgdCkRdYpK4xLY0JvjaJOh2vZa+D9vuPkO+lPVbgs5/w3\n3FLG/qjf/W/t4kFfz8GtkMGMyfDFgRWfwxhXtxDhAoGBANer+CDgHWUoM9EV2DQ3\nOca2hClsAUsI4k0wNeecsLBrxVvzoT2Rfx/SNB2WvrCl2h9RZ66xUEkZzxQ7EgXm\n0Gs1o9mLMdCWqdGBNrlXXn8nEyedAWEc/8Z48RdbDYQoo+eQpXUxwpDXeHwIj/vM\nBWAHmeZSDmPAw/jJnJ6GcFtt\n-----END PRIVATE KEY-----\n";
 
// set wifi info
const char* ssid = "MakerNexus";
const char* password = "Maker234";

// create instances of our hardware components
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
LiquidCrystal_I2C lcd(0x27,16,2); 
MFRC522::MIFARE_Key key; 


// set string for ID 
String tagID = "";
String approvedID = "E317851E";
boolean getID();

bool taskComplete = false;

void setup() { 
  // Init Serial 
  SPI.begin(); // Init SPI bus
  Serial.begin(9600);
  pinMode(D8, INPUT);
  //Init rfid
  rfid.PCD_Init();

  //WiFI
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(". ");
    delay(500);
  }
  Serial.println();
  Serial.println("WiFi Connected");

  //Begin the access token generation for Google API authentication
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

  // set up LCD
  lcd.init();
  lcd.backlight();
}
 
void loop() {
  int buttonVal = digitalRead(D8);

  if (buttonVal == 1) {

    lcd.setCursor(2,0);
    lcd.print("Place Card");

    if (getID()){
      // run the getID fucntion to get the HEX value for card
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Card no: ");
      lcd.setCursor(2, 1);
      lcd.print(tagID);
      delay(2000);
      lcd.clear();

      bool inSheet = checkSheet();
      
      // check if it is in the sheets in order to see if it is approved
      if(!inSheet){
        lcd.setCursor(1,0);
        lcd.print("Access Denied");
        delay(2000);
        lcd.clear();
      }


    }
  }
}
// check if it is in the sheet
boolean checkSheet(){
  bool ready = GSheet.ready();

  if (ready && !taskComplete){
    //For basic FirebaseJson usage example, see examples/FirebaseJson/Create_Edit_Parse/Create_Edit_Parse.ino

    //If you assign the spreadsheet id from your own spreadsheet,
    //you need to set share access to the Service Account's CLIENT_EMAIL

    FirebaseJson response;
      
    Serial.println("Get spreadsheet values from range...");
    Serial.println("---------------------------------------------------------------");

    bool success = GSheet.values.get(&response /* returned response */, "19gHJ14CXwInlk0dg2wJrR4vardE-ZPwwI7-Ef6m0T3w" /* spreadsheet Id to read */, "Sheet1!B2:B" /* range to read */);


    FirebaseJsonData result;
    response.get(result, "values");
    Serial.println();

    FirebaseJsonArray arr;

    //Get array data
    result.get<FirebaseJsonArray>(arr);

    //Call get with FirebaseJsonData to parse the array at defined index i

    boolean isThere = false;
    // looks throughout the spreadsheet
    for (size_t i = 0; i < arr.size(); i++)
    {
      //result now used as temporary object to get the parse results
      arr.get(result, i);
      String str = result.to<String>();
      str.remove(0,2);
      str.remove(str.length()-2);
      str.trim();
      // compares string to the tag ID scanned
      if (str == tagID){
        lcd.setCursor(1,0);
        lcd.print("Access Granted");
        delay(2000);
        lcd.clear();
        isThere =  true;
        break;

      }
      

    }
    taskComplete = true;  

    return isThere;
  }
}
boolean getID(){
  // check if theres a card
  if (!rfid.PICC_IsNewCardPresent()){
    return false;
  }
  // check if it has an approved HEX value
  if (!rfid.PICC_ReadCardSerial()){
    return false;
  }
  // concat string with HEX value to get a good formatted string
  tagID = "";
  for (uint8_t i = 0; i < 4; i++){
    tagID.concat(String(rfid.uid.uidByte[i], HEX));
  }
  tagID.toUpperCase();
  rfid.PICC_HaltA();
  return true;

}