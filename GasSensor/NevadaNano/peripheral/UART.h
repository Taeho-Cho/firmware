/*
 * UART.h
 *
 *  Created on: 2021. 10. 14.
 *      Author: chotaeho
 */

#ifndef PERIPHERAL_UART_UART_H_
#define PERIPHERAL_UART_UART_H_


#include "main.h"


/*
 *  UARTx Global Interrupt will be used?
 *    0 - interrupt is not used
 *    1 - interrupt is used
 */
#define UART1_USE_INT 	0
#define UART2_USE_INT 	0
#define UART3_USE_INT 	0
#define UART4_USE_INT 	0 // 1
#define UART5_USE_INT 	0



typedef enum {
	eUART_CHANNEL_1,
	eUART_CHANNEL_2,
	eUART_CHANNEL_3,
	eUART_CHANNEL_4,
	eUART_CHANNEL_5
} eUART_CHANNEL_t;



bool UART_init(eUART_CHANNEL_t channel, uint32_t baudrate);

bool UART_send(eUART_CHANNEL_t channel, uint8_t *pData, uint16_t Size );

bool UART_read(eUART_CHANNEL_t channel, uint8_t *pData, uint16_t Size );


#endif /* PERIPHERAL_UART_UART_H_ */
