/*
 * UART.c
 *
 *  Created on: 2021. 10. 14.
 *      Author: chotaeho
 */

#include "ringbuffer.h"
#include "UART_wrapper.h"
#include "usart.h"

#define	TX_TIMEOUT	1000
#define RX_TIMEOUT	2000


static sRING_BUFFER_t RX4ringBuffer;
static uint8_t rxdata;


bool UART_init(eUART_CHANNEL_t channel, uint32_t baudrate)
{
	bool ret = true;

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	switch(channel)
	{

	case eUART_CHANNEL_4 : {

		if(huart4.Instance != UART4)
		{
			/** Initializes the peripherals clock
			 */
			PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_UART4;
			PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
			if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
			{
				ret = false;
			}

			/* UART4 clock enable */
			__HAL_RCC_UART4_CLK_ENABLE();
			__HAL_RCC_GPIOA_CLK_ENABLE();
			/**UART4 GPIO Configuration
			    PA0     ------> UART4_TX
			    PA1     ------> UART4_RX
			 */
			GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
			GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			/* UART4 interrupt Init */
			HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
			HAL_NVIC_EnableIRQ(UART4_IRQn);

			huart4.Instance = UART4;
			huart4.Init.BaudRate = baudrate; // 38400
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

			/* UART4 start receiving data */
			HAL_UART_Receive_IT(&huart4, &rxdata, 1);
		}

	} break;

	default : {
		ret = false;
	} break;

	}

	return ret;
}


bool UART_send(eUART_CHANNEL_t channel, uint8_t *pData, uint16_t Size )
{
	bool ret = true;

	switch(channel)
	{

	case eUART_CHANNEL_4 : {
		if( HAL_UART_Transmit( &huart4, pData, Size, TX_TIMEOUT ) != HAL_OK ) ret = false;

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

		if(numberOfData(&RX4ringBuffer) >= Size)
		{
			for(uint8_t i = 0; i < Size; i++)
			{
				pop(&RX4ringBuffer, pData + i);
			}
		}
		else ret = false;

	} break;

	default : ret = false; break;
	}

	return ret;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if( huart->Instance == UART4 )
  {
	  push(&RX4ringBuffer, rxdata);
	  HAL_UART_Receive_IT(&huart4, &rxdata, 1);
  }

}
