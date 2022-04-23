/*
 * usDelay.c
 *
 *  Created on: Nov 2, 2021
 *      Author: chotaeho
 */

#include "usDelay.h"
#include "tim.h"


bool usDelayInit()
{
	bool ret = true;
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	if(htim6.Instance != TIM6)
	{
		/* TIM6 clock enable */
		__HAL_RCC_TIM6_CLK_ENABLE();

		htim6.Instance = TIM6;
		htim6.Init.Prescaler = 80 - 1;
		htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim6.Init.Period = 65535;
		htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
		{
			ret = false;
		}
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
		{
			ret = false;
		}
	}

	HAL_TIM_Base_Start(&htim6);

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
		__HAL_TIM_SET_COUNTER(&htim6, 0);  // set the counter value a 0
		while (__HAL_TIM_GET_COUNTER(&htim6) < us);  // wait for the counter to reach the us input in the parameter
	}

	return ret;
}

