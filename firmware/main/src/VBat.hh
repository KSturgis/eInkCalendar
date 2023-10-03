#ifndef VBAT_H
#define VBAT_H

#include <Arduino.h>
#include "PinConfig.h"

#define NUM_LOOPS 10
#define MV_SHUTDOWN 2000
#define MV_LOW_POWER 2100

enum BatteryCondition {
    BAT_NORMAL = 0,
    BAT_LOW = 1,
    BAT_CRITICAL = 2
};

uint32_t readBatteryVoltage() {
    pinMode(PIN_VBAT_AD, INPUT);

    analogReadResolution(12);
    uint32_t tempMv = 0;
    uint32_t accum;
    for (int loop_num = 0; loop_num < NUM_LOOPS; loop_num++) {
        delay(10);
        tempMv = analogReadMilliVolts(PIN_VBAT_AD);
        accum += tempMv;
    }
    tempMv = accum / NUM_LOOPS;

    return tempMv;
}

BatteryCondition getBatteryCondition() {
    uint32_t batteryMv = readBatteryVoltage();   
    Serial.printf("VBat is %f\n", (batteryMv / 1000.0f));
    if (batteryMv <= MV_SHUTDOWN) {
        return BAT_CRITICAL;
    } else if (batteryMv <= MV_LOW_POWER) {
        return BAT_LOW;
    } else {
        return BAT_NORMAL;
    }
}

#endif // VBAT_H