#ifndef EPAPER_H
#define EPAPER_H
#include <stdint.h>
#include "../image/PngExt.hh"

/**
 * Class using to send an image to the EPaper display.
*/
class EPaper {
public:
    void initialize();
    void nextImageChannel();
    void showImage();
    void sendByte(uint8_t data); 
    void shutdown();   
};

uint8_t getClosestEpColor(pixel_format px);

#endif // EPAPER_H 