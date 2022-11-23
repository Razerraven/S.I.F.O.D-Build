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
#include <NTPClient.h>
#include <WiFiUdp.h>

#define SS_PIN D4 // set pins
#define RST_PIN D3 // set pins

#define PROJECT_ID "salam-pantry-rfid-project"
#define CLIENT_EMAIL "salam-pantry-sa@salam-pantry-rfid-project.iam.gserviceaccount.com"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDcnB2MRgQ/+/oW\nMkcukcCxYf9KkDx9JoBVjnY7o645N6d12wGmF+1QcOkJ+yhgPExC4sDWB9UoaJD8\n3u69TAVQZ4Qiat1rGkphAjw4P0kW3GpN6/sGzH60KEEoLVq4TXWEtJuWJpbN4f3d\ndTdsKbck+MAfpVbDf0b2a6gwcHKmP1KTSne4IUP2lXlJemvUmbh4MPup/Zeri+B3\nCEaXUmVuYNw1skrmWkPz49ZtYFdjIJjQC9+T5sRPBhUJSP0ZpkT5XcYtj4+P+xB1\nWB8SnDuLmmEASvxa5dm9lNPK+Z48HUuiiCjVE61pXgMttLkW0Nb2ouTx3QhpX1jr\n8OyukB4vAgMBAAECggEAAe/VEFgQ6CIUeL89fhmtFHKLbXNdu6WNrrZgv2t5M6r8\nzU5abhZ/R0UFGCrWxdI7HYagxNjBDU6ua6l1R+Tx/5Y3qbyiZH5M+rIsiKVMauNJ\nv/5Et5y18wIWaJf7OgICcOrbA6mNAt2NRsRwqFUu9KfRX8Lt4bKD7ZjOUBElnE1h\nqRLxEArfYokd36DfTZUmTu2/QESOF9tEYn/l5pz/plduJDPN97nm96+MZnueeR1e\ndMsFBLtwxi6spo6MA5KJRp3iKGGdcrnz9G87k/p/uek7AhEh+JPoWoGKlMlour5l\nDf97jvAk5ObFI2OqK09rho/9xeB3gFde2rjyfsxFIQKBgQD/cYNCHoK6Lor/r0YL\nqwzmtyy/ct8N9vGxgPAO2448o4UEapXiVaNYAfa+5gz4mU8c/DAgSMvSCuO9Ut+I\njcv1/8OSJFK+dqRGYkMWRb/58B1CEdfF2tBjI8JQdA5STjmNRrMk7eQW9H9rfLWT\nODgRNumlBwYHJf2JVnNEPUWnzwKBgQDdFywh9xTkuu2YdGGu9awd+IdQxEWYiCwa\nGffZIVg9jWkxDxcZMtQb05lUcViE//xcY3YFx6mKd1y9rg3BwKUuhO+VqDcEX/E9\njjqZNtJ38OFvLEH3fVMJdqDxqjp8RB96m7bNCvo4LaXzu3l9CG6aptOgKFd8BB3E\nwoDLFvVboQKBgBTKUlqRTSaGHz4ML/FHLLeidPvoB4uFa8p8qg0rhXod/PeNDUHs\nomzJTCHha3DYWX4McIZQJnaj+EEye4sLr2wIMwyz9CAYW+p2mOs5JhTbnCYCecNa\nudc309rkdz3vq/N5q5731bagk7ZV4hlnW/jatU1/MthmGSqSZLNLIV41AoGBAMj2\nzjvVbt12eI/SghRZ5WicxssIjUZXyNa08hAg+ieBCbP5RoK2LA1bKo2SNec3FHoa\nQmaAlcByPQ+BTannZX5CgdCkRdYpK4xLY0JvjaJOh2vZa+D9vuPkO+lPVbgs5/w3\n3FLG/qjf/W/t4kFfz8GtkMGMyfDFgRWfwxhXtxDhAoGBANer+CDgHWUoM9EV2DQ3\nOca2hClsAUsI4k0wNeecsLBrxVvzoT2Rfx/SNB2WvrCl2h9RZ66xUEkZzxQ7EgXm\n0Gs1o9mLMdCWqdGBNrlXXn8nEyedAWEc/8Z48RdbDYQoo+eQpXUxwpDXeHwIj/vM\nBWAHmeZSDmPAw/jJnJ6GcFtt\n-----END PRIVATE KEY-----\n";
 
// set wifi info
const char* ssid = "SSID";
const char* password = "PASS";

// set date and time info
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

String sheetName = "Sheet1";

// create instances of our hardware components
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
LiquidCrystal_I2C lcd(0x27,16,2); 
MFRC522::MIFARE_Key key; 
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() { 
  // Init Serial 
  SPI.begin(); // Init SPI bus
  Serial.begin(9600);
  pinMode(D8, INPUT);
  //Init rfid
  rfid.PCD_Init();

  // set up LCD
  lcd.init();
  lcd.backlight();

  //WiFI
  lcd.setCursor(2,0);
  lcd.print("Connecting to ");
  lcd.setCursor(2,1);
  lcd.print(ssid);
  delay(500);
  lcd.clear();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
  }
  lcd.setCursor(2,0);
  lcd.print("WiFi Connected");
  delay(500);
  lcd.clear();

  //Begin time
  timeClient.begin();
  timeClient.setTimeOffset(0);

  //Begin the access token generation for Google API authentication
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);
}
 
void loop() {
  bool ready = GSheet.ready();

  lcd.setCursor(2,0);
  lcd.print("Place Card");
  String tagID = "";
  if (getID(tagID)){
    // run the getID fucntion to get the HEX value for card

    int cellNum = 1;
    bool inSheet = checkSheet(cellNum, tagID);
    
    lcd.clear();
    lcd.print("Card no: ");
    lcd.setCursor(2, 1);
    lcd.print(tagID);
    delay(2000);
    lcd.clear();

    // Update name
    if (ready && inSheet){
      String name = getName(cellNum);
      lcd.setCursor(2,0);
      lcd.print(name);
      delay(2000);
      lcd.clear();

      lcd.setCursor(2,0);
      lcd.print("Access Granted");
      setTime(cellNum);
      delay(2000);
      lcd.clear();
    }

    // check if it is not in sheets 
    if(!inSheet){
      lcd.setCursor(1,0);
      lcd.print("Access Denied");
      delay(2000);
      lcd.clear();
    }

  }
}

void setTime(int cellNum){
  timeClient.begin();

  //Get a time structure
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = (ptm->tm_mday)-1;
  int currentMonth = ptm->tm_mon+1;
  String currentMonthName = months[currentMonth-1];
  int currentYear = ptm->tm_year+1900;
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);

  FirebaseJson response;
  FirebaseJson valueRange;

  // Update sheet with attendance check-in date 
  String temp = sheetName + "!G" + String(cellNum) + ":G";
  valueRange.add("range", temp);
  valueRange.add("majorDimension", "ROWS");
  valueRange.set("values/[0]/[0]", currentDate  ); //row 1/ column 1

  bool success = GSheet.values.update(&response, "19gHJ14CXwInlk0dg2wJrR4vardE-ZPwwI7-Ef6m0T3w", temp, &valueRange);
  response.toString(Serial, true);
}

// check if it is in the sheet
boolean checkSheet(int &cellNum, String tagID){
  bool ready = GSheet.ready();

  if (ready){

    FirebaseJson response;
    String temp = sheetName + "!F2:F";
    bool success = GSheet.values.get(&response, "19gHJ14CXwInlk0dg2wJrR4vardE-ZPwwI7-Ef6m0T3w" , temp);

    FirebaseJsonData result;
    response.get(result, "values");

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
      cellNum++;
      // compares string to the tag ID scanned
      if (str == tagID){
        isThere =  true;
        break;
      }
    }
    return isThere;
  }
}

String getName(int cellNum){
 
 bool ready = GSheet.ready();
  
  if (ready)
  {
    FirebaseJson response;
    String temp = sheetName + "!A2:A";
    bool success = GSheet.values.get(&response, "19gHJ14CXwInlk0dg2wJrR4vardE-ZPwwI7-Ef6m0T3w", temp);

    FirebaseJsonData result;
    response.get(result, "values");

    FirebaseJsonArray arr;

    //Get array data
    result.get<FirebaseJsonArray>(arr);
    //Call get with FirebaseJsonData to parse the array at defined index i

    boolean isThere = false;
    // looks throughout the spreadsheet
    String str;
    
    //result now used as temporary object to get the parse results
    arr.get(result, cellNum-2);
    str = result.to<String>();
    str.remove(0,2);
    str.remove(str.length()-2);
    str.trim();      
    
    return str;
  }

}
boolean getID(String &tagID){
  // check if theres a card
  if (!rfid.PICC_IsNewCardPresent()){
    return false;
  }
  // check if it has an approved HEX value
  if (!rfid.PICC_ReadCardSerial()){
    return false;
  }
  // concat string with HEX value to get a good formatted string

  for (uint8_t i = 0; i < 4; i++){
    tagID.concat(String(rfid.uid.uidByte[i], HEX));
  }
  tagID.toUpperCase();
  rfid.PICC_HaltA();
  return true;
}