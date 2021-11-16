/*
 * NevadaNano.c
 *
 *  Created on: Oct 8, 2021
 *      Author: chotaeho
 */

#include "NevadaNano.h"
#include "UART_wrapper.h"


/* @defgroup  NevadaNano macros
 * @{
 */
#define RESERVED           0x0U
#define RX_BUFFERFER_SIZE  34U
#define CRC_START_VALUE	   0xFFFF
#define LOOP_LIMIT 	       20000U
/*
 * @}
 */


/* @defgroup NevadaNano static symbols
 * @{
 */
static bool sendPacket(eCOMMAND_t);
static bool receivePacket(eCOMMAND_t);

/* @defgroup inner functions
 * @{
 */
static bool setRequestPacket(eCOMMAND_t);
static bool setReplyPacket(eCOMMAND_t);
static uint16_t crc_generate(uint8_t *buffer, size_t length, uint16_t startValue);
static bool checkCRC(uint8_t *, eCOMMAND_t);
/*
 * @}
 */

static sNEVADANANO_HANDLER_t sNevadaNanoHandler;
static uint8_t RX_BUFFER[RX_BUFFERFER_SIZE];
/*
 * @}
 */


bool initSensor(void)
{
	uint32_t index = 0;

	HAL_Delay(3000);

	UART_init(eUART_CHANNEL_4, 38400);

	sendPacket(eCOMMAND_STATUS);

	for(index = 0; index <= LOOP_LIMIT; index++)
	{
		if(receivePacket(eCOMMAND_STATUS) == true) break;
	}

	if(index == LOOP_LIMIT) sNevadaNanoHandler.Error = eERROR_TIMEOUT;

	if(sNevadaNanoHandler.Error != eERROR_NONE) return false;

	return true;
}


bool startMeasurement(void)
{
	uint32_t index = 0;

	sendPacket(eCOMMAND_MEAS);

	for(index = 0; index <= LOOP_LIMIT; index++)
	{
		if(receivePacket(eCOMMAND_MEAS) == true) break;
	}

	if(index == LOOP_LIMIT) sNevadaNanoHandler.Error = eERROR_TIMEOUT;

	if(sNevadaNanoHandler.Error != eERROR_NONE) return false;

	HAL_Delay(1000);

	return true;
}


bool getAnswer(void)
{
	uint32_t index = 0;

	HAL_Delay(1000);

	sendPacket(eCOMMAND_ANSWER);

	for(index = 0; index <= LOOP_LIMIT; index++)
	{
		if(receivePacket(eCOMMAND_ANSWER) == true) break;
	}

	if(index == LOOP_LIMIT) sNevadaNanoHandler.Error = eERROR_TIMEOUT;

	if(sNevadaNanoHandler.Error != eERROR_NONE) return false;

	return true;
}


static bool sendPacket(eCOMMAND_t comm)
{
	uint8_t size = 0;

	setRequestPacket(comm);

	switch(comm)
	{
	case eCOMMAND_STATUS : { size = sizeof(sREQUEST_PACKET_HEADER_t) + PAYLOAD_LENGTH_STATUS_REQUEST; } break;
	case eCOMMAND_MEAS   : { size = sizeof(sREQUEST_PACKET_HEADER_t) + PAYLOAD_LENGTH_MEAS_REQUEST;   } break;
	case eCOMMAND_ANSWER : { size = sizeof(sREQUEST_PACKET_HEADER_t) + PAYLOAD_LENGTH_ANSWER_REQUEST; } break;
	default : return false;
	}

	if(UART_send(eUART_CHANNEL_4, (uint8_t*)&sNevadaNanoHandler.RequestPacket, size) != true) return false;

	return true;
}


/* @brief   check if the reply packet has been received
 * @retval  return true if the reply packet is received
 */
static bool receivePacket(eCOMMAND_t comm)
{
	uint8_t size = 0;

	switch (comm) {
	case eCOMMAND_STATUS : { size = sizeof(sREPLY_PACKET_HEADER_t) + PAYLOAD_LENGTH_STATUS_REPLY; } break;
	case eCOMMAND_MEAS   : { size = sizeof(sREPLY_PACKET_HEADER_t) + PAYLOAD_LENGTH_MEAS_REPLY;   } break;
	case eCOMMAND_ANSWER : { size = sizeof(sREPLY_PACKET_HEADER_t) + PAYLOAD_LENGTH_ANSWER_REPLY; } break;
	default : return false;
	}

	if(UART_read(eUART_CHANNEL_4, RX_BUFFER, size) != true) return false;

	if (setReplyPacket(comm) != true) sNevadaNanoHandler.Error = eERROR_INVALID_REPLY;

	return true;
}


/* @defgroup  inner functions
 * @{
 */
static bool setRequestPacket(eCOMMAND_t comm)
{
	sNevadaNanoHandler.Comm = comm;

	memset(&sNevadaNanoHandler.RequestPacket, 0, sizeof(sREPLY_PACKET_t));

	switch (comm) {
	case eCOMMAND_MEAS : {
		sNevadaNanoHandler.RequestPacket.PacketHeader.CmdID  = eCOMMAND_MEAS;

		/* set the Request Payload byte to 0x2 to start Measurement in continuous mode */
		sNevadaNanoHandler.RequestPacket.PacketHeader.Length = PAYLOAD_LENGTH_MEAS_REQUEST;
		sNevadaNanoHandler.RequestPacket.measurement.measurement_mode = MEASUREMENT_MODE_CONT;
	} break;

	case eCOMMAND_STATUS :
	case eCOMMAND_ANSWER : {
		sNevadaNanoHandler.RequestPacket.PacketHeader.CmdID  = comm;
		sNevadaNanoHandler.RequestPacket.PacketHeader.Length = PAYLOAD_LENGTH_ZERO;
	} break;

	default : return false;
	}


	sNevadaNanoHandler.RequestPacket.PacketHeader.Reserved = RESERVED;
	sNevadaNanoHandler.RequestPacket.PacketHeader.Checksum = crc_generate( (uint8_t *) &sNevadaNanoHandler.RequestPacket,
																			sizeof(sREQUEST_PACKET_HEADER_t) + sNevadaNanoHandler.RequestPacket.PacketHeader.Length,
																			CRC_START_VALUE );

	return true;
}


static bool setReplyPacket(eCOMMAND_t comm)
{
	sREPLY_PACKET_t tempReplyPacket = {0, };

	switch(comm)
	{
	case eCOMMAND_STATUS : {
		memcpy( &tempReplyPacket, RX_BUFFER, sizeof(sREPLY_PACKET_HEADER_t) + PAYLOAD_LENGTH_STATUS_REPLY );
	} break;

	case eCOMMAND_MEAS : {
		memcpy( &tempReplyPacket, RX_BUFFER, sizeof(sREPLY_PACKET_HEADER_t) + PAYLOAD_LENGTH_MEAS_REPLY );
	} break;

	case eCOMMAND_ANSWER : {
		memcpy( &tempReplyPacket, RX_BUFFER, sizeof(sREPLY_PACKET_HEADER_t) + PAYLOAD_LENGTH_ANSWER_REPLY );
	} break;

	default : return false;
	}


	if( checkCRC( (uint8_t*)&tempReplyPacket, sNevadaNanoHandler.Comm ) != true )
	{
		return false;
	}

	if( sNevadaNanoHandler.Comm != (eCOMMAND_t)tempReplyPacket.PacketHeader.CmdID )
	{
		return false;
	}

	if( ((eSTATUS_t)tempReplyPacket.PacketHeader.Status != eSTATUS_SENSOR_INITIALIZATION) &&
		((eSTATUS_t)tempReplyPacket.PacketHeader.Status != eSTATUS_OK))
	{
		return false;
	}

	sNevadaNanoHandler.Stat = (eSTATUS_t)tempReplyPacket.PacketHeader.Status;
	sNevadaNanoHandler.ReplyPacket = tempReplyPacket;

	return true;
}


bool checkCRC(uint8_t *buffer, eCOMMAND_t comm)
{
	sREPLY_PACKET_t * bufptr = (sREPLY_PACKET_t*)buffer;

	uint16_t receivedCRC = bufptr->PacketHeader.Checksum;
	uint16_t tempCRC 	 = 0;

	bufptr->PacketHeader.Checksum = 0;

	switch(sNevadaNanoHandler.Comm)
	{
	case eCOMMAND_STATUS : {
		tempCRC = crc_generate( (uint8_t*)bufptr,
								sizeof(sREPLY_PACKET_HEADER_t) + PAYLOAD_LENGTH_STATUS_REPLY,
								CRC_START_VALUE );
	} break;

	case eCOMMAND_MEAS : {
		tempCRC = crc_generate( (uint8_t*)bufptr,
								sizeof(sREPLY_PACKET_HEADER_t) + PAYLOAD_LENGTH_MEAS_REPLY,
								CRC_START_VALUE );
	} break;

	case eCOMMAND_ANSWER : {
		tempCRC = crc_generate( (uint8_t*)bufptr,
								sizeof(sREPLY_PACKET_HEADER_t) + PAYLOAD_LENGTH_ANSWER_REPLY,
								CRC_START_VALUE );
	} break;

	default : break;
	}

	if(receivedCRC != tempCRC) return false;
	else bufptr->PacketHeader.Checksum = receivedCRC;

	return true;
}


uint16_t crc_generate(uint8_t *buffer, size_t length, uint16_t startValue)
{
  static uint16_t crc_table[256] = {
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
  0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
  0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
  0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
  0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
  0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
  0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
  0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
  0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
  0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
  0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
  0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
  0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
  0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
  0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
  0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
  0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
  0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
  0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
  0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
  0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
  0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
  0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,};

  uint16_t crc;
  uint8_t *p;
  int ii;
  crc = startValue;

  for(p = buffer, ii = 0; ii < length; ii++)
  {
    crc = (crc << 8) ^ crc_table[(crc >> 8) ^ *p];
    p++;
  }

  return crc;
}
/*
 * @}
 */
