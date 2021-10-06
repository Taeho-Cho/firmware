#ifndef _NEVADA_NANO_H_
#define _NEVADA_NANO_H_

#include "main.h"

#ifdef _NEVADA_NANO_C_
    #define NEVADA_NANO_EXT
#else 
    #define NEVADA_NANO_EXT extern
#endif

/*
    1. power on and wait 3 seconds for the sensor to boot
    2. read the register 0x41
    3. if Status is OK, start Continuous Measurement
    4. wait 2 seconds
    5. read the register 0x01 to get the Answer
    6. check the cycleCount
    7. repeat 4 ~ 6
*/

#define NevadaStatus_OK                         0x00
#define NevadaStatus_CRC_FAILED                 0x01
#define NevadaStatus_BAD_PARAM                  0x02
#define NevadaStatus_EXE_FAILED                 0x03
#define NevadaStatus_NO_MEM                     0x04
#define NevadaStatus_UNKNOWN_CMD                0x05
#define NevadaStatus_INCOMPLETE_CMD             0x07
#define NevadaStatus_HW_ERR_AO                  0x20
#define NevadaStatus_HW_ERR_VDD                 0x21
#define NevadaStatus_HW_ERR_VREF                0x22
#define NevadaStatus_HW_ENV_XCD_RANGE           0x23
#define NevadaStatus_HW_ENV_SNSR_MALFUNCTION    0x24
#define NevadaStatus_HW_ERR_MCU_FLASH           0x25
#define NevadaStatus_HW_SENSOR_INITIALIZATION   0x26
#define NevadaStatus_HW_SENSOR_NEGATIVE         0x30
#define NevadaStatus_HW_CONDENSE_COND           0x31
#define NevadaStatus_HW_SENSOR_MALFUNCTION      0x32

#define NevadaCmd_none                          0x00

#define NevadaCmd_ANSWER                        0x01
#define NevadaCmd_CONC                          0x03
#define NevadaCmd_ID                            0x04
#define NevadaCmd_ENG_DATA                      0x09
#define NevadaCmd_TEMP                          0x21
#define NevadaCmd_PRES                          0x22
#define NevadaCmd_REL_HUM                       0x23
#define NevadaCmd_ABS_HUM                       0x24
#define NevadaCmd_STATUS                        0x41
#define NevadaCmd_VERSION                       0x42
#define NevadaCmd_SENSOR_INFO                   0x43
#define NevadaCmd_MEAS                          0x61
#define NevadaCmd_SHUT_DOWN                     0x62


#define REPLY_BUFFER_SIZE                       256

#define CHECKSUM_START_VALUE                    0xFFFF


typedef __packed struct {
    uint16_t CmdID;
    uint16_t Length;
    uint16_t Reserved;
    uint16_t Checksum;
    uint8_t  Payload;
} Command_Request;

typedef __packed struct {
    uint8_t  CmdID;
    uint8_t  Status;
    uint16_t Length;
    uint16_t Checksum;
    uint8_t  Payload[28]; // 128?
} Command_Reply;

typedef __packed struct {
    uint32_t    CYCLE_COUNT;
    float       CONC;
    uint32_t    gas_ID;
    float       TEMP;
    float       PRESSURE;
    float       REL_HUMI;
    float       ABS_HUMI;
} Answer_Data;


NEVADA_NANO_EXT void nevada_init();

NEVADA_NANO_EXT uint8_t nevada_run();

NEVADA_NANO_EXT Answer_Data Get_Answer_Data();

#endif
