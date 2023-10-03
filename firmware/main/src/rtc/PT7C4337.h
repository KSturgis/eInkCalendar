#ifndef PT7C4337_H
#define PT7C4337_H

#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>

const int DEFAULT_I2C_ADDRESS = 0x68;

struct PtTime {
    int seconds;
    int minutes;
    int hours;
    int dayOfWeek;
    int day;
    int month;
    int year;
};

/**
 * reads the current time from a Real Time Clock IC (PT7C4337)
 * Using the I2C bus.
 **/
class PT7C4337 {
protected:
    TwoWire *bus;
    uint8_t i2c_address;

public:
    PT7C4337(TwoWire *bus = &Wire, uint8_t i2c_address = DEFAULT_I2C_ADDRESS) {
        this->bus = bus;
        this->i2c_address = i2c_address;
    }
    /* Get the current time from the RTC */
    PtTime getTime();
    /* Set the time of the real time clock to time. */
    void setTime(PtTime time);
    /* Set an alarm on Interupt 1 to interrupt at midnight on each day.  */
    void setDailyAlarm(int hour, int minute, int second);
    /* Remove al alarms */
    void clearAlarms();
    /* Print debug information of the registers in the RTC */
    void readData();
};

void printTime(PtTime time);
int64_t getEpochS(PtTime time);

#endif // PT7C4337_H