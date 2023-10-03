#include "EInkServer.hh"
#include <Arduino.h>
#include <FS.h>
#include <SPI.h>
#include <Wire.h>
#include "../PinConfig.h"
#include "../epaper/EPaper.hh"
#include "../PNGdec/PNGdec.h"
#include "../rtc/PT7C4337.h"
#include "../rtc/TimeFile.hh"
#include "../rtc/TimeServer.hh"
#include "../lm75/LM75.hh"
#include "../VBat.hh"

// Internal methods
void* openImageFile(const char *filename, int32_t *size);
void closeImageFile(void* handle);
int32_t readFromFile(PNGFILE *handle, uint8_t *buffer, int32_t length);
int32_t seekFile(PNGFILE *handle, int32_t position);
void handlePNGLine(PNGDRAW *pDraw);

PNG png;
int colorPass = 0;
PT7C4337 rtc;
EPaper ePaper;
File imageFile;
PtTime current_time;
char filenameBuf[127];
EInkServer* currentServer = 0;

const char* low_power_filename = "/low_power.png";
const char* first_time_filename = "/first_display.png";

void EInkServer::setup() {
    // First check our battery voltage
    BatteryCondition batteryCondition = getBatteryCondition();
    if (batteryCondition == BAT_CRITICAL) {
        // If we have <MV_SHUTDOWN volts, don't even attempt to start up.
        mode = SHUTDOWN;
        return;
    }

    // Set-up the real time clock
    Wire.begin();

    // Talk to the Real time clock and get the current time.
    pinMode(PIN_RTC_INT, INPUT_PULLUP);
    current_time = rtc.getTime();
    rtc.clearAlarms();
    rtc.setDailyAlarm(0, 0, 0);
    Serial.print("Current time is ");
    printTime(current_time);

    // Set the randomSeed based on the clock
    int64_t timeS = getEpochS(current_time);
    randomSeed(static_cast<uint32_t>(timeS));

    // Measure the temperature, so we can updated the ePaper device with the temperature
    // TODO update EPD so it can call the temp
    LM75 tempSensor = LM75(&Wire, 0x48);
    tempSensor.enable();
    float temperatureC = tempSensor.readTemperatureC();
    tempSensor.shutdown();
    Serial.printf("Temperature = %2.3f C\n", temperatureC);

    // Setup the SDCard 
    bool sdCardAvailable = getSdCard()->initialize();
    if (!sdCardAvailable) {
        mode = NO_CARD;
        return;
    }

    TimeFile timeFile = TimeFile(getSdCard());
    if (timeFile.hasTimeFile()) {    
        // Set time from the file.
        PtTime time = timeFile.getTimeFromTimeFile();

        Serial.print("Setting time to ");
        printTime(time);
        rtc.setTime(time);

        // Reset the current_time and alarms
        current_time = rtc.getTime();

        // Delete the time file
        timeFile.removeTimeFile();
    }
    
    const char* filename;
    // Check if we should show the change the battery image
    if (batteryCondition == BAT_LOW && sdCard->doesFileExist(low_power_filename)) {
        filename = low_power_filename;
        mode = LOW_POWER;
    } else if (sdCard->doesFileExist(first_time_filename)) {
        // Check if we should be in OOBE mode, which is indicated by the prescence of the first_time_filename
        filename = first_time_filename;
        mode = FIRST_IMAGE;
    } else {
        filename = getImageToShow();
        if (!filename) {
            filename = sdCard->randomFile("/", filenameBuf);
        }
        mode = DISPLAY_MODE;
    }

    if (filename) {
        showImage(filename);
    } else {
        mode = NO_CARD;
        Serial.println("No file found");
    }

    if (config.useSDCardPower) {
        digitalWrite(SD_CS, HIGH);
        digitalWrite(SD_EN, HIGH);
    }
}

void EInkServer::showImage(const char* filename) {
    Serial.printf("Showing %s %d\n", filename, mode);
    // ePaper initialization
    ePaper.initialize();

    Serial.println("Loading KW Channel");
    colorPass = 0;
    decodeImage(filename);
    // Set next Channel, and then decode the image again.
    Serial.println("Loading RED Channel");
    colorPass = 1;
    ePaper.nextImageChannel();
    decodeImage(filename);
    
    Serial.println("Starting show command");
    ePaper.showImage();
    Serial.println("Done showing");
    ePaper.shutdown();
}

bool EInkServer::loop() {
    if (mode == FIRST_IMAGE) {
        TimeServer timeServer = TimeServer();
        if (timeServer.isWifiAvailable()) {           
            Serial.println("Setting clock from NTP EInkServer. Opening WiFi connection.");
            timeServer.initialize();
            Serial.println("Getting time from WiFi.");
            PtTime ntpTime = timeServer.getTimeFromWifi();
            Serial.print("Setting time to ");
            printTime(ntpTime);
            rtc.setTime(ntpTime);
        } else {
            Serial.println("WiFi connection not available");
        }

        // Check that all days have a 
        checkAllDates();
    }

    return false;
}

void EInkServer::shutdown() {
    // Only enable wake up if are in certain modes.
    if (mode == DISPLAY_MODE || mode == LOW_POWER) {
        enableWakeUpFromRTC();        
        Serial.println("\r\nGoing sleep, see you after midnight\r\n");
    } else if (mode == FIRST_IMAGE) {
        Serial.println("Removing OOBE image, next boot will be calendar");
        sdCard->remove(first_time_filename);
    } else {
        Serial.println("\r\nGoodnight moon\r\n");
    }

    //Go to sleep now
    esp_deep_sleep_start();
}

void EInkServer::decodeImage(const char* filename) {
    currentServer = this;
    int rc = png.open((const char *)filename, openImageFile, closeImageFile, readFromFile, seekFile, handlePNGLine);
    currentServer = 0;
    if (rc == PNG_SUCCESS) {
      Serial.printf("Image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
      rc = png.decode(NULL, 0);
      png.close();
    } else {
      Serial.printf("Failed to load the image rc:%d\n", rc);
    }
}

SDCard* EInkServer::getSdCard() {
    return sdCard;
}

void* openImageFile(const char *filename, int32_t *size) {
    imageFile = currentServer->getSdCard()->open(filename);
    *size = imageFile.size();
    return &imageFile;
}

void closeImageFile(void *handle) {
    Serial.println("Done reading file");
    ((File*)handle)->close();
}

int32_t readFromFile(PNGFILE *handle, uint8_t *buffer, int32_t length) {
    return ((File*)(handle->fHandle))->read(buffer, length);
}

int32_t seekFile(PNGFILE *handle, int32_t position) {
    return ((File*)(handle->fHandle))->seek(position);
}

// Function to draw pixels to the display
void handlePNGLine(PNGDRAW *pDraw) { 
  int c;
  uint8_t data = 0x0;
  uint8_t b;
  for (int x = 0; x < pDraw->iWidth; x++) {
    pixel_format px = getPixel(pDraw, x);
    c = getClosestEpColor(px);

    if (colorPass == 0 /*KW*/ && c == 1 /* isWhite */) {
        b = 1;
    } else if (colorPass == 1 /*RED*/ && c == 2 /* isRed */) {
        b = 1;
    } else {
        b = 0x0;
    }
    // Shift into data and send every 8 bits
    data <<= 1;
    data |= b;
    if (x % 8 == 7) {
        ePaper.sendByte(data);
    }
  }
}

void EInkServer::checkAllDates() {
    // Make sure files exist
    for (int yr = 22; yr <= 23; yr++) {
        int mn;
        if (yr == 22) { 
            mn = 12; 
        } else {
            mn = 1; 
        }
        for (; mn <= 12; mn++) {
            for (int d = 1; d <= 31; d++) {
                current_time.year = yr;
                current_time.month = mn;
                current_time.day = d;
                current_time.dayOfWeek = 1;
                current_time.hours = 0;
                current_time.minutes = 0;
                current_time.seconds = 0;

                const char* imageForDay = getImageToShow();
                if (imageForDay) {           
                    Serial.printf("Showing \"%s\" for %d/%d/%d\r\n", 
                        imageForDay,
                        current_time.day,
                        current_time.month, 
                        current_time.year);
                } else {
                    Serial.printf("I have no image for %d/%d/%d\r\n",
                        current_time.day,
                        current_time.month, 
                        current_time.year);
                }
            }
        }
    }
}

const char* EInkServer::getImageToShow() {
    // Check YearMonthDate
    sprintf(filenameBuf, "/20%02d%02d%02d.png", current_time.year, current_time.month, current_time.day);
    //Serial.printf("Checking YMD %s\n", filenameBuf);
    if (sdCard->doesFileExist(filenameBuf)) {
        return filenameBuf;
    }

    // Check MonthDate
    sprintf(filenameBuf, "/%02d%02d.png", current_time.month, current_time.day);
    //Serial.printf("Checking MD %s\n", filenameBuf);
    if (sdCard->doesFileExist(filenameBuf)) {
        return filenameBuf;
    }

    // Check Day
    sprintf(filenameBuf, "/%02d.png", current_time.day);
    //Serial.printf("Checking D %s\n", filenameBuf);
    if (sdCard->doesFileExist(filenameBuf)) {
        return filenameBuf;
    }

    return NULL;
}

void EInkServer::enableWakeUpFromRTC() {
    // in rev2 a pull up resistor was added, this will allow us to use
    // ext1 which disables the peripheral system and saves battery, but we need
    // to use ext0 pn REV 1 since we do not have the pullup on RTC_INT
    if (config.useExtOne) {
        esp_sleep_enable_ext1_wakeup(0x1 << PIN_RTC_INT, ESP_EXT1_WAKEUP_ALL_LOW);
    } else {
        esp_sleep_enable_ext0_wakeup(PIN_RTC_INT, LOW);
    }
}
