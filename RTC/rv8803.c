
#define _RV_8803_C_
    #include "rv8803.h"
#undef _RV_8803_C_

static void MX_I2C3_Init(void);

I2C_HandleTypeDef hi2c3;

sRTC srtc;

void rtc_set_time(){

//   srtc.seconds = ;
//   srtc.minutes = ;
//   srtc.hours   = ;
//   srtc.date    = ;
//   srtc.month   = ;
//   srtc.year    = ;
   
  /* RC-8803-C7 setting */
  HAL_I2C_Mem_Write(&hi2c3, DEV_ID_RTC8803, RV8803_SECONDS, I2C_MEMADD_SIZE_8BIT, &srtc.seconds, 1, 10);
  HAL_I2C_Mem_Write(&hi2c3, DEV_ID_RTC8803, RV8803_MINUTES, I2C_MEMADD_SIZE_8BIT, &srtc.minutes, 1, 10);
  HAL_I2C_Mem_Write(&hi2c3, DEV_ID_RTC8803, RV8803_HOURS,   I2C_MEMADD_SIZE_8BIT, &srtc.hours,   1, 10);
//  HAL_I2C_Mem_Write(&hi2c3, DEV_ID_RTC8803, RV8803_WEEKDAY, I2C_MEMADD_SIZE_8BIT, &srtc.weekday, 1, 10);
  HAL_I2C_Mem_Write(&hi2c3, DEV_ID_RTC8803, RV8803_DATE,    I2C_MEMADD_SIZE_8BIT, &srtc.date,    1, 10);
  HAL_I2C_Mem_Write(&hi2c3, DEV_ID_RTC8803, RV8803_MONTH,   I2C_MEMADD_SIZE_8BIT, &srtc.month,   1, 10);
  HAL_I2C_Mem_Write(&hi2c3, DEV_ID_RTC8803, RV8803_YEAR,    I2C_MEMADD_SIZE_8BIT, &srtc.year,    1, 10);
}

void rtc_read_time(){

  /* RC-8803-C7 read */
  HAL_I2C_Mem_Read(&hi2c3, DEV_ID_RTC8803, RV8803_SECONDS, I2C_MEMADD_SIZE_8BIT, &srtc.seconds, 1, 10);
  HAL_I2C_Mem_Read(&hi2c3, DEV_ID_RTC8803, RV8803_MINUTES, I2C_MEMADD_SIZE_8BIT, &srtc.minutes, 1, 10);
  HAL_I2C_Mem_Read(&hi2c3, DEV_ID_RTC8803, RV8803_HOURS,   I2C_MEMADD_SIZE_8BIT, &srtc.hours,   1, 10);
//  HAL_I2C_Mem_Read(&hi2c3, DEV_ID_RTC8803, RV8803_WEEKDAY, I2C_MEMADD_SIZE_8BIT, &srtc.weekday, 1, 10);
  HAL_I2C_Mem_Read(&hi2c3, DEV_ID_RTC8803, RV8803_DATE,    I2C_MEMADD_SIZE_8BIT, &srtc.date,    1, 10);
  HAL_I2C_Mem_Read(&hi2c3, DEV_ID_RTC8803, RV8803_MONTH,   I2C_MEMADD_SIZE_8BIT, &srtc.month,   1, 10);
  HAL_I2C_Mem_Read(&hi2c3, DEV_ID_RTC8803, RV8803_YEAR,    I2C_MEMADD_SIZE_8BIT, &srtc.year,    1, 10);
}

void rtc_timer_init() {
  
  uint8_t buf[2] = {0,};
  
  /* Timer Counter setting */
  buf[0] = 30;
  buf[1] = 0;
  HAL_I2C_Mem_Write(&hi2c3, DEV_ID_RTC8803, TIMER_COUNTER_0, I2C_MEMADD_SIZE_8BIT, &buf[0], 1, 4);
  HAL_I2C_Mem_Write(&hi2c3, DEV_ID_RTC8803, TIMER_COUNTER_1, I2C_MEMADD_SIZE_8BIT, &buf[1], 1, 4);
  
  buf[0] = 0x00;
  
  /* TIE set */
  buf[0] = (1<<4);
  HAL_I2C_Mem_Write(&hi2c3, DEV_ID_RTC8803, RV8803_CTRL, I2C_MEMADD_SIZE_8BIT, buf, 1, 4);
  
  buf[0] = 0x00;
  
  /* TD set */
  buf[0] = 0x02;
  HAL_I2C_Mem_Write(&hi2c3, DEV_ID_RTC8803, RV8803_EXT, I2C_MEMADD_SIZE_8BIT, buf, 1, 4);
  
  /* TE set, timer start */
  buf[0] |= (1<<4);
  HAL_I2C_Mem_Write(&hi2c3, DEV_ID_RTC8803, RV8803_EXT, I2C_MEMADD_SIZE_8BIT, buf, 1, 4);
  
}


static void MX_I2C3_Init(void)
{

  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x00702685;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }

}

