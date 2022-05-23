/*
 * character_lcd.h
 *
 *  Created on: Nov 1, 2021
 *      Author: chotaeho
 */

#ifndef DEVICE_CHARACTER_LCD_H_
#define DEVICE_CHARACTER_LCD_H_

/*
 * 1. Basic Setting
 *  - N=1, 2-line display
 *  - F=0, 5x8 dots font
 *  - D=1, display on
 *
 * 2. when turning on, resetting the LCD takes about 50ms with the status below
 *  - Display clear
 *  - DL=1,  8-bit interface
 *  - N=0,   1-line display
 *  - F=0,   5x8 dot character font
 *  - D=0,   Display off
 *  - C=0,   Cursor off
 *  - B=0,   Blinking off
 *  - I/D=1, Increment by 1
 *  - S=0,   No shift
 *
 * 3. DDRAM (Display Data RAM) address
 *  - 1st line from 0x00h to 0x0F
 *  - 2nd line from 0x40h to 0x4F
 *
 */

#include "main.h"
#include "GPIO_wrapper.h"
#include "macro_variable.h"


typedef struct{

	sGPIO_t RS;
#if _USE_LCD_RW_
	sGPIO_t RW;
#endif
	sGPIO_t E;

	/* Data Bus */
#if _USE_4_BIT_MODE_
	/* only use DB4 ~ DB7 */
#else
	sGPIO_t DB0;
	sGPIO_t DB1;
	sGPIO_t DB2;
	sGPIO_t DB3;
#endif /* _USE_4_BIT_MODE_ */
	sGPIO_t DB4;
	sGPIO_t DB5;
	sGPIO_t DB6;
	sGPIO_t DB7;

} sLCD_IO_t;


typedef enum{
	eLCD_ERROR_NONE,
	eLCD_ERROR_X_POSITION_OUT_OF_RANGE,
	eLCD_ERROR_Y_POSITION_OUT_OF_RANGE,
	eLCD_ERROR_DATA_SIZE_OUT_OF_RANGE
} eLCD_ERROR_t;



typedef struct{
	sLCD_IO_t    IO;
	eLCD_ERROR_t errorCode;
} sLCD_HANDLER_t;



#define NUMBER_OF_CHAR_IN_LINE  16
#define X_POSITION_MAX			0xF
#define SECOND_LINE_OFFSET 		0x40


/** @defgroup Wait Time in micro-second
  * @{
  */
#define WAIT_TIME_CLEAR_DISPLAY					1520
#define WAIT_TIME_RETURN_HOME					1520
#define WAIT_TIME_ENTRY_MODE_SET				37
#define WAIT_TIME_DISPLAY_ON_OFF				37
#define WAIT_TIME_CURSOR_OR_DISPLAY_SHIFT		37
#define WAIT_TIME_FUNCTION_SET					37
#define WAIT_TIME_SET_CGRAM_ADDRESS				37
#define WAIT_TIME_SET_DDRAM_ADDRESS				37
#define WAIT_TIME_READ_BUSY_FLAG_AND_ADDRESS 	0
#define WAIT_TIME_WRITE_DATA_TO_RAM				37
#define WAIT_TIME_READ_DATA_FROM_RAM			37
/**
  * @}
  */


#define ENABLE_DELAY 1	// micro-second


#define POWER_ON_WAIT_TIME 	40 // milli-second


void initLCD();


bool writeLCD(
		uint8_t xPosition,	/*!< the value ranges from 00h to 0Fh */

		uint8_t yPosition,	/*!< line selection
                                 This parameter can be a value of @ref Y_POSITION */

		char *data, 		/*!< date to write to RAM */

		uint8_t size 		/*!< data size, should not go over the limit */
);
/*
 * @defgroup macros for the function writeLCD
 * @{
 */
/*
 * @defgroup Y_POSITION
 * @{
 */
#define Y_POSITION_1ST_LINE	0
#define Y_POSITION_2ND_LINE	1
/*
 * @}
 */
/*
 * @}
 */


/** @defgroup character LCD instructions
  * @{
  */
void clear_display();
void return_home();
void entry_mode_set(uint8_t ID, uint8_t S);
void display_on_off(uint8_t D, uint8_t C, uint8_t B);
void cursor_or_display_shift(uint8_t SC, uint8_t RL);
void function_set(uint8_t DL, uint8_t N, uint8_t F);
void set_DDRAM_address(uint8_t addr);
void write_data_to_RAM(uint8_t data);
/**
  * @}
  */


/* LCD IO pin settings begin */
#ifndef LCD_DB4_Pin
	#define LCD_DB4_Pin GPIO_PIN_10
#endif
#ifndef LCD_DB4_GPIO_Port
	#define LCD_DB4_GPIO_Port GPIOB
#endif

#ifndef LCD_DB5_Pin
	#define LCD_DB5_Pin GPIO_PIN_11
#endif
#ifndef LCD_DB5_GPIO_Port
	#define LCD_DB5_GPIO_Port GPIOB
#endif

#ifndef LCD_DB6_Pin
	#define LCD_DB6_Pin GPIO_PIN_12
#endif
#ifndef LCD_DB6_GPIO_Port
	#define LCD_DB6_GPIO_Port GPIOB
#endif

#ifndef LCD_DB7_Pin
	#define LCD_DB7_Pin GPIO_PIN_13
#endif
#ifndef LCD_DB7_GPIO_Port
	#define LCD_DB7_GPIO_Port GPIOB
#endif

#ifndef LCD_E_Pin
	#define LCD_E_Pin GPIO_PIN_14
#endif
#ifndef LCD_E_GPIO_Port
	#define LCD_E_GPIO_Port GPIOB
#endif

#ifndef LCD_RS_Pin
	#define LCD_RS_Pin GPIO_PIN_15
#endif
#ifndef LCD_RS_GPIO_Port
	#define LCD_RS_GPIO_Port GPIOB
#endif

#if _USE_LCD_RW_PIN_
#ifndef LCD_RW_Pin
	#define LCD_RW_Pin GPIO_PIN_5
#endif
#ifndef LCD_RW_GPIO_Port
	#define LCD_RW_GPIO_Port GPIOB
#endif
#endif /* _USE_LCD_RW_PIN_ */

#if _USE_4_BIT_MODE_
	/* only use DB4 ~ DB7 */
#else
#ifndef LCD_DB0_Pin
	#define LCD_DB0_Pin GPIO_PIN_6
#endif
#ifndef LCD_DB0_GPIO_Port
	#define LCD_DB0_GPIO_Port GPIOB
#endif

#ifndef LCD_DB1_Pin
	#define LCD_DB1_Pin GPIO_PIN_7
#endif
#ifndef LCD_DB1_GPIO_Port
	#define LCD_DB1_GPIO_Port GPIOB
#endif

#ifndef LCD_DB2_Pin
	#define LCD_DB2_Pin GPIO_PIN_8
#endif
#ifndef LCD_DB2_GPIO_Port
	#define LCD_DB2_GPIO_Port GPIOB
#endif

#ifndef LCD_DB3_Pin
	#define LCD_DB3_Pin GPIO_PIN_9
#endif
#ifndef LCD_DB3_GPIO_Port
	#define LCD_DB3_GPIO_Port GPIOB
#endif
#endif /* _USE_4_BIT_MODE_ */
/* LCD IO pin settings end */


#endif /* DEVICE_CHARACTER_LCD_H_ */
