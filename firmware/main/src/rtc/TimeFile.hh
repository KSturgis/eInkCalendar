#ifndef TIME_FILE_H
#define TIME_FILE_H
#include "PT7C4337.h"
#include "../sd/SDCard.hh"

/**
 * Used when setting the Clock time from a text file on the SD card.
 * 
 * The file should have a single text line with the following format
 * year,month,day,dayOfWeek,hours,minutes,seconds
 * 
 * For example
 * 2023,3,24,6,20,3,32\r\n
 * 
**/
class TimeFile {
public:
    TimeFile(SDCard* sdCard) {
        this->sdCard = sdCard;
    }

    /** Does the SD card have a file that includes the time. */
    bool hasTimeFile();
    /** Read time from the time file. */
    PtTime getTimeFromTimeFile();
    /** Remove the time file from the SD card. */
    void removeTimeFile();
private:
    SDCard* sdCard;
};
#endif //TIME_FILE_H