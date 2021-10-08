
#define _ADS8865_C_
    #include "ads8865.h"
#undef  _ADS8865_C_ 

static void MX_SPI1_Init(void);

SPI_HandleTypeDef hspi1;

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


/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}
