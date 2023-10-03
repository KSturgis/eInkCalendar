#ifndef SD_H
#define SD_H
#include <Arduino.h>
#include <FS.h>

class SDCard {    
public:
    SDCard(bool detect, bool enablePower) {
        this->sdCardDetect = detect;
        this->sdCardPower = enablePower;
    }

    bool initialize();
    bool doesFileExist(const char * filename);
    File open(const char* path);
    const char* randomFile(const char* dirname, char* filenameBuf);
    bool remove(const char* path);

private:
    bool sdCardDetect;
    bool sdCardPower;
};
#endif // SD_H