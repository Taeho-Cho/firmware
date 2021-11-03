/*
 * character_lcd.c
 *
 *  Created on: Nov 1, 2021
 *      Author: chotaeho
 */
#include "character_lcd.h"
#include "usDelay.h"


static void DataEnable(); /* refer to the Timing Diagram in the datasheet */
static sLCD_HANDLER_t hLCD;


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

#else
	hLCD.IO.DB0.port = LCD_D0_GPIO_Port;
	hLCD.IO.DB0.pin  = LCD_D0_Pin;
	hLCD.IO.DB1.port = LCD_D1_GPIO_Port;
	hLCD.IO.DB1.pin  = LCD_D1_Pin;
	hLCD.IO.DB2.port = LCD_D2_GPIO_Port;
	hLCD.IO.DB2.pin  = LCD_D2_Pin;
	hLCD.IO.DB3.port = LCD_D3_GPIO_Port;
	hLCD.IO.DB3.pin  = LCD_D3_Pin;
#endif /* _USE_4_BIT_MODE_ */
	hLCD.IO.DB4.port = LCD_D4_GPIO_Port;
	hLCD.IO.DB4.pin  = LCD_D4_Pin;
	hLCD.IO.DB5.port = LCD_D5_GPIO_Port;
	hLCD.IO.DB5.pin  = LCD_D5_Pin;
	hLCD.IO.DB6.port = LCD_D6_GPIO_Port;
	hLCD.IO.DB6.pin  = LCD_D6_Pin;
	hLCD.IO.DB7.port = LCD_D7_GPIO_Port;
	hLCD.IO.DB7.pin  = LCD_D7_Pin;

	memset(hLCD.data.line1, 0, NUMBER_OF_CHAR_IN_LINE);
	memset(hLCD.data.line2, 0, NUMBER_OF_CHAR_IN_LINE);

	hLCD.errorCode = eLCD_ERROR_NONE;


	HAL_Delay(50);

	/* 2-line display setting */
	FunctionSet(1,1,0);

	/* display on */
	DisplayOnOff(1,1,1);

	/* clear the LCD */
	ClearDisplay();
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

	if(yPosition == 0)
	{
		memset(&hLCD.data.line1[xPosition], 0, NUMBER_OF_CHAR_IN_LINE - xPosition);
		memcpy(&hLCD.data.line1[xPosition], data, size);
	}
	else
	{
		memset(&hLCD.data.line2[xPosition], 0, NUMBER_OF_CHAR_IN_LINE - xPosition);
		memcpy(&hLCD.data.line2[xPosition], data, size);
	}

	ClearDisplay();
	for(uint8_t i = 0; i < NUMBER_OF_CHAR_IN_LINE; i++)
	{
		if(hLCD.data.line1[i] == 0) WriteDataToRAM(' ');
		else 						WriteDataToRAM(hLCD.data.line1[i]);
	}

	SetDDRAMaddress(SECOND_LINE_OFFSET);
	for(uint8_t i = 0; i < NUMBER_OF_CHAR_IN_LINE; i++)
	{
		if(hLCD.data.line2[i] == 0) WriteDataToRAM(' ');
		else 						WriteDataToRAM(hLCD.data.line2[i]);
	}

	/* set DDRAM address */
	//SetDDRAMaddress(xPosition + (yPosition * SECOND_LINE_OFFSET));

	/* write data to RAM */
	//for(uint8_t i = 0; i < size; i++)  WriteDataToRAM(data[i]);

	return ret;
}


void ClearDisplay()
{
	/* set the I/O pins */
	writeGPIO(&hLCD.IO.RS, 0);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 0);
	writeGPIO(&hLCD.IO.DB3, 0);
	writeGPIO(&hLCD.IO.DB2, 0);
	writeGPIO(&hLCD.IO.DB1, 0);
	writeGPIO(&hLCD.IO.DB0, 1);

	DataEnable();
}


/*
 * @brief  set DDRAM address to 0x00
 */
void ReturnHome()
{
	/* set the I/O pins */
	writeGPIO(&hLCD.IO.RS, 0);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 0);
	writeGPIO(&hLCD.IO.DB3, 0);
	writeGPIO(&hLCD.IO.DB2, 0);
	writeGPIO(&hLCD.IO.DB1, 1);
	writeGPIO(&hLCD.IO.DB0, 0);

	DataEnable();
}


/*
 * @param  I/D  1 for AC increment, 0 for AC decrement
 * @param  S    1 for screen shifting,  0 for no screen shifting
 */
void EntryModeSet(uint8_t ID, uint8_t S)
{
	/* set the I/O pins */
	writeGPIO(&hLCD.IO.RS, 0);

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

	DataEnable();
}


/*
 * @param  D   1 for display on, 0 for display off
 * @param  C   1 for cursor on,  0 for cursor off
 * @param  B   1 for cursor blinking on, 0 for cursor blinking off
 */
void DisplayOnOff(uint8_t D, uint8_t C, uint8_t B)
{
	/* set the I/O pins */
	writeGPIO(&hLCD.IO.RS, 0);

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

	DataEnable();
}


/*
 * @brief  move the cursor or shift the display
 * @param  S/C  1 for shift screen, 0 for shift cursor
 * @param  R/L  1 for shift to the right, 0 for shift to the left
 */
void CursorOrDisplayShift(uint8_t SC, uint8_t RL)
{
	/* set the I/O pins */
	writeGPIO(&hLCD.IO.RS, 0);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 0);
	writeGPIO(&hLCD.IO.DB4, 1);

	if(SC == 0) writeGPIO(&hLCD.IO.DB3, 0);
	else 		writeGPIO(&hLCD.IO.DB3, 1);

	if(RL == 0) writeGPIO(&hLCD.IO.DB2, 0);
	else 		writeGPIO(&hLCD.IO.DB2, 1);

	/* don't care */
	writeGPIO(&hLCD.IO.DB1, 0);
	writeGPIO(&hLCD.IO.DB0, 0);

	DataEnable();
}


/*
 * @param  DL  1 for 8-bit interface, 0 for 4-bit interface
 * @param  N   1 for 2-line display,  0 for 1-line display
 * @param  F   1 for 5x11 dots font,  0 for 5x8 dots font
 */
void FunctionSet(uint8_t DL, uint8_t N, uint8_t F)
{
	/* set the I/O pins */
	writeGPIO(&hLCD.IO.RS, 0);

	writeGPIO(&hLCD.IO.DB7, 0);
	writeGPIO(&hLCD.IO.DB6, 0);
	writeGPIO(&hLCD.IO.DB5, 1);

	if(DL == 0) writeGPIO(&hLCD.IO.DB4, 0);
	else 		writeGPIO(&hLCD.IO.DB4, 1);

	if(N == 0) 	writeGPIO(&hLCD.IO.DB3, 0);
	else 		writeGPIO(&hLCD.IO.DB3, 1);

	if(F == 0) 	writeGPIO(&hLCD.IO.DB2, 0);
	else 		writeGPIO(&hLCD.IO.DB2, 1);

	/* don't care */
	writeGPIO(&hLCD.IO.DB1, 0);
	writeGPIO(&hLCD.IO.DB0, 0);

	DataEnable();
}


void SetDDRAMaddress(uint8_t addr)
{
	/* set the I/O pins */
	writeGPIO(&hLCD.IO.RS, 0);

	writeGPIO(&hLCD.IO.DB7, 1);

	writeGPIO(&hLCD.IO.DB6, addr & (1<<6));
	writeGPIO(&hLCD.IO.DB5, addr & (1<<5));
	writeGPIO(&hLCD.IO.DB4, addr & (1<<4));
	writeGPIO(&hLCD.IO.DB3, addr & (1<<3));
	writeGPIO(&hLCD.IO.DB2, addr & (1<<2));
	writeGPIO(&hLCD.IO.DB1, addr & (1<<1));
	writeGPIO(&hLCD.IO.DB0, addr & (1<<0));

	DataEnable();
}


void WriteDataToRAM(uint8_t data)
{
	/* set the I/O pins */
	writeGPIO(&hLCD.IO.RS, 1);

	writeGPIO(&hLCD.IO.DB7, data & (1<<7));
	writeGPIO(&hLCD.IO.DB6, data & (1<<6));
	writeGPIO(&hLCD.IO.DB5, data & (1<<5));
	writeGPIO(&hLCD.IO.DB4, data & (1<<4));
	writeGPIO(&hLCD.IO.DB3, data & (1<<3));
	writeGPIO(&hLCD.IO.DB2, data & (1<<2));
	writeGPIO(&hLCD.IO.DB1, data & (1<<1));
	writeGPIO(&hLCD.IO.DB0, data & (1<<0));

	DataEnable();
}

void DataEnable()
{
	/* enable cycle */
	usDelay(US_DELAY_TIME);
	writeGPIO(&hLCD.IO.E, 1);
	usDelay(US_DELAY_TIME);
	writeGPIO(&hLCD.IO.E, 0);
	usDelay(US_DELAY_TIME);
}
