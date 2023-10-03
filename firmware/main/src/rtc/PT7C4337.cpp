#include "PT7C4337.h"
#include <Wire.h>
#include <time.h>

const int RTC_ID = 0x68;

int bcdToDecimal(uint8_t s);
uint8_t decimalToBcd8(int s);

void PT7C4337::setTime(PtTime time) {
    bus->beginTransmission(RTC_ID);
    bus->write(0x00);    
    bus->write(decimalToBcd8(time.seconds));
    bus->write(decimalToBcd8(time.minutes));
    bus->write(decimalToBcd8(time.hours));
    bus->write(decimalToBcd8(time.dayOfWeek));
    bus->write(decimalToBcd8(time.day));
    bus->write(decimalToBcd8(time.month));
    bus->write(decimalToBcd8(time.year));
    bus->endTransmission();

    //Reset status
    bus->beginTransmission(RTC_ID);
    /*Address*/ bus->write(0x0F);  
    /*staus*/ bus->write(0x00); 
    bus->endTransmission(); 
}

void PT7C4337::setDailyAlarm(int hour, int minute, int second) {
    // Set the alarm to go off at midnight every day.
    // A1M4:1 A1M3:0, A1M2:0, A1M1:0, 
    // Seconds: 0
    bus->beginTransmission(RTC_ID);
    /*Address*/ bus->write(0x07);  
    /*A1S*/ bus->write(decimalToBcd8(second));     
    /*A1M*/ bus->write(decimalToBcd8(minute));  
    /*A1H*/ bus->write(decimalToBcd8(hour)); 
    // A1DD [AllDays/#Date] [DOW/#Date] {[DateBCD[6]] | [][][] [DowBCD[3]] }
    /*A1DD*/ bus->write(0x80);
     
    /*A2M*/ bus->write(0x00);
    /*A2H*/ bus->write(0x00);
    /*A2DD*/ bus->write(0x00);
    // CONTROL [#Enabled:0] [] [] [SquareWaveDrive[2]:0x11] [INTCN:0] [A2IE:0] [A1IE:1]
    /*CONTROL*/ bus->write(0x19);
    bus->endTransmission();
}

void PT7C4337::clearAlarms() {
    // read the current value of the status register
    bus->beginTransmission(RTC_ID);
    bus->write(0x0F);
    bus->endTransmission();
    bus->requestFrom(RTC_ID, 1);
    uint8_t status = bus->read();
    // Clear the two alarm bits
    status = status & 0xF6;

    //Write status
    bus->beginTransmission(RTC_ID);
    /*Address*/ bus->write(0x0F);  
    /*staus*/ bus->write(status); 
    bus->endTransmission(); 
}

void PT7C4337::readData() {
    // Select the temperature register at register_index.
    bus->beginTransmission(RTC_ID);
    bus->write(0x07);
    bus->endTransmission();

    // Start a transaction to read the register data.
    bus->requestFrom(RTC_ID, 9);
    Serial.print("A1S:"); Serial.println(String(bus->read(), HEX));
    Serial.print("A1M:"); Serial.println(String(bus->read(), HEX));
    Serial.print("A1H:"); Serial.println(String(bus->read(), HEX));
    Serial.print("A1DD:"); Serial.println(String(bus->read(), HEX));
    Serial.print("A2M:"); Serial.println(String(bus->read(), HEX));
    Serial.print("A2H:"); Serial.println(String(bus->read(), HEX));
    Serial.print("A2DD:");  Serial.println(String(bus->read(), HEX));
    Serial.print("CONTROL:"); Serial.println(String(bus->read(), HEX));
    Serial.print("STATUS:"); Serial.println(String(bus->read(), HEX));
}

PtTime PT7C4337::getTime() {
    // Select the temperature register at register_index.
    bus->beginTransmission(RTC_ID);
    bus->write(0x00);
    bus->endTransmission();

    // Start a transaction to read the register data.
    bus->requestFrom(RTC_ID, 7);
    PtTime time;
    time.seconds = bcdToDecimal(bus->read() & 0x7f);
    time.minutes = bcdToDecimal(bus->read() & 0x7f);
    time.hours = bcdToDecimal(bus->read() & 0x3f);
    time.dayOfWeek = bcdToDecimal(bus->read() & 0x07);
    time.day = bcdToDecimal(bus->read() & 0x3F);
    time.month = bcdToDecimal(bus->read() & 0x1F);
    time.year = bcdToDecimal(bus->read() & 0xFF);
    return time;
}

void printTime(PtTime time) {
    Serial.print(time.month);
    Serial.print("/");
    Serial.print(time.day);
    Serial.print("/");
    Serial.print(time.year);
    Serial.print(" ");
    Serial.print(time.dayOfWeek);
    Serial.print(" ");
    Serial.print(time.hours);
    Serial.print(":");
    if (time.minutes < 10) { Serial.print("0"); }
    Serial.print(time.minutes);
    Serial.print(":");
    if (time.seconds < 10) { Serial.print("0"); }
    Serial.print(time.seconds);
    Serial.println(".");
}

int bcdToDecimal(uint8_t s) {
    int sum = 0;
    sum += s & 0xf;
    sum += 10 * ((s>>4) & 0xf);
    return sum;
}

uint8_t decimalToBcd8(int s) {
    uint8_t bcd = (s / 10) << 4 | (s % 10);
    return bcd;
}


/**
 * Convert the [PtTime] to epochS using the system mktime() command, assuming the
 * century is 2000-2100.
 */
int64_t getEpochS(PtTime time) {
    struct tm t;
    time_t t_of_day;
    _TIME_T_ b;

    t.tm_year = 2000 + time.year;    
    t.tm_mon = time.month;
    t.tm_mday = time.day; 
    t.tm_hour = time.hours;
    t.tm_min = time.minutes;
    t.tm_sec = time.seconds;
    t.tm_isdst = -1; 
    t_of_day = mktime(&t);
    return (int64_t) t_of_day;
} 
