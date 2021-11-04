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

#define _USE_LCD_RW_PIN_	0
#define _USE_4_BIT_MODE_ 	0


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

#define NUMBER_OF_CHAR_IN_LINE  16
#define X_POSITION_MAX			0xF
#define SECOND_LINE_OFFSET 		0x40
#define US_DELAY_TIME			700


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

typedef struct {
	char line1[NUMBER_OF_CHAR_IN_LINE];
	char line2[NUMBER_OF_CHAR_IN_LINE];
} sLCD_DATA_t;


typedef enum{
	eLCD_ERROR_NONE,
	eLCD_ERROR_X_POSITION_OUT_OF_RANGE,
	eLCD_ERROR_Y_POSITION_OUT_OF_RANGE,
	eLCD_ERROR_DATA_SIZE_OUT_OF_RANGE
} eLCD_ERROR_t;


typedef struct{
	sLCD_IO_t    IO;
	sLCD_DATA_t  data;
	eLCD_ERROR_t errorCode;
} sLCD_HANDLER_t;


void initLCD();
bool writeLCD(uint8_t xPosition, uint8_t yPosition, char *data, uint8_t size);


void ClearDisplay();
void ReturnHome();
void EntryModeSet(uint8_t ID, uint8_t S);
void DisplayOnOff(uint8_t D, uint8_t C, uint8_t B);
void CursorOrDisplayShift(uint8_t SC, uint8_t RL);
void FunctionSet(uint8_t DL, uint8_t N, uint8_t F);
void SetDDRAMaddress(uint8_t addr);
void WriteDataToRAM(uint8_t data);

#endif /* DEVICE_CHARACTER_LCD_H_ */
