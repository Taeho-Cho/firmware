/*
 * macro_variables.h
 *
 *  Created on: 2021. 11. 12.
 *      Author: chotaeho
 */

#ifndef ADXL355_MACRO_VARIABLES_H_
#define ADXL355_MACRO_VARIABLES_H_


/* @brief  Select one between eI2C_CHANNEL_1, eI2C_CHANNEL_2, and eI2C_CHANNEL_3
 *         this is from @ref eI2C_CHANNEL_t in "i2c_wrapper.h"
 */
#define ADXL355_I2C_CHANNEL  eI2C_CHANNEL_3


/* @brief  Select one between RANGE_2G, RANGE_4G, and RANGE_8G
 *         This is from @ref RANGE in "adxl355.h"
 */
#define ADXL355_RANGE  RANGE_4G


#endif /* ADXL355_MACRO_VARIABLES_H_ */
