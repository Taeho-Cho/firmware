#ifndef _ADS8865_H_
#define _ADS8865_H_

#include "main.h"

#ifdef _ADS8865_C_
    #define ADS8865_EXT
#else 
    #define ADS8865_EXT extern
#endif

#define Atmosphere_mVolt (2100.0)
#define mVolt_to_mBar    (2500.0/4000.0)

#define ADS8865_VOLT_REF  3000.0

#define OP_AMP_GAIN       8.0

#define ADC_CODE_MAX                    0xFFFF
#define ADC_CODE_POSITIVE_VALUE_RANGE   32768.0


ADS8865_EXT uint16_t ads8865_mVolt();

//float BAT_mVolt();
ADS8865_EXT uint32_t BAT_mVolt();

ADS8865_EXT int32_t PRES_mVolt();

ADS8865_EXT void Set_PRES_default(int32_t pressure_default);

#endif