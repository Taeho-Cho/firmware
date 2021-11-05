/*
 * NevadaNano.h
 *
 *  Created on: Oct 8, 2021
 *      Author: chotaeho
 *
 *
 *  the measurement steps
 *  1. After powering on the sensor, wait 3 seconds for the sensor to boot
 *  2. Read MPS Status (Read Register: 0x41)
 *  3. if the Status is OK, Read MPS Version Info (Read Register: 0x42)
 *  4. Start measurement in continuous mode (Write Register: 0x61, Value: 0x2)
 *  5. Wait 2 seconds and Get Answer (Read Register: 0x01)
 *  6. repeat
 *
 *  Note
 *  1. the sensor initializes for the first 10 cycles after powering up.
 *	2. Little Endian format (LSB first)
 *
 */

#ifndef DEVICE_NEVADANANO_NEVADANANO_H_
#define DEVICE_NEVADANANO_NEVADANANO_H_

#include "main.h"


//#define READ_MPS_INFO

#define RESERVED				0x0

#define RX_BUFFERFER_SIZE 		34

#define CRC_START_VALUE			0xFFFF


typedef enum {
	eSTATUS_OK 							= 0x00,
	eSTATUS_CRC_FAILED 					= 0x01,
	eSTATUS_BAD_PARAM 					= 0x02,
	eSTATUS_EXE_FAILED 					= 0x03,
	eSTATUS_NO_MEM 						= 0x04,
	eSTATUS_UNKNOWN_CMD 				= 0x05,

	eSTATUS_INCOMPLETE_COMMAND 			= 0x07,

	eSTATUS_HW_ERR_AO 					= 0x20,
	eSTATUS_HW_ERR_VDD 					= 0x21,
	eSTATUS_HW_ERR_VREF 				= 0x22,
	eSTATUS_HW_ENV_XCD_RANGE 			= 0x23,
	eSTATUS_HW_ENV_SNSR_MALFUNCTION 	= 0x24,
	eSTATUS_HW_ERR_MCU_FLASH 			= 0x25,
	eSTATUS_SENSOR_INITIALIZATION 		= 0x26,

	eSTATUS_SENSOR_NEGATIVE 			= 0x30,
	eSTATUS_CONDENSE_COND 				= 0x31,
	eSTATUS_HW_SENSOR_MALFUNCTION 		= 0x32,

	eSTATUS_BREATH_OR_HUMIDITY_SURGE 	= 0x35
} eSTATUS_t;


typedef enum {
	eCOMMAND_NONE 			= 0x00,
	eCOMMAND_ANSWER 		= 0x01,

	eCOMMAND_CONC 			= 0x03,
	eCOMMAND_ID 			= 0x04,

	eCOMMAND_TEMP 			= 0x21,
	eCOMMAND_PRES 			= 0x22,
	eCOMMAND_REL_HUM 		= 0x23,
	eCOMMAND_ABS_HUM 		= 0x24,

	eCOMMAND_STATUS 		= 0x41,
	eCOMMAND_VERSION 		= 0x42,
	eCOMMAND_SENSOR_INFO 	= 0x43,

	eCOMMAND_MEAS 			= 0x61,
	eCOMMAND_SHUT_DOWN 		= 0x62
} eCOMMAND_t;


typedef enum {
	eGAS_ID_No_Gas 				= 0,
	eGAS_ID_Hydrogen 			= 1,
	eGAS_ID_Hydrogen_Mixture 	= 2,
	eGAS_ID_Methane 			= 3,
	eGAS_ID_Light_Gas 			= 4,
	eGAS_ID_Medium_Gas 			= 5,
	eGAS_ID_Heavy_Gas 			= 6,

	eGAS_ID_Unknown_Gas 		= 253,
	eGAS_ID_Under_Range 		= 254,
	eGAS_ID_Over_Range 			= 255
} eGAS_ID_t;


typedef struct __attribute__((packed)) {
	uint16_t CmdID;
	uint16_t Length;
	uint16_t Reserved;
	uint16_t Checksum;
} sREQUEST_PACKET_HEADER_t;

typedef union {
	struct
	{
		uint8_t   measurement_mode :4; 	/*!< This parameter can be a value of @ref MEASUREMENT_MODE */

		uint8_t concentration_unit :4; 	/*!< This parameter can be a value of @ref CONCENTRATION_UNIT */
	};
	uint8_t value;
} uMEASUREMENT_t;


typedef struct __attribute__((packed)) {
	uint8_t  CmdID;
	uint8_t  Status;
	uint16_t Length;
	uint16_t Checksum;
} sREPLY_PACKET_HEADER_t;

typedef struct __attribute__((packed)) {
	uint32_t CYCLE_COUNT;
	float 	 CONC;
	uint32_t gas_ID;
	float 	 TEMP;
	float 	 PRESSURE;
	float 	 REL_HUMI;
	float 	 ABS_HUMI;
} sANSWER_RESULT_t;


typedef struct __attribute__((packed)) {
	sREQUEST_PACKET_HEADER_t PacketHeader;
	uMEASUREMENT_t measurement;
} sREQUEST_PACKET_t;

typedef struct __attribute__((packed)) {
	sREPLY_PACKET_HEADER_t PacketHeader;
	union{
	sANSWER_RESULT_t AnswerResult;
	uint8_t	StatusPayload;
	};
} sREPLY_PACKET_t;


typedef struct {
	eCOMMAND_t		Comm; 	/*!< from the request packet */
	eSTATUS_t 		Stat; 	/*!< from the   reply packet */

	sREQUEST_PACKET_t RequestPacket;
	sREPLY_PACKET_t   ReplyPacket;

} sNEVADANANO_HANDLER_t;


#ifdef READ_MPS_INFO
typedef struct __attribute__((packed)) {
	uint8_t SW_Version_W;
	uint8_t SW_Version_X;
	uint8_t SW_Version_Y;
	uint8_t SW_Version_Z;
	uint8_t HW_Version_W;
	uint8_t HW_Version_X;
	uint8_t Protocol_Version_W;
	uint8_t Protocol_Version_X;
} sVERSION_t;

typedef struct __attribute__((packed)) {
	char SERIAL_NUM[32];
	uint32_t SENSOR_TYPE; 	// MPS Flammable Gas Sensor type = 0x03
	char CAL_DATE[16]; 		// Date of last calibration, MM/DD/YYYY
	char MFG_DATE[16]; 		// Date of manufacture,      MM/DD/YYYY
} sSENSOR_INFO_t;
#endif


/*
 * @defgroup  MEASUREMENT_MODE
 * @{
 */
#define MEASUREMENT_MODE_CONT 	0x2U 	/*!< continuous mode  */
#define MEASUREMENT_MODE_STOP 	0x3U 	/*!< stop measurement */
/*
 * @}
 */


/*
 * @defgroup  CONCENTRATION_UNIT
 * @{
 */
#define CONCENTRATION_UNIT_ISO 	0x0U 	/*!< ISO 10156  	*/
#define CONCENTRATION_UNIT_IEC 	0x2U 	/*!< IEC 60079-20-1 */
/*
 * @}
 */


/*
 * @defgroup PAYLOAD_LENGTH
 * @{
 */
#define PAYLOAD_LENGTH_ZERO  0

#define PAYLOAD_LENGTH_ANSWER_REQUEST  0
#define PAYLOAD_LENGTH_ANSWER_REPLY    28U

#define PAYLOAD_LENGTH_CONC_REQUEST  0
#define PAYLOAD_LENGTH_CONC_REPLY    4U

#define PAYLOAD_LENGTH_ID_REQUEST  0
#define PAYLOAD_LENGTH_ID_REPLY    4U

#define PAYLOAD_LENGTH_TEMP_REQUEST  0
#define PAYLOAD_LENGTH_TEMP_REPLY    4U

#define PAYLOAD_LENGTH_PRES_REQUEST  0
#define PAYLOAD_LENGTH_PRES_REPLY    4U

#define PAYLOAD_LENGTH_REL_HUM_REQUEST  0
#define PAYLOAD_LENGTH_REL_HUM_REPLY    4U

#define PAYLOAD_LENGTH_ABS_HUM_REQUEST  0
#define PAYLOAD_LENGTH_ABS_HUM_REPLY    4U

#define PAYLOAD_LENGTH_STATUS_REQUEST  0
#define PAYLOAD_LENGTH_STATUS_REPLY    1U

#define PAYLOAD_LENGTH_VERSION_REQUEST  0
#define PAYLOAD_LENGTH_VERSION_REPLY    8U

#define PAYLOAD_LENGTH_SENSOR_INFO_REQUEST  0
#define PAYLOAD_LENGTH_SENSOR_INFO_REPLY    68U

#define PAYLOAD_LENGTH_MEAS_REQUEST  1U
#define PAYLOAD_LENGTH_MEAS_REPLY    0

#define PAYLOAD_LENGTH_SHUT_DOWN_REQUEST  0
#define PAYLOAD_LENGTH_SHUT_DOWN_REPLY    0
/*
 * @}
 */


bool initSensor(void);

bool startMeasurement(void);

bool getAnswer(void);

#endif /* DEVICE_NEVADANANO_NEVADANANO_H_ */
