#define _PEGASUS_C_
    #include "Pegasus.h"
#undef _PEGASUS_C_

static void MX_USART1_UART_Init(void);

static void Pegasus_init();

static eLTE_STATE PegasusParser(uint8_t data);

extern Result_Data_Protocol result_data_protocol;

UART_HandleTypeDef huart1;

CIRCULAR_QUEUE cq;

sPEGASUS_DATA g_sPegasus_data;


void Pegasus_init()
{  
  MX_USART1_UART_Init();
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
  
  CQ_init(&cq);
  memset((uint8_t*)&g_sPegasus_data, 0, sizeof(sPEGASUS_DATA));
  
  HAL_UART_Transmit(&huart1, (uint8_t*)"AT$LGTRESET\r\n", strlen("AT$LGTRESET\r\n"), 2000);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}


uint8_t Pegasus_run(eLTE_CMD eLTE_cmd)
{
  static uint16_t st_u16TimeOutTick = 0;
  static eLTE_CMD st_eCmd           = eLTE_CMD_INIT;
  
  switch(st_eCmd)
  {
  case eLTE_CMD_INIT :
    Pegasus_init();
    st_eCmd = eLTE_CMD_WAIT;
    break;
    
  case eLTE_CMD_READY : 
    st_eCmd = eLTE_cmd;
    break;
  
  case eLTE_CMD_GET_TIME :
    HAL_UART_Transmit(&huart1, (uint8_t*)"AT$LGTTIME?\r\n", strlen("AT$LGTTIME?\r\n"), 2000);
    st_eCmd = eLTE_CMD_WAIT;
    break;
  
  case eLTE_CMD_GET_CNUM :
    HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CNUM\r\n", strlen("AT+CNUM\r\n"), 2000);
    st_eCmd = eLTE_CMD_WAIT;
    break;
    
  case eLTE_CMD_SEND_DATA :
    {
      uint8_t str[64] = {0,};
      uint16_t u16strlen = 0;
      u16strlen = sprintf((char*)str,"AT$LGTTCPOP=%u.%u.%u.%u,%u\r\n", IP_ADDRESS1, IP_ADDRESS2,
                                                                       IP_ADDRESS3, IP_ADDRESS4, PORT_NUMBER);
  
      HAL_UART_Transmit(&huart1, str, u16strlen, 5000);
      st_eCmd = eLTE_CMD_WAIT;
      break;
    }
    
  case eLTE_CMD_WAIT :
  default            : break;
  }
  
/******************************************************************************/
  
  if( 1 != CQ_is_empty(&cq) )
  {
    switch( PegasusParser( CQ_pop(&cq) ) ) 
    {
      
    case eLTE_STATE_NWCAUSE :
      if( '0' != g_sPegasus_data.nwcause) { Pegasus_init(); }
      else { st_eCmd = eLTE_CMD_READY; }
      break;
      
    case eLTE_STATE_TIME :
      st_u16TimeOutTick = 0;
      st_eCmd = eLTE_CMD_READY;
      return 1;
      break;
      
    case eLTE_STATE_CNUM :
      st_u16TimeOutTick = 0;
      st_eCmd = eLTE_CMD_READY;
      return 1;
      break;
      
    case eLTE_STATE_TCPOP :
      if('0' != g_sPegasus_data.tcpop) { Pegasus_init(); }
      else {
        uint8_t str[55] = {0,};
        
        sprintf((char*)str,"AT$LGTTCPWR=%d,", sizeof(Result_Data_Protocol));
        memcpy( &str[15], &result_data_protocol, sizeof(Result_Data_Protocol));
        str[52] = '\r';
        str[53] = '\n';
        
        HAL_UART_Transmit(&huart1, str, 54, 6000);
      }
      break;
    case eLTE_STATE_TCPWR : 
      {
        if('0' != g_sPegasus_data.tcpwr) { Pegasus_init(); }
        else 
        {
            HAL_UART_Transmit(&huart1, (uint8_t*)"AT$LGTTCPCL\r\n", strlen("AT$LGTTCPCL\r\n"), 2000);
        }
        break; 
      }
      
    case eLTE_STATE_TCPCL :
      if('0' != g_sPegasus_data.tcpcl) { Pegasus_init(); }
      else 
      {
        st_u16TimeOutTick = 0;
        st_eCmd = eLTE_CMD_READY;
        return 1;
      }
      
      break;
      
          
    case eLTE_STATE_NONE : 
    default              : break;
    
    }
      
  }
  
  if(PEGASUS_TIMEOUT_LIMIT <= st_u16TimeOutTick++) 
  {
    st_u16TimeOutTick = 0;
    Pegasus_init();
  }
  
  return 0;
}


  /*
   * brief : parse the Pegasus data into the sLTE global Object
   *
   * Param : data popped out of the Queue
   *
   * note : this will set the sLTE global Object here, but requires clearing sLTE.eState elsewhere
   */
static eLTE_STATE PegasusParser(uint8_t pData)
{
  static uint8_t st_u8ArrIndex                  =  0;
  static uint8_t st_u8BufArr[BUFFER_ARRAY_SIZE] = {0,};
  static ePARSING_PHASE st_eParsingPhase        = ePHASE_START_CHAR;
  static eLTE_STATE st_sLTE_state_temp          = eLTE_STATE_NONE;
  
  
  if( BUFFER_ARRAY_SIZE == st_u8ArrIndex + 1 ) // handle the exception 
  {
    st_u8ArrIndex      = 0;
    st_eParsingPhase   = ePHASE_START_CHAR;
    st_sLTE_state_temp = eLTE_STATE_NONE;
    memset(st_u8BufArr, 0, BUFFER_ARRAY_SIZE);
  }
  
/******************************************************************************/  
  
  if( ('$' == pData) )
  {
    st_u8ArrIndex = 0;
    st_u8BufArr[st_u8ArrIndex] = pData;
    st_sLTE_state_temp = eLTE_STATE_NONE;
    st_eParsingPhase = ePHASE_CMD_STR;
    return eLTE_STATE_NONE;
  }
  
    
  if( ('+' == pData) && (ePHASE_ANSWER_DATA != st_eParsingPhase) )
  {
    st_u8ArrIndex = 0;
    st_u8BufArr[st_u8ArrIndex] = pData;
    st_sLTE_state_temp = eLTE_STATE_NONE;
    st_eParsingPhase = ePHASE_CMD_STR;
    return eLTE_STATE_NONE;
  }
  
/******************************************************************************/
  
  if( '+' == st_u8BufArr[0] && (ePHASE_CMD_STR == st_eParsingPhase))
  {
    st_u8BufArr[++st_u8ArrIndex] = pData;
    
    if( ':' == st_u8BufArr[st_u8ArrIndex] )
    {
        if( ('C' == st_u8BufArr[1]) && ('N' == st_u8BufArr[2]) && ('U' == st_u8BufArr[3]) && ('M' == st_u8BufArr[4]) ) st_sLTE_state_temp = eLTE_STATE_CNUM;
        
        st_eParsingPhase = ePHASE_ANSWER_DATA;
        
        return eLTE_STATE_NONE;
    }
  }
  
  
  if( '$' == st_u8BufArr[0] && (ePHASE_CMD_STR == st_eParsingPhase))
  {
    st_u8BufArr[++st_u8ArrIndex] = pData;
    
    if( ':' == st_u8BufArr[st_u8ArrIndex] )
    {
        if( ('L' == st_u8BufArr[1]) && ('G' == st_u8BufArr[2]) && ('T' == st_u8BufArr[3]) && ('T' == st_u8BufArr[4]) && ('I' == st_u8BufArr[5]) && ('M' == st_u8BufArr[6]) && ('E' == st_u8BufArr[7]) ) st_sLTE_state_temp = eLTE_STATE_TIME;
        if( ('L' == st_u8BufArr[1]) && ('G' == st_u8BufArr[2]) && ('T' == st_u8BufArr[3]) && ('N' == st_u8BufArr[4]) && ('W' == st_u8BufArr[5]) && ('C' == st_u8BufArr[6]) && ('A' == st_u8BufArr[7]) && ('U' == st_u8BufArr[8]) && ('S' == st_u8BufArr[9]) && ('E' == st_u8BufArr[10]) ) st_sLTE_state_temp = eLTE_STATE_NWCAUSE;
        if( ('L' == st_u8BufArr[1]) && ('G' == st_u8BufArr[2]) && ('T' == st_u8BufArr[3]) && ('T' == st_u8BufArr[4]) && ('C' == st_u8BufArr[5]) && ('P' == st_u8BufArr[6]) && ('O' == st_u8BufArr[7]) && ('P' == st_u8BufArr[8]) ) st_sLTE_state_temp = eLTE_STATE_TCPOP;
        if( ('L' == st_u8BufArr[1]) && ('G' == st_u8BufArr[2]) && ('T' == st_u8BufArr[3]) && ('T' == st_u8BufArr[4]) && ('C' == st_u8BufArr[5]) && ('P' == st_u8BufArr[6]) && ('W' == st_u8BufArr[7]) && ('R' == st_u8BufArr[8]) ) st_sLTE_state_temp = eLTE_STATE_TCPWR;
        if( ('L' == st_u8BufArr[1]) && ('G' == st_u8BufArr[2]) && ('T' == st_u8BufArr[3]) && ('T' == st_u8BufArr[4]) && ('C' == st_u8BufArr[5]) && ('P' == st_u8BufArr[6]) && ('C' == st_u8BufArr[7]) && ('L' == st_u8BufArr[8]) ) st_sLTE_state_temp = eLTE_STATE_TCPCL;
        
        st_eParsingPhase = ePHASE_ANSWER_DATA;
        
        return eLTE_STATE_NONE;
    }
  }
  
/******************************************************************************/
  
  if( ('+' == st_u8BufArr[0]) && (eLTE_STATE_CNUM == st_sLTE_state_temp) && (ePHASE_ANSWER_DATA == st_eParsingPhase) )
  {
    st_u8BufArr[++st_u8ArrIndex] = pData;
    
    if( CNUM_LAST_DATA_INDEX <= st_u8ArrIndex )
    {
        for(uint8_t i=0; i<CNUM_NUM_OF_DATA; i++)
        {
            g_sPegasus_data.cnum[i] = st_u8BufArr[i + CNUM_FIRST_DATA_INDEX];
        }
        g_sPegasus_data.eState  = eLTE_STATE_CNUM;
        
        st_u8ArrIndex      = 0;
        st_eParsingPhase   = ePHASE_START_CHAR;
        st_sLTE_state_temp = eLTE_STATE_NONE;
        memset(st_u8BufArr, 0, BUFFER_ARRAY_SIZE);
        
        return eLTE_STATE_CNUM; // can be deleted
    }
  }
  
    
  if( ('$' == st_u8BufArr[0]) && (eLTE_STATE_TIME == st_sLTE_state_temp) && (ePHASE_ANSWER_DATA == st_eParsingPhase) )
  {
    st_u8BufArr[++st_u8ArrIndex] = pData;
    
    if( TIME_LAST_DATA_INDEX <= st_u8ArrIndex )
    {
        for(uint8_t i=0; i<TIME_NUM_OF_DATA; i++)
        {
            g_sPegasus_data.time[i] = st_u8BufArr[i + TIME_FIRST_DATA_INDEX];    
        }
        g_sPegasus_data.eState  = eLTE_STATE_TIME;
        
        st_u8ArrIndex      = 0;
        st_eParsingPhase   = ePHASE_START_CHAR;
        st_sLTE_state_temp = eLTE_STATE_NONE;
        memset(st_u8BufArr, 0, BUFFER_ARRAY_SIZE);
        
        return eLTE_STATE_TIME; // can be deleted
    }
  }
  
  
  if( ('$' == st_u8BufArr[0]) && (eLTE_STATE_NWCAUSE == st_sLTE_state_temp) && (ePHASE_ANSWER_DATA == st_eParsingPhase) )
  {
    g_sPegasus_data.nwcause = pData;
    g_sPegasus_data.eState  = eLTE_STATE_NWCAUSE;
    
    st_u8ArrIndex      = 0;
    st_eParsingPhase   = ePHASE_START_CHAR;
    st_sLTE_state_temp = eLTE_STATE_NONE;
    memset(st_u8BufArr, 0, BUFFER_ARRAY_SIZE);
    
    return eLTE_STATE_NWCAUSE; // can be deleted
  }
  
  
  if( ('$' == st_u8BufArr[0]) && (eLTE_STATE_TCPOP == st_sLTE_state_temp) && (ePHASE_ANSWER_DATA == st_eParsingPhase) )
  {
    g_sPegasus_data.tcpop = pData;
    g_sPegasus_data.eState  = eLTE_STATE_TCPOP;
    
    st_u8ArrIndex      = 0;
    st_eParsingPhase   = ePHASE_START_CHAR;
    st_sLTE_state_temp = eLTE_STATE_NONE;
    memset(st_u8BufArr, 0, BUFFER_ARRAY_SIZE);
    
    return eLTE_STATE_TCPOP; // can be deleted
  }
  
  
  if( ('$' == st_u8BufArr[0]) && (eLTE_STATE_TCPWR == st_sLTE_state_temp) && (ePHASE_ANSWER_DATA == st_eParsingPhase) )
  {
    g_sPegasus_data.tcpwr = pData;
    g_sPegasus_data.eState  = eLTE_STATE_TCPWR;
    
    st_u8ArrIndex      = 0;
    st_eParsingPhase   = ePHASE_START_CHAR;
    st_sLTE_state_temp = eLTE_STATE_NONE;
    memset(st_u8BufArr, 0, BUFFER_ARRAY_SIZE);
    
    return eLTE_STATE_TCPWR; // can be deleted
  }
  

  if( ('$' == st_u8BufArr[0]) && (eLTE_STATE_TCPCL == st_sLTE_state_temp) && (ePHASE_ANSWER_DATA == st_eParsingPhase) )
  {
    g_sPegasus_data.tcpcl = pData;
    g_sPegasus_data.eState  = eLTE_STATE_TCPCL;
    
    st_u8ArrIndex      = 0;
    st_eParsingPhase   = ePHASE_START_CHAR;
    st_sLTE_state_temp = eLTE_STATE_NONE;
    memset(st_u8BufArr, 0, BUFFER_ARRAY_SIZE);
    
    return eLTE_STATE_TCPCL; // can be deleted
  }  
    
  return eLTE_STATE_NONE;
}


void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  //HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  
  /* Overrun Error: when the ORE bit is set, clear by writing 1 to the ORECF */
    if( (huart1.Instance->ISR & USART_ISR_ORE) != 0U )
    {
        huart1.Instance->ICR |= USART_ICR_ORECF;
    }


    if( (huart1.Instance->ISR & UART_FLAG_RXNE) != 0U ) 
    {
        CQ_push(&cq, (uint8_t)huart1.Instance->RDR);
    }
    
  /* USER CODE END USART1_IRQn 1 */
}


/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
