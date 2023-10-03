#include "SDCard.hh"
#include <SPI.h>
#include <SD.h>
#include "../PinConfig.h"

SPIClass sdSpi = SPIClass(VSPI);

void listDir(const char * dirname, uint8_t levels);

bool SDCard::initialize() {
    if (sdCardPower) {
        pinMode(SD_EN, OUTPUT);
        digitalWrite(SD_EN, LOW);
        // Wait for the SD card to boot
        Serial.println("Waiting for SD Card to power on");
        delay(2000L);
    }

    // Check the CardDetect pin on the sd card holder.
    // REV2 error no external pullup was used and this pin doesn't have a pullup.
    if (sdCardDetect) {
        pinMode(SD_CD, INPUT);
        int hasSD = digitalRead(SD_CD);
        if (hasSD == LOW) {
            Serial.println("Card not detected");
            return false;
        }
    }

    // Initialize the SD card file system.
    pinMode(SD_CS, OUTPUT);
    sdSpi.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);

    if (!SD.begin(SD_CS, sdSpi)) {
        Serial.println("Card Mount Failed");
        return false;
    } 

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    return true;
}

bool SDCard::doesFileExist(const char * filename) {
    return SD.exists(filename);
}

File SDCard::open(const char* path) {
    return SD.open(path);
}
    
const char* SDCard::randomFile(const char* dirname, char* filenameBuf) {
    Serial.printf("Getting random file from directory: %s\n", dirname);

    File root = SD.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return NULL;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return NULL;
    }

    int fileCount = 0;
    File file = root.openNextFile();
    while (file) {
        if(file.isDirectory()){
        } else {
            fileCount++;
        }
        file = root.openNextFile();
    }

    int selectedFile = random(fileCount);
    fileCount = 0;
    root.rewindDirectory();
    file = root.openNextFile();
    while (file) {
        if(file.isDirectory()) {
        } else {
            if (selectedFile == fileCount) {
                break;
            }
            fileCount++;
        }
        file = root.openNextFile();
    }

    Serial.printf("Selected file#%d: %s\n", fileCount, file.name());
    sprintf(filenameBuf, "%s%s", dirname, file.name());
    return filenameBuf;
}

bool SDCard::remove(const char* path) {
    return SD.remove(path);
}

/**
 * Debug method that lists a whole directory. 
 **/
void listDir(const char * dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\n", dirname);

    File root = SD.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}