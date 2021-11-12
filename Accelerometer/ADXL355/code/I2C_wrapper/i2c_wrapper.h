/*
 * i2c_wrapper.h
 *
 *  Created on: Nov 11, 2021
 *      Author: chotaeho
 */

#ifndef ADXL355_I2C_WRAPPER_I2C_WRAPPER_H_
#define ADXL355_I2C_WRAPPER_I2C_WRAPPER_H_

#include "main.h"


#define NUMBER_OF_TRIALS  3
#define I2C_TIMEOUT       100


typedef enum{
	eI2C_CHANNEL_1,
	eI2C_CHANNEL_2,
	eI2C_CHANNEL_3
} eI2C_CHANNEL_t;


bool I2C_init(eI2C_CHANNEL_t I2C_channel);
bool I2C_isDeviceReady(eI2C_CHANNEL_t I2C_channel, uint16_t DevAddress);
bool I2C_read (eI2C_CHANNEL_t I2C_channel, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
bool I2C_write(eI2C_CHANNEL_t I2C_channel, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

#endif /* ADXL355_I2C_WRAPPER_I2C_WRAPPER_H_ */
