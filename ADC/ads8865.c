
#define _ADS8865_C_
    #include "ads8865.h"
#undef  _ADS8865_C_ 

extern SPI_HandleTypeDef hspi1;

static int32_t PRES_default;

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


uint32_t BAT_mVolt()
{
  uint16_t buf = 0;
  float mVolt = 0;
  
  if(HAL_GPIO_ReadPin(ADC_SELECT_GPIO_Port, ADC_SELECT_Pin) != GPIO_PIN_SET){
    HAL_GPIO_WritePin(ADC_SELECT_GPIO_Port, ADC_SELECT_Pin, GPIO_PIN_SET);
    HAL_Delay(PWR_REBOOT_TIME);
  }
  
  buf = ads8865_mVolt();
  
  mVolt = ((float)(ADC_CODE_MAX - buf)) * (ADS8865_VOLT_REF/ADC_CODE_POSITIVE_VALUE_RANGE);
  
  if(mVolt > ADS8865_VOLT_REF) mVolt = ADS8865_VOLT_REF;
  
  return (uint32_t) (mVolt * OP_AMP_GAIN);
}


int32_t PRES_mVolt()
{
  uint16_t buf = 0;
  float mVolt = 0;
  int32_t water_level_cm = 0;
  
  if(HAL_GPIO_ReadPin(ADC_SELECT_GPIO_Port, ADC_SELECT_Pin) != GPIO_PIN_RESET){
    HAL_GPIO_WritePin(ADC_SELECT_GPIO_Port, ADC_SELECT_Pin, GPIO_PIN_RESET);
    HAL_Delay(PWR_REBOOT_TIME);
  }
  
  buf = ads8865_mVolt();

  
  mVolt = ((float)(ADC_CODE_MAX - buf)) * (ADS8865_VOLT_REF/ADC_CODE_POSITIVE_VALUE_RANGE);
  
  if(mVolt > ADS8865_VOLT_REF) mVolt = ADS8865_VOLT_REF;
  
  
  /* mVolt to mBar conversion */ 
  water_level_cm = (((mVolt * OP_AMP_GAIN)) * mVolt_to_mBar) - PRES_default;
  
  return water_level_cm;
}

void Set_PRES_default(int32_t pressure_default) { PRES_default = pressure_default; }
