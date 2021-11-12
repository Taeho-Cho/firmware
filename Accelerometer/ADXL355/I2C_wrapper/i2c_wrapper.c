/*
 * i2c_wrapper.c
 *
 *  Created on: Nov 11, 2021
 *      Author: chotaeho
 */
#include "i2c_wrapper.h"
#include "i2c.h"


/*
 * @retval  true if it initialized the I2C, false if the I2C is already initialized
 */
bool I2C_init(eI2C_CHANNEL_t I2C_channel)
{
	bool ret = true;

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	switch(I2C_channel)
	{

	case eI2C_CHANNEL_3 : {
		if(hi2c3.Instance != I2C3)
		{
			/** Initializes the peripherals clock
			 */
			PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C3;
			PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
			if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
			{
				ret = false;
			}

			__HAL_RCC_GPIOC_CLK_ENABLE();
			/**I2C3 GPIO Configuration
				    PC0     ------> I2C3_SCL
				    PC1     ------> I2C3_SDA
			 */
			GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
			GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
			HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

			/* I2C3 clock enable */
			__HAL_RCC_I2C3_CLK_ENABLE();

			/* I2C3 init */
			hi2c3.Instance = I2C3;
			hi2c3.Init.Timing = 0x00300F33;
			hi2c3.Init.OwnAddress1 = 0;
			hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
			hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
			hi2c3.Init.OwnAddress2 = 0;
			hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
			hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
			hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
			if (HAL_I2C_Init(&hi2c3) != HAL_OK)
			{
				ret = false;
			}
			/** Configure Analogue filter
			 */
			if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
			{
				ret = false;
			}
			/** Configure Digital filter
			 */
			if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
			{
				ret = false;
			}
			/** I2C Fast mode Plus enable
			 */
			HAL_I2CEx_EnableFastModePlus(I2C_FASTMODEPLUS_I2C3);
		}
	} break;

	default : { ret = false; } break;

	}

	return ret;
}




bool I2C_isDeviceReady(eI2C_CHANNEL_t I2C_channel, uint16_t DevAddress)
{
	bool ret = true;

	switch(I2C_channel)
	{

	case eI2C_CHANNEL_3 : {
		if(HAL_I2C_IsDeviceReady(&hi2c3, DevAddress<<1, NUMBER_OF_TRIALS, I2C_TIMEOUT) != HAL_OK)
		{
			ret = false;
		}
	} break;

	default : { ret = false; } break;

	}

	return ret;
}


bool I2C_read(eI2C_CHANNEL_t I2C_channel, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
	bool ret = true;

	switch(I2C_channel)
	{

	case eI2C_CHANNEL_3 : {
		if(HAL_I2C_Mem_Read(&hi2c3, DevAddress<<1, MemAddress, MemAddSize, pData, Size, I2C_TIMEOUT) != HAL_OK)
		{
			ret = false;
		}
	} break;

	default : { ret = false; } break;

	}

	return ret;
}


bool I2C_write(eI2C_CHANNEL_t I2C_channel, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{

	bool ret = true;

	switch(I2C_channel)
	{

	case eI2C_CHANNEL_3 : {
		if(HAL_I2C_Mem_Write(&hi2c3, DevAddress<<1, MemAddress, MemAddSize, pData, Size, I2C_TIMEOUT) != HAL_OK)
		{
			ret = false;
		}
	} break;

	default : { ret = false; } break;

	}

	return ret;
}
