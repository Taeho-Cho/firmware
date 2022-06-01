/*
 * usDelay.c
 *
 *  Created on: Nov 2, 2021
 *      Author: chotaeho
 */

#include "usDelay.h"
/*
 * this library uses the base timers.
 * please go to "stm32l4xx_hal_conf.h" and uncomment "#define HAL_TIM_MODULE_ENABLED".
 *
 */


/*
 * @ this macro variable's value can be either TIM6, TIM7, TIM16 or TIM17
 */
#define US_DELAY_TIMER	TIM6


/* @defgroup time base units MSP init, deinit. those are copies from HAL functions.
 * @{
 */
void TIM_Base_MspInit  (TIM_HandleTypeDef* tim_baseHandle);
void TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle);
/*
 * @}
 */


TIM_HandleTypeDef timer_handler;


bool usDelayInit()
{
	bool ret = true;
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	if(timer_handler.Instance != US_DELAY_TIMER)
	{
		/* TIM6 clock enable */
		__HAL_RCC_TIM6_CLK_ENABLE();

		timer_handler.Instance = US_DELAY_TIMER;
		timer_handler.Init.Prescaler = 80 - 1;
		timer_handler.Init.CounterMode = TIM_COUNTERMODE_UP;
		timer_handler.Init.Period = 65535;
		timer_handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_Base_Init(&timer_handler) != HAL_OK)
		{
			ret = false;
		}

		TIM_Base_MspInit(&timer_handler);

		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&timer_handler, &sMasterConfig) != HAL_OK)
		{
			ret = false;
		}
	}

	HAL_TIM_Base_Start(&timer_handler);

	return ret;
}


bool usDelay(uint16_t us)
{
	bool ret = true;

	if(us > 1000)
	{
		ret = false;
	}
	else
	{
		__HAL_TIM_SET_COUNTER(&timer_handler, 0);  // set the counter value a 0
		while (__HAL_TIM_GET_COUNTER(&timer_handler) < us);  // wait for the counter to reach the us input in the parameter
	}

	return ret;
}



void TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM6)
  {
  /* USER CODE BEGIN TIM6_MspInit 0 */

  /* USER CODE END TIM6_MspInit 0 */
    /* TIM6 clock enable */
    __HAL_RCC_TIM6_CLK_ENABLE();
  /* USER CODE BEGIN TIM6_MspInit 1 */

  /* USER CODE END TIM6_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM7)
  {
  /* USER CODE BEGIN TIM7_MspInit 0 */

  /* USER CODE END TIM7_MspInit 0 */
    /* TIM7 clock enable */
    __HAL_RCC_TIM7_CLK_ENABLE();
  /* USER CODE BEGIN TIM7_MspInit 1 */

  /* USER CODE END TIM7_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM16)
  {
  /* USER CODE BEGIN TIM16_MspInit 0 */

  /* USER CODE END TIM16_MspInit 0 */
    /* TIM16 clock enable */
    __HAL_RCC_TIM16_CLK_ENABLE();
  /* USER CODE BEGIN TIM16_MspInit 1 */

  /* USER CODE END TIM16_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM17)
  {
  /* USER CODE BEGIN TIM17_MspInit 0 */

  /* USER CODE END TIM17_MspInit 0 */
    /* TIM17 clock enable */
    __HAL_RCC_TIM17_CLK_ENABLE();
  /* USER CODE BEGIN TIM17_MspInit 1 */

  /* USER CODE END TIM17_MspInit 1 */
  }
}

void TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM6)
  {
  /* USER CODE BEGIN TIM6_MspDeInit 0 */

  /* USER CODE END TIM6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM6_CLK_DISABLE();
  /* USER CODE BEGIN TIM6_MspDeInit 1 */

  /* USER CODE END TIM6_MspDeInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM7)
  {
  /* USER CODE BEGIN TIM7_MspDeInit 0 */

  /* USER CODE END TIM7_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM7_CLK_DISABLE();
  /* USER CODE BEGIN TIM7_MspDeInit 1 */

  /* USER CODE END TIM7_MspDeInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM16)
  {
  /* USER CODE BEGIN TIM16_MspDeInit 0 */

  /* USER CODE END TIM16_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM16_CLK_DISABLE();
  /* USER CODE BEGIN TIM16_MspDeInit 1 */

  /* USER CODE END TIM16_MspDeInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM17)
  {
  /* USER CODE BEGIN TIM17_MspDeInit 0 */

  /* USER CODE END TIM17_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM17_CLK_DISABLE();
  /* USER CODE BEGIN TIM17_MspDeInit 1 */

  /* USER CODE END TIM17_MspDeInit 1 */
  }
}

