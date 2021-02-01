#ifndef _PEGASUS_H_
#define _PEGASUS_H_

#include "main.h"

#ifdef _PEGASUS_C_
    #define PEGASUS_EXT 
#else
    #define PEGASUS_EXT extern
#endif

#define IP_ADDRESS1             ( )
#define IP_ADDRESS2             ( )
#define IP_ADDRESS3             ( )
#define IP_ADDRESS4             ( )
#define PORT_NUMBER             ( )

#define PEGASUS_TIMEOUT_LIMIT   (1200)

#define BUFFER_ARRAY_SIZE           (32)

#define CNUM_NUM_OF_DATA            (12)
#define CNUM_FIRST_DATA_INDEX       (15)
#define CNUM_LAST_DATA_INDEX        (26)

#define TIME_NUM_OF_DATA            (14)
#define TIME_FIRST_DATA_INDEX       (9)
#define TIME_LAST_DATA_INDEX        (22)


typedef enum {
    eLTE_STATE_NONE=0,
    eLTE_STATE_NWCAUSE,
    eLTE_STATE_TIME,
    eLTE_STATE_CNUM,
    eLTE_STATE_TCPOP,
    eLTE_STATE_TCPWR,
    eLTE_STATE_TCPCL,
} eLTE_STATE;

typedef struct {
    eLTE_STATE  eState;
    uint8_t     time[TIME_NUM_OF_DATA];
    uint8_t     cnum[CNUM_NUM_OF_DATA];
    uint8_t     nwcause;
    uint8_t     tcpop;
    uint8_t     tcpwr;
    uint8_t     tcpcl;
} sPEGASUS_DATA;

typedef enum {
    ePHASE_START_CHAR,
    ePHASE_CMD_STR,
    ePHASE_ANSWER_DATA,
} ePARSING_PHASE;

typedef enum {
    eLTE_CMD_INIT=0,
    eLTE_CMD_WAIT,
    eLTE_CMD_READY,
    eLTE_CMD_GET_TIME,
    eLTE_CMD_GET_CNUM,
    eLTE_CMD_SEND_DATA,
} eLTE_CMD;

PEGASUS_EXT uint8_t Pegasus_run(eLTE_CMD eLTE_cmd);

#endif

