#ifndef _ADS8865_H_
#define _ADS8865_H_

#include "main.h"

#ifdef _ADS8865_C_
    #define ADS8865_EXT
#else 
    #define ADS8865_EXT extern
#endif

ADS8865_EXT uint16_t ads8865_mVolt();

#endif
