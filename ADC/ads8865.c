
#define _ADS8865_C_
    #include "ads8865.h"
#undef  _ADS8865_C_ 

extern SPI_HandleTypeDef hspi1;

uint16_t ads8865_mVolt()
{
  uint8_t buf[2];
  
  /* 3-WIRE OPERATION, ADS8865_DIN stays HIGH */    
  HAL_GPIO_WritePin(ADS8865_CONV_GPIO_Port, ADS8865_CONV_Pin, GPIO_PIN_SET);
  HAL_Delay(2);
  HAL_GPIO_WritePin(ADS8865_CONV_GPIO_Port, ADS8865_CONV_Pin, GPIO_PIN_RESET);
  HAL_SPI_Receive(&hspi1, buf, 1,1000);
  
  return *(uint16_t*)buf;
}

