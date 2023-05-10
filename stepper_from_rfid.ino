//www.elegoo.com
//2018.10.25

/*
Activates a stepper motor fro the correct rfid card

*/

//Stepper motor libraies and pins
#include <Stepper.h>

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
const int rolePerMinute = 15;         // Adjustable range of 28BYJ-48 stepper is 0~17 rpm

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 4, 6, 5, 7);

//RFID libraries and pins
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

//Create string instances
String firstName, lastName;

//Create bool for storing lock state
bool isLocked = true;

//Assign LED output pins
#define RED 2
#define GREEN 3

void setup() {
  myStepper.setSpeed(rolePerMinute);
  // initialize the serial port:
  Serial.begin(9600);
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read

  //init LED pins
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
}

void loop() { 
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  //Serial.print(F("Name: "));

  byte buffer1[18];

  block = 4;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT FIRST NAME
  // Amend to pull buffer data into a string
  firstName = String();
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      //Serial.write(buffer1[i]);
      firstName = firstName + (char) buffer1[i];
    }
  }
  //Serial.print(firstName);

  //---------------------------------------- GET LAST NAME

  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT LAST NAME
  // Amend to pull data into astring
  lastName = String();
  
  for (uint8_t i = 0; i < 16; i++) {
    lastName = lastName + (char) buffer2[i];
    //Serial.write(buffer2[i] );
  }

  //Get rid of any newline characters in the names
  firstName.replace("\n", "");
  lastName.replace("\n", "");
  Serial.print(firstName + " " + lastName);


  //----------------------------------------

  Serial.println(F("\n**End Reading**\n"));
    
  //  Now we have our first name and last name in blocks 4 and 1, presumably we can use logic to engage the stepper

  
  //if (firstName == "Keith" && lastName == "Macpherson"){ //doesn't work? second part of logic, oh well
  if (firstName == "Keith"){
    digitalWrite(GREEN, HIGH);
    if(isLocked){
      Serial.println("Lock opened");
      myStepper.step(stepsPerRevolution);
      delay(500);
      isLocked = false;
      } else {
        Serial.println("Lock closed");
        myStepper.step(-stepsPerRevolution);
        delay(500);
        isLocked = true;
      }
   } else {
    digitalWrite(RED, HIGH);
    }


  delay(1000); //change value if you want to read cards faster
  //once done, turn off the LEDs
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();



 
  /* 
  // step one revolution  in one direction:
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(500);

  // step one revolution in the other direction:
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(500);
  */
}
