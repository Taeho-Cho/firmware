/*
 * UART.c
 *
 *  Created on: 2021. 10. 14.
 *      Author: chotaeho
 */

#include "UART.h"
#include "usart.h"


bool UART_init(eUART_CHANNEL_t channel, uint32_t baudrate)
{
	bool ret = true;

	switch(channel)
	{

	case eUART_CHANNEL_4 : {

		huart4.Instance = UART4;
		huart4.Init.BaudRate = baudrate;
		huart4.Init.WordLength = UART_WORDLENGTH_8B;
		huart4.Init.StopBits = UART_STOPBITS_1;
		huart4.Init.Parity = UART_PARITY_NONE;
		huart4.Init.Mode = UART_MODE_TX_RX;
		huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart4.Init.OverSampling = UART_OVERSAMPLING_16;
		huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
		if (HAL_UART_Init(&huart4) != HAL_OK)
		{
			ret = false;
		}

#if UART4_USE_INT
		HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(UART4_IRQn);
#else

#endif
	} break;

	default : ret = false; break;
	}

	return ret;
}


bool UART_send(eUART_CHANNEL_t channel, uint8_t *pData, uint16_t Size )
{
	bool ret = true;

	switch(channel)
	{

	case eUART_CHANNEL_4 : {
		if( HAL_UART_Transmit( &huart4, pData, Size, 1000 ) != HAL_OK ) ret = false;

#if UART4_USE_INT

#else

#endif
	} break;

	default : ret = false; break;
	}

	return ret;
}


bool UART_read(eUART_CHANNEL_t channel, uint8_t *pData, uint16_t Size )
{
	bool ret = true;

	switch(channel)
	{

	case eUART_CHANNEL_4 : {
#if UART4_USE_INT

#else
		if(HAL_UART_Receive(&huart4, pData, Size, 2000) != HAL_OK) ret = false;
#endif
	} break;

	default : ret = false; break;
	}

	return ret;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

#if UART4_USE_INT
  if( huart->Instance == UART4 )
  {

  }
#endif

}
