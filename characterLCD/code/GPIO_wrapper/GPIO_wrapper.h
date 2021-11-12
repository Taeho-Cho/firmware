/*
 * GPIO_wrapper.h
 *
 *  Created on: 2021. 11. 2.
 *      Author: chotaeho
 */

#ifndef DEVICE_CHARACTER_LCD_GPIO_WRAPPER_GPIO_WRAPPER_H_
#define DEVICE_CHARACTER_LCD_GPIO_WRAPPER_GPIO_WRAPPER_H_


#include "main.h"

typedef struct{
	GPIO_TypeDef   *port;
	uint16_t 		pin;
} sGPIO_t;


void writeGPIO(sGPIO_t *, uint8_t);
uint8_t readGPIO(sGPIO_t *);
void toggleGPIO(sGPIO_t *);


#endif /* DEVICE_CHARACTER_LCD_GPIO_WRAPPER_GPIO_WRAPPER_H_ */
