/*
 * GPIO_wrapper.c
 *
 *  Created on: 2021. 11. 2.
 *      Author: chotaeho
 */
#include "GPIO_wrapper.h"


/*
 * @param  value  0 for Low, 1 for High
 */
void writeGPIO(sGPIO_t * gpio, uint8_t value)
{
	if(value == 0) HAL_GPIO_WritePin(gpio->port, gpio->pin, GPIO_PIN_RESET);
	else 		   HAL_GPIO_WritePin(gpio->port, gpio->pin, GPIO_PIN_SET);
}


uint8_t readGPIO(sGPIO_t * gpio)
{
	uint8_t ret = 0;

	if(HAL_GPIO_ReadPin(gpio->port, gpio->pin) == GPIO_PIN_SET) ret = 1;

	return ret;
}


void toggleGPIO(sGPIO_t * gpio)
{
	HAL_GPIO_TogglePin(gpio->port, gpio->pin);
}
