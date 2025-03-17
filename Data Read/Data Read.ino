#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN D8  
#define RST_PIN D3  

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;











void setup() {
    Serial.begin(115200);
    SPI.begin();
    rfid.PCD_Init();

    // Default key (FFFFFFFFFFFF)
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println("Scan your RFID card to WRITE data...");
}

void loop() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return;
    }

    Serial.println("Card detected!");

    byte block = 4;  // Block to write to
    byte dataToWrite[16] = "tamilarasan6242";  // 16-byte message

    if (writeToBlock(block, dataToWrite)) {
        Serial.println("Write successful! Reading back...");
        readFromBlock(block);
    } else {
        Serial.println("Write failed!");
    }

    // Halt the card
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

bool writeToBlock(byte block, byte data[]) {
    // Authenticate with Key A
    MFRC522::StatusCode status = rfid.PCD_Authenticate(
        MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));

    if (status != MFRC522::STATUS_OK) {
        Serial.print("Authentication failed: ");
        Serial.println(rfid.GetStatusCodeName(status));
        return false;
    }

    // Write data to the block
    status = rfid.MIFARE_Write(block, data, 16);

    if (status != MFRC522::STATUS_OK) {
        Serial.print("Write failed: ");
        Serial.println(rfid.GetStatusCodeName(status));
        return false;
    }

    return true;
}

void readFromBlock(byte block) {
    byte buffer[18];  // Buffer to hold the data (16 bytes + 2 extra for CRC)
    byte size = sizeof(buffer);

    // Authenticate before reading
    MFRC522::StatusCode status = rfid.PCD_Authenticate(
        MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));

    if (status != MFRC522::STATUS_OK) {
        Serial.print("Authentication failed: ");
        Serial.println(rfid.GetStatusCodeName(status));
        return;
    }

    // Read data from the block
    status = rfid.MIFARE_Read(block, buffer, &size);

    if (status != MFRC522::STATUS_OK) {
        Serial.print("Reading failed: ");
        Serial.println(rfid.GetStatusCodeName(status));
    } else {
        Serial.print("Data in Block "); Serial.print(block); Serial.print(": ");
        for (byte i = 0; i < 16; i++) {
            Serial.write(buffer[i]);  // Print as character
        }
        Serial.println();
    }
}
