#include "TimeFile.hh"
#include "../parser.h"

const char* time_filename = "/time.txt";

bool TimeFile::hasTimeFile() {
    return sdCard->doesFileExist("/time.txt");
}

PtTime TimeFile::getTimeFromTimeFile() {
    PtTime time;
    memset(&time, 0, sizeof(PtTime));

    Serial.println("Reading time from time file");
    uint8_t buf[128];

    File timeFile = sdCard->open(time_filename);
    int len = timeFile.read(buf, 128);
    String content = String(buf, len);

    std::vector<int> parts = parseInts(content, ",");
    if (parts.size() != 7) {
        Serial.println("Failed to set time");
        return time;
    }

    time.year = parts[0];
    // Convert from 4digit to 2digit 2022 -> 22
    time.year %= 100;
    time.month = parts[1];
    time.day = parts[2];
    time.dayOfWeek = parts[3];
    time.hours = parts[4];
    time.minutes = parts[5];
    time.seconds = parts[6];
    return time;
}

void TimeFile::removeTimeFile() {
    sdCard->remove(time_filename);
}