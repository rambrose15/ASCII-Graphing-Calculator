#ifndef __SCREEN_INFO_H__
#define __SCREEN_INFO_H__

#include "../numeric/big_rational.h"

struct ScreenInfo{
  // Screen size settings
  BigRational screenDimXL, screenDimXR, screenDimYL, screenDimYR;
  ScreenInfo(): screenDimXL{BigRational("-10")}, screenDimXR{BigRational("10")},
    screenDimYL{BigRational("-10")}, screenDimYR{BigRational("10")} {}
};

#endif
