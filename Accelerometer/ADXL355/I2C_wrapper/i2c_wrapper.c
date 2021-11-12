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

	switch(I2C_channel)
	{

	case eI2C_CHANNEL_3 : {
		if(hi2c3.Instance != I2C3)
		{
			MX_I2C3_Init();
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
