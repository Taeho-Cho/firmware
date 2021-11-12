/*
 * adxl355.c
 *
 *  Created on: Nov 10, 2021
 *      Author: chotaeho
 */

#include "adxl355.h"
#include "i2c_wrapper.h"


/* @defgroup Static Symbol List
 * @{
 */
static bool ADXL355_isReady(uint16_t DevAddress);
static bool ADXL355_read (uint16_t MemAddress, uint8_t *pData, uint16_t Size);
static bool ADXL355_write(uint16_t MemAddress, uint8_t *pData, uint16_t Size);

static sADXL355_HANDLER_t hADXL = {0,};
static float temperatureValue = 0.0f;
/*
 * @}
 */


bool ADXL355_init()
{
	uint8_t temp_buf = 0;
	uREGISTER_8BIT_t temp_register = {0, };

	I2C_init(ADXL355_I2C_CHANNEL);

	if(ADXL355_isReady(DEVICE_ADDRESS_ASEL_HIGH) == true)
	{
		hADXL.DevAddr = DEVICE_ADDRESS_ASEL_HIGH;
	}
	else if(ADXL355_isReady(DEVICE_ADDRESS_ASEL_LOW) == true)
	{
		hADXL.DevAddr = DEVICE_ADDRESS_ASEL_LOW;
	}
	else
	{
		hADXL.error = eERROR_DEVICE_NOT_READY;
		return false;
	}


	ADXL355_read(REGISTER_ADDRESS_PARTID, &temp_buf, 1);
	if(temp_buf != ADXL355_DEVICE_ID)
	{
		hADXL.error = eERROR_WRONG_DEVICE_ID;
		return false;
	}


	/* G range setting */
	ADXL355_read(REGISTER_ADDRESS_RANGE, &temp_register.value, 1);
#if ADXL355_RANGE == RANGE_2G
	temp_register.bit0 = 1;
	temp_register.bit1 = 0;
#elif ADXL355_RANGE == RANGE_4G
	temp_register.bit0 = 0;
	temp_register.bit1 = 1;
#else
	temp_register.bit0 = 1;
	temp_register.bit1 = 1;
#endif /* ADXL355_RANGE == RANGE_xG */
	ADXL355_write(REGISTER_ADDRESS_RANGE, &temp_register.value, 1);


	/* POWER_CTL */
	ADXL355_read(REGISTER_ADDRESS_POWER_CTL, &temp_register.value, 1);
	temp_register.bit0 = 0;
	ADXL355_write(REGISTER_ADDRESS_POWER_CTL, &temp_register.value, 1);

	return true;
}


bool ADXL355_updateXYZ()
{

	int32_t xyzValue[3] = {0, };

	uint8_t xyzBuf[3][3] = {0,};

	if(ADXL355_read(REGISTER_ADDRESS_XDATA3, (uint8_t *)xyzBuf, 9) != true)
	{
		hADXL.error = eERROR_READ_WRITE_FAILED;
		return false;
	}

	for(uint8_t i = 0; i < 3; i++)
	{
		xyzValue[i]  = xyzBuf[i][0]<<24;
		xyzValue[i] |= xyzBuf[i][1]<<16;
		xyzValue[i] |= xyzBuf[i][2]<<8;
		xyzValue[i] >>= 12;
	}

	hADXL.AxisData.X_Axis = ((float)xyzValue[0]) * LSB_TO_G_SCALE_FACTOR;
	hADXL.AxisData.Y_Axis = ((float)xyzValue[1]) * LSB_TO_G_SCALE_FACTOR;
	hADXL.AxisData.Z_Axis = ((float)xyzValue[2]) * LSB_TO_G_SCALE_FACTOR;

	return true;
}


/*
 * @brief  scan the temperature value from ADXL355. But not accurate (shouldn't use)
 */
bool ADXL355_updateTemp()
{

	int16_t tempValue = 0;

	uint8_t tempBuf[2] = {0, };

	if(ADXL355_read(REGISTER_ADDRESS_TEMP2, (uint8_t *)tempBuf, 2) != true)
	{
		hADXL.error = eERROR_READ_WRITE_FAILED;
		return false;
	}

	tempValue  = tempBuf[0]<<8;
	tempValue |= tempBuf[1];

	if((tempValue & 0x800) != 0) tempValue |= 0xF<<12;

	temperatureValue = TEMPERATURE_NOMINAL_INTERCEPT_C + (((float)(tempValue - TEMPERATURE_NOMINAL_INTERCEPT_LSB)) / TEMPERATURE_NOMINAL_SLOPE);

	return true;
}


/* @defgroup Static Functions
 * @{
 */
bool ADXL355_isReady(uint16_t DevAddress)
{

	if(I2C_isDeviceReady(ADXL355_I2C_CHANNEL, DevAddress) != true)
	{
		return false;
	}

	return true;
}


bool ADXL355_read(uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{

	if(	I2C_read(ADXL355_I2C_CHANNEL, hADXL.DevAddr, MemAddress, ADXL355_MEMORY_ADDRESS_SIZE, pData, Size) != true )
	{
		return false;
	}

	return true;
}


bool ADXL355_write(uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{

	if( I2C_write(ADXL355_I2C_CHANNEL, hADXL.DevAddr, MemAddress, ADXL355_MEMORY_ADDRESS_SIZE, pData, Size) != HAL_OK)
	{
		return false;
	}

	return true;
}
/*
 * @}
 */
