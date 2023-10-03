#ifndef PNG_EXT_H
#define PNG_EXT_H
#include "../PNGdec/PNGdec.h"

/**
 * Al values from 0-255
 */
struct pixel_format {
  int r;
  int g;
  int b;
  int a;
};

pixel_format getPixel(PNGDRAW *pDraw, int x);

#endif //PNG_EXT_H