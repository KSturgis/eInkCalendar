#ifndef IMAGE_SERVER_H
#define IMAGE_SERVER_H

#include "../sd/SDCard.hh"

enum ServerMode { SHUTDOWN, LOW_POWER, DISPLAY_MODE, NO_CARD, FIRST_IMAGE };

struct ServerConfig {
    bool useSdCardDetect;
    bool useExtOne;
    bool useSDCardPower;
};

class EInkServer {
public:
    EInkServer(ServerConfig cfg, SDCard* sdCard) {
        this->config = cfg;
        this->sdCard = sdCard;
    }

    void setup();
    bool loop();
    void shutdown();

    SDCard* getSdCard();

private:
    ServerMode mode;
    ServerConfig config;
    SDCard* sdCard;

    void checkAllDates();
    void decodeImage(const char * filename);
    void enableWakeUpFromRTC();
    const char* getImageToShow();
    void showImage(const char* filename);
};

#endif // IMAGE_SERVER_H