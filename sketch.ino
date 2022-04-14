    /*
  PN532-NFC-RFID-Module-Library
  modified on 18 Nov 2020
  by Amir Mohammad Shojaee @ Electropeak
  https://electropeak.com/learn/
 
  based on www.electroschematics.com Arduino Examples
*/

#include <CheapStepper.h>
#include <SoftwareSerial.h>
#include <PN532_SWHSU.h>
#include <PN532.h>
 
SoftwareSerial SWSerial( 10, 11 ); // RX, TX
PN532_SWHSU pn532swhsu( SWSerial );
PN532 nfc( pn532swhsu );
 
CheapStepper stepper (5,4,3,2);
boolean moveClockwise = true;

byte allowedKey[] = {0x36, 0x82, 0xA1, 0xAC};





void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello Maker!");
 
  nfc.begin();
  stepper.setRpm(2); 
 
  uint32_t versiondata = nfc.getFirmwareVersion();
 
  if (! versiondata) {
    Serial.print("Didn't Find PN53x Module");
    while (1); // Halt
  }
 
  // Got valid data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  // Configure board to read RFID tags
  nfc.SAMConfig();
  Serial.println("Waiting for an ISO14443A Card ...");
}

void loop(void) {

  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID 
  uint8_t uidLength;                       // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);


  moveClockwise = false;
  stepper.moveTo (moveClockwise, 0);
 
  if (success) {

    if (uid[0] == allowedKey[0] && uid[1] == allowedKey[1] && uid[2] == allowedKey[2] && uid[3] == allowedKey[3] ) {
      moveClockwise = true;
      stepper.moveTo (moveClockwise, 2048);
    }
 
    Serial.println("Found A Card!");
    Serial.print("UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("UID Value: ");
 
    for (uint8_t i=0; i < uidLength; i++)
    { 
      Serial.print(" 0x");
      Serial.print(uid[i], HEX);
    }
 
    Serial.println("");
    // 2 second halt
    delay(2000);
 
  }
  else
  {
    // PN532 probably timed out waiting for a card
    Serial.println("Timed out! Waiting for a card...");
  }
}
