/*
 * character_lcd.c
 *
 *  Created on: Nov 1, 2021
 *      Author: chotaeho
 */
#include "character_lcd.h"
#include "usDelay.h"


static sLCD_HANDLER_t hLCD;

#if _USE_4_BIT_MODE_
static void enable_4_bit_mode();
#endif /* _USE_4_BIT_MODE_ */


#define SET_OR_RESET_ENABLE(_ONE_OR_ZERO_)		do{\
													usDelay(ENABLE_DELAY);\
													writeGPIO(&hLCD.IO.E, _ONE_OR_ZERO_);\
													usDelay(ENABLE_DELAY);\
												}while(0)



void initLCD()
{
	usDelayInit();

	hLCD.IO.RS.port = LCD_RS_GPIO_Port;
	hLCD.IO.RS.pin  = LCD_RS_Pin;
#if _USE_LCD_RW_
	hLCD.IO.RW.port = LCD_RW_GPIO_Port;
	hLCD.IO.RW.pin  = LCD_RW_Pin;
#endif /* _USE_LCD_RW_ */
	hLCD.IO.E.port = LCD_E_GPIO_Port;
	hLCD.IO.E.pin  = LCD_E_Pin;

#if _USE_4_BIT_MODE_
	/* only use DB4 ~ DB7 */
#else
	hLCD.IO.DB0.port = LCD_DB0_GPIO_Port;
	hLCD.IO.DB0.pin  = LCD_DB0_Pin;
	hLCD.IO.DB1.port = LCD_DB1_GPIO_Port;
	hLCD.IO.DB1.pin  = LCD_DB1_Pin;
	hLCD.IO.DB2.port = LCD_DB2_GPIO_Port;
	hLCD.IO.DB2.pin  = LCD_DB2_Pin;
	hLCD.IO.DB3.port = LCD_DB3_GPIO_Port;
	hLCD.IO.DB3.pin  = LCD_DB3_Pin;
#endif /* _USE_4_BIT_MODE_ */
	hLCD.IO.DB4.port = LCD_DB4_GPIO_Port;
	hLCD.IO.DB4.pin  = LCD_DB4_Pin;
	hLCD.IO.DB5.port = LCD_DB5_GPIO_Port;
	hLCD.IO.DB5.pin  = LCD_DB5_Pin;
	hLCD.IO.DB6.port = LCD_DB6_GPIO_Port;
	hLCD.IO.DB6.pin  = LCD_DB6_Pin;
	hLCD.IO.DB7.port = LCD_DB7_GPIO_Port;
	hLCD.IO.DB7.pin  = LCD_DB7_Pin;


	hLCD.errorCode = eLCD_ERROR_NONE;


	HAL_Delay(POWER_ON_WAIT_TIME);

	/* 2-line display setting */
#if _USE_4_BIT_MODE_
	/* only use DB4 ~ DB7 */
	enable_4_bit_mode();

	function_set(0,1,0);
#else
	function_set(1,1,0);
#endif /* _USE_4_BIT_MODE_ */

	/* display on */
	display_on_off(1,1,1);

	/* clear the LCD */
	clear_display();

}



/*
 * @brief  write data on DDRAM
 * @param  xPosition   0x0 ~ 0xF
 * @param  yPosition   0 for the first Line, 1 for the second Line
 * @param  data	  the data to write on DDRAM
 * @param  size	  the size of the data, must be less than 0xF
 */
bool writeLCD(uint8_t xPosition, uint8_t yPosition, char *data, uint8_t size)
{
	bool ret = true;

	if(xPosition > X_POSITION_MAX)
	{
		hLCD.errorCode = eLCD_ERROR_X_POSITION_OUT_OF_RANGE;
		return false;
	}

	if(yPosition > 1)
	{
		hLCD.errorCode = eLCD_ERROR_Y_POSITION_OUT_OF_RANGE;
		return false;
	}

	if(size > (X_POSITION_MAX - xPosition))
	{
		hLCD.errorCode = eLCD_ERROR_DATA_SIZE_OUT_OF_RANGE;
		return false;
	}

	/* set the DDRAM address */
	if(yPosition == 1) // if it's the 2nd line to write
	{
		set_DDRAM_address(SECOND_LINE_OFFSET + xPosition);
	}
	else
	{
		set_DDRAM_address(xPosition);
	}

	/* write data to the DDRAM */
	for(uint8_t i = 0; i < size; i++)
	{
		if(i + xPosition > X_POSITION_MAX)
		{
			hLCD.errorCode = eLCD_ERROR_X_POSITION_OUT_OF_RANGE;
			break;
		}
		else write_data_to_RAM(data[i]);
	}

	return ret;
}



void clear_display()
{
	/* set the I/O pins */
#if _USE_4_BIT_MODE_
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 0);

	SET_OR_RESET_ENABLE(0);
	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB4, 1);
#else
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 0);
	writeGPIO(&hLCD.IO.DB3, 0);
	writeGPIO(&hLCD.IO.DB2, 0);
	writeGPIO(&hLCD.IO.DB1, 0);
	writeGPIO(&hLCD.IO.DB0, 1);
#endif /* _USE_4_BIT_MODE_ */

	SET_OR_RESET_ENABLE(0);

	usDelay(WAIT_TIME_CLEAR_DISPLAY);
}



/*
 * @brief  set DDRAM address to 0x00
 */
void return_home()
{
	/* set the I/O pins */
#if _USE_4_BIT_MODE_
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 0);

	SET_OR_RESET_ENABLE(0);
	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB5, 1);
#else
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 0);
	writeGPIO(&hLCD.IO.DB3, 0);
	writeGPIO(&hLCD.IO.DB2, 0);
	writeGPIO(&hLCD.IO.DB1, 1);
	writeGPIO(&hLCD.IO.DB0, 0);
#endif /* _USE_4_BIT_MODE_ */

	SET_OR_RESET_ENABLE(0);

	usDelay(WAIT_TIME_RETURN_HOME);
}



/*
 * @param  I/D  1 for AC increment, 0 for AC decrement
 * @param  S    1 for screen shifting,  0 for no screen shifting
 */
void entry_mode_set(uint8_t ID, uint8_t S)
{
	/* set the I/O pins */
#if _USE_4_BIT_MODE_
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 0);

	SET_OR_RESET_ENABLE(0);
	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB6, 1);

	if(ID == 0)	writeGPIO(&hLCD.IO.DB5, 0);
	else		writeGPIO(&hLCD.IO.DB5, 1);

	if(S == 0)  writeGPIO(&hLCD.IO.DB4, 0);
	else 		writeGPIO(&hLCD.IO.DB4, 1);
#else
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 0);
	writeGPIO(&hLCD.IO.DB3, 0);
	writeGPIO(&hLCD.IO.DB2, 1);

	if(ID == 0)	writeGPIO(&hLCD.IO.DB1, 0);
	else		writeGPIO(&hLCD.IO.DB1, 1);

	if(S == 0)  writeGPIO(&hLCD.IO.DB0, 0);
	else 		writeGPIO(&hLCD.IO.DB0, 1);
#endif /* _USE_4_BIT_MODE_ */

	SET_OR_RESET_ENABLE(0);

	usDelay(WAIT_TIME_ENTRY_MODE_SET); // 37us according to the ST7066U datasheet
}



/*
 * @param  D   1 for display on, 0 for display off
 * @param  C   1 for cursor on,  0 for cursor off
 * @param  B   1 for cursor blinking on, 0 for cursor blinking off
 */
void display_on_off(uint8_t D, uint8_t C, uint8_t B)
{
	/* set the I/O pins */
#if _USE_4_BIT_MODE_
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 0);

	SET_OR_RESET_ENABLE(0);
	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 1);

	if(D == 0)  writeGPIO(&hLCD.IO.DB6, 0);
	else 		writeGPIO(&hLCD.IO.DB6, 1);

	if(C == 0) 	writeGPIO(&hLCD.IO.DB5, 0);
	else		writeGPIO(&hLCD.IO.DB5, 1);

	if(B == 0) 	writeGPIO(&hLCD.IO.DB4, 0);
	else 		writeGPIO(&hLCD.IO.DB4, 1);
#else
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 0);
	writeGPIO(&hLCD.IO.DB3, 1);

	if(D == 0)  writeGPIO(&hLCD.IO.DB2, 0);
	else 		writeGPIO(&hLCD.IO.DB2, 1);

	if(C == 0) 	writeGPIO(&hLCD.IO.DB1, 0);
	else		writeGPIO(&hLCD.IO.DB1, 1);

	if(B == 0) 	writeGPIO(&hLCD.IO.DB0, 0);
	else 		writeGPIO(&hLCD.IO.DB0, 1);
#endif /* _USE_4_BIT_MODE_ */

	SET_OR_RESET_ENABLE(0);

	usDelay(WAIT_TIME_DISPLAY_ON_OFF);
}



/*
 * @brief  move the cursor or shift the display
 * @param  S/C  1 for shift screen, 0 for shift cursor
 * @param  R/L  1 for shift to the right, 0 for shift to the left
 */
void cursor_or_display_shift(uint8_t SC, uint8_t RL)
{
	/* set the I/O pins */
#if _USE_4_BIT_MODE_
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 1);

	SET_OR_RESET_ENABLE(0);
	SET_OR_RESET_ENABLE(1);

	if(SC == 0) writeGPIO(&hLCD.IO.DB7, 0);
	else 		writeGPIO(&hLCD.IO.DB7, 1);

	if(RL == 0) writeGPIO(&hLCD.IO.DB6, 0);
	else 		writeGPIO(&hLCD.IO.DB6, 1);
#else
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 1);

	if(SC == 0) writeGPIO(&hLCD.IO.DB3, 0);
	else 		writeGPIO(&hLCD.IO.DB3, 1);

	if(RL == 0) writeGPIO(&hLCD.IO.DB2, 0);
	else 		writeGPIO(&hLCD.IO.DB2, 1);
#endif /* _USE_4_BIT_MODE_ */

	SET_OR_RESET_ENABLE(0);

	usDelay(WAIT_TIME_CURSOR_OR_DISPLAY_SHIFT);
}



/*
 * @param  DL  1 for 8-bit interface, 0 for 4-bit interface
 * @param  N   1 for 2-line display,  0 for 1-line display
 * @param  F   1 for 5x11 dots font,  0 for 5x8 dots font
 */
void function_set(uint8_t DL, uint8_t N, uint8_t F)
{
	/* set the I/O pins */
#if _USE_4_BIT_MODE_
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 1);
	writeGPIO(&hLCD.IO.DB4, 0);

	SET_OR_RESET_ENABLE(0);
	SET_OR_RESET_ENABLE(1);

	if(N == 0) 	writeGPIO(&hLCD.IO.DB7, 0);
	else 		writeGPIO(&hLCD.IO.DB7, 1);

	if(F == 0) 	writeGPIO(&hLCD.IO.DB6, 0);
	else 		writeGPIO(&hLCD.IO.DB6, 1);

#else
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 1);
	writeGPIO(&hLCD.IO.DB4, 1);

	if(N == 0) 	writeGPIO(&hLCD.IO.DB3, 0);
	else 		writeGPIO(&hLCD.IO.DB3, 1);

	if(F == 0) 	writeGPIO(&hLCD.IO.DB2, 0);
	else 		writeGPIO(&hLCD.IO.DB2, 1);
#endif /* _USE_4_BIT_MODE_ */

	SET_OR_RESET_ENABLE(0);

	usDelay(WAIT_TIME_FUNCTION_SET);
}



void set_DDRAM_address(uint8_t addr)
{
	/* set the I/O pins */
#if _USE_4_BIT_MODE_
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 1);

	writeGPIO(&hLCD.IO.DB6, addr & (1<<6));
	writeGPIO(&hLCD.IO.DB5, addr & (1<<5));
	writeGPIO(&hLCD.IO.DB4, addr & (1<<4));

	SET_OR_RESET_ENABLE(0);
	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, addr & (1<<3));
	writeGPIO(&hLCD.IO.DB6, addr & (1<<2));
	writeGPIO(&hLCD.IO.DB5, addr & (1<<1));
	writeGPIO(&hLCD.IO.DB4, addr & (1<<0));
#else
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 1);

	writeGPIO(&hLCD.IO.DB6, addr & (1<<6));
	writeGPIO(&hLCD.IO.DB5, addr & (1<<5));
	writeGPIO(&hLCD.IO.DB4, addr & (1<<4));
	writeGPIO(&hLCD.IO.DB3, addr & (1<<3));
	writeGPIO(&hLCD.IO.DB2, addr & (1<<2));
	writeGPIO(&hLCD.IO.DB1, addr & (1<<1));
	writeGPIO(&hLCD.IO.DB0, addr & (1<<0));
#endif /* _USE_4_BIT_MODE_ */

	SET_OR_RESET_ENABLE(0);

	usDelay(WAIT_TIME_SET_DDRAM_ADDRESS);
}



void write_data_to_RAM(uint8_t data)
{
	/* set the I/O pins */
#if _USE_4_BIT_MODE_
	writeGPIO(&hLCD.IO.RS, 1);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, data & (1<<7));
	writeGPIO(&hLCD.IO.DB6, data & (1<<6));
	writeGPIO(&hLCD.IO.DB5, data & (1<<5));
	writeGPIO(&hLCD.IO.DB4, data & (1<<4));

	SET_OR_RESET_ENABLE(0);
	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, data & (1<<3));
	writeGPIO(&hLCD.IO.DB6, data & (1<<2));
	writeGPIO(&hLCD.IO.DB5, data & (1<<1));
	writeGPIO(&hLCD.IO.DB4, data & (1<<0));
#else
	writeGPIO(&hLCD.IO.RS, 1);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, data & (1<<7));
	writeGPIO(&hLCD.IO.DB6, data & (1<<6));
	writeGPIO(&hLCD.IO.DB5, data & (1<<5));
	writeGPIO(&hLCD.IO.DB4, data & (1<<4));
	writeGPIO(&hLCD.IO.DB3, data & (1<<3));
	writeGPIO(&hLCD.IO.DB2, data & (1<<2));
	writeGPIO(&hLCD.IO.DB1, data & (1<<1));
	writeGPIO(&hLCD.IO.DB0, data & (1<<0));
#endif /* _USE_4_BIT_MODE_ */

	SET_OR_RESET_ENABLE(0);

	usDelay(WAIT_TIME_WRITE_DATA_TO_RAM);
}



#if _USE_4_BIT_MODE_
static void enable_4_bit_mode()
{
	/* set the I/O pins */
	writeGPIO(&hLCD.IO.RS, 0);

	SET_OR_RESET_ENABLE(1);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 1);
	writeGPIO(&hLCD.IO.DB4, 0);

	SET_OR_RESET_ENABLE(0);

	usDelay(WAIT_TIME_FUNCTION_SET);
}
#endif /* _USE_4_BIT_MODE_ */
