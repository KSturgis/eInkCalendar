#include "PngExt.hh"

pixel_format Px;

float getBytesPerPixel(PNGDRAW *pDraw) {
  switch (pDraw->iPixelType) {
    case PNG_PIXEL_GRAYSCALE: return 1.0f;
    case PNG_PIXEL_TRUECOLOR: return 3.0f;
    case PNG_PIXEL_INDEXED: 
      switch (pDraw->iBpp) {
        case 8: return 1.0f;
        case 4: return 0.5f;
        case 2: return 0.25f;
        case 1: return 0.125f;
      }
    case PNG_PIXEL_GRAY_ALPHA: return 2.0f;
    case PNG_PIXEL_TRUECOLOR_ALPHA: return 4.0f;
  }
  return 0.0f;
}

pixel_format getPixel(PNGDRAW *pDraw, int x) {
    int pOffset = static_cast<int>(floorf(getBytesPerPixel(pDraw) * x));
    uint8_t *s = pDraw->pPixels + pOffset;
    
    switch (pDraw->iPixelType) {
        case PNG_PIXEL_GRAY_ALPHA:
            Px.r = Px.g = Px.b = *s++; // gray level
            Px.a = *s++;
            break;
        case PNG_PIXEL_GRAYSCALE:
            Px.r = Px.g = Px.b = *s++; // gray level
            Px.a = 255;
            break;
        case PNG_PIXEL_TRUECOLOR:
            Px.r = *s++;
            Px.g = *s++;
            Px.b = *s++;
            Px.a = 255;
            break;
        case PNG_PIXEL_INDEXED: // palette color (can be 1/2/4 or 8 bits per pixel)
            uint8_t c, *pPal;
            Px.a = 255;
            c = *s++;
            switch (pDraw->iBpp) {
                case 8: // 8-bit palette also supports palette alpha
                    if (pDraw->iHasAlpha) { 
                      Px.a = pDraw->pPalette[768+c]; // get alpha
                    }
                    break;
                case 4: // 4-bit palette
                    switch (x % 2) {
                      case 0: c = c >> 4; break;
                      case 1: c = c & 0xf; break;
                    }
                    break;
                case 2:
                    switch (x % 4) {
                      case 0: c = c >> 6; break;
                      case 1: c = (c >> 4) & 0x3; break;
                      case 2: c = (c >> 2) & 0x3; break;
                      case 3: c = c & 0x3; break;
                    }
                case 1:
                switch (x % 4) {
                      case 0: c = c >> 7; break;
                      case 1: c = (c >> 6) & 0x1; break;
                      case 2: c = (c >> 5) & 0x1; break;
                      case 3: c = (c >> 4) & 0x1; break;
                      case 4: c = (c >> 3) & 0x1; break;
                      case 5: c = (c >> 2) & 0x1; break;
                      case 6: c = (c >> 1) & 0x1; break;
                      case 7: c = c & 0x1; break;
                    }
                    break;
            } // switch on bits per pixel
            pPal = &pDraw->pPalette[c * 3];
            Px.r = pPal[0];
            Px.g = pPal[1];
            Px.b = pPal[2];
            break;
        case PNG_PIXEL_TRUECOLOR_ALPHA: // truecolor + alpha
            Px.r = *s++;
            Px.g = *s++;
            Px.b = *s++;
            Px.a = *s++;
            break;
    }

    return Px;
}
