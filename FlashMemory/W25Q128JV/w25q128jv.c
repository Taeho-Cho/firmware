#define _W25Q128JV_C_
    #include "w25q128jv.h"
#undef  _W25Q128JV_C_

static void QUADSPI_Init(void);

static uint8_t W25Q128JV_Write_Enable();
static uint8_t W25Q128JV_Send(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
static uint8_t W25Q128JV_ReadID();
static uint8_t W25Q128JV_Reset();
static uint8_t W25Q128JV_Sector_Erase(uint32_t EraseAddr);
static uint8_t W25Q128JV_Block_Erase(uint32_t BlockAddr);
static uint8_t W25Q128JV_BusyFlagCheck(uint32_t Timeout);

static uint8_t CMD_PageProgram(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);

QSPI_HandleTypeDef hqspi;

uint8_t W25Q128JV_ManufacturerID;
uint8_t W25Q128JV_DeviceID;

uint8_t W25Q128JV_Init()
{
  QSPI_CommandTypeDef s_command;
  W25Q128JV_STATUS_REGISTER sr2;
 
  QUADSPI_Init();
  
  sr2.Register_8bit = 0;
  sr2.bit1 = 1;
  
  /* Write Enable */
  if (W25Q128JV_Write_Enable() != HAL_OK) 
  {  
    return HAL_ERROR; 
  }
  
  /* Command Setting */  
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = CMD_WRITE_SR2;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }
  
  /* Transfer Data */
  if (HAL_QSPI_Transmit(&hqspi, &sr2.Register_8bit, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }

/******************************************************************************/
  
  QSPI_AutoPollingTypeDef s_config;
  
  /* Polling Setting */
  s_config.Match           = sr2.Register_8bit;
  s_config.Mask            = sr2.Register_8bit;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
  
  s_command.Instruction    = CMD_READ_SR2;
  s_command.DataMode       = QSPI_DATA_1_LINE;
  s_command.NbData         = 1;
  
  if(HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  { 
    return HAL_ERROR;
  }

/*
  if(W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  {
    return HAL_ERROR;
  }
*/
  
  return HAL_OK;
}


uint8_t W25Q128JV_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
  QSPI_CommandTypeDef s_command;
  
  /* Command Setting */
  s_command.Instruction       = CMD_FAST_READ_QUAD_OUTPUT;
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
  s_command.Address           = ReadAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = 8;
  s_command.NbData            = Size;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
  W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT);
  
  if (HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }
  
  /* Receive Data */
  if(HAL_QSPI_Receive(&hqspi, pData, W25Q128JV_QSPI_TIMEOUT)!= HAL_OK) 
  {
    return HAL_ERROR;
  }
  
  if(W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  {
    return HAL_ERROR;
  }
  
  return HAL_OK;
}


uint8_t W25Q128JV_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
  uint8_t TimeOutCnt = 0;
  uint8_t * temp_data;
  temp_data = (uint8_t *)calloc(Size, 1);
  
  /* repeat Writing until completion */
  do{
    W25Q128JV_Send(pData,     WriteAddr, Size);
    W25Q128JV_Read(temp_data, WriteAddr, Size);
    
    if(TimeOutCnt++ > W25Q128JV_QSPI_COUNT_LIMIT) 
    {
      free(temp_data);
      return 0;
    } 
    
  }while( memcmp(pData, temp_data, Size) != 0 );
  
/*
  for(TimeOutCnt = 0; (memcmp(pData, temp_data, Size)!=0) && (TimeOutCnt<W25Q128JV_QSPI_COUNT_LIMIT); TimeOutCnt++)
  {
    W25Q128JV_Send(pData,     WriteAddr, Size);
    W25Q128JV_Read(temp_data, WriteAddr, Size);
  }
*/
  
  free(temp_data);
  return 1;
}


/** problem here **/
uint8_t W25Q128JV_Send(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
  
  QSPI_CommandTypeDef s_command;
  
  /* Erase Sector */
//  if(W25Q128JV_Sector_Erase(WriteAddr) != HAL_OK) 
//  {
//    return HAL_ERROR;
//  }
  for(uint8_t i=0; (W25Q128JV_Sector_Erase(WriteAddr) != HAL_OK) && (i<W25Q128JV_QSPI_COUNT_LIMIT) ; i++);
  
  /* command setting */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = CMD_QUAD_INPUT_PAGE_PROGRAM;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.NbData            = Size;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
  HAL_Delay(500);
  
  if (W25Q128JV_Write_Enable() != HAL_OK) 
  {
    asm("nop");
  }
  
  if(HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  { 
    asm("nop");
  }
  
  /* Transfer Data */
  if(HAL_QSPI_Transmit(&hqspi, pData, W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  { 
    asm("nop");
  }
  
  W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT);
  
  return HAL_OK;
}


uint8_t W25Q128JV_Write_Enable()
{
  
  QSPI_CommandTypeDef     s_command;
  
  /* command setting */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = CMD_WRITE_ENABLE;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if(HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  {  
    return HAL_ERROR; 
  }
  
/******************************************************************************/  
  
  QSPI_AutoPollingTypeDef s_config;
  W25Q128JV_STATUS_REGISTER sr1;
  
  /* check the Write Enable bit */
  sr1.Register_8bit   = 0;
  sr1.bit1            = 1;
  
  s_config.Match           = sr1.Register_8bit;
  s_config.Mask            = sr1.Register_8bit;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
  
  
  s_command.Instruction    = CMD_READ_SR1;
  s_command.DataMode       = QSPI_DATA_1_LINE;
  s_command.NbData         = 1;
  
  
  if(HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  { 
    return HAL_ERROR;
  }
  
//  if(W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
//  {
//    return HAL_ERROR;
//  }
  
  return HAL_OK;
}


uint8_t W25Q128JV_Reset()
{
  QSPI_CommandTypeDef s_command;
    
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = CMD_ENABLE_RESET;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
  if (HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  } 
  
  s_command.Instruction       = CMD_RESET_DEVICE;
 
  if (HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }
  
  return HAL_OK;
}


uint8_t W25Q128JV_ReadID()
{
  QSPI_CommandTypeDef s_command;
  uint8_t pData[2] = {0,};
  
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = CMD_READ_ID_CMD; 
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
  s_command.Address           = 0x000000;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 2;  
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
  
  if(HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  {
    return HAL_ERROR;
  }
  
  if(HAL_QSPI_Receive(&hqspi, pData, W25Q128JV_QSPI_TIMEOUT)!= HAL_OK) 
  {
    return HAL_ERROR;
  }
  
  if(W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  {
    return HAL_ERROR;
  }
  
  W25Q128JV_ManufacturerID  = pData[0];
  W25Q128JV_DeviceID        = pData[1];
  
  return HAL_OK;
}


uint8_t W25Q128JV_Sector_Erase(uint32_t EraseAddr)
{  
  QSPI_CommandTypeDef s_command;
  
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = CMD_ERASE_SECTOR; 
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
  s_command.Address           = EraseAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.NbData            = 0;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
//  do{
  
    if (W25Q128JV_Write_Enable() != HAL_OK) 
    {  
//      asm("nop");
      return HAL_ERROR; 
    }
    
    if (HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
    {
//      asm("nop");
      return HAL_ERROR;
    }
    
//  }while(HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK);
  

  if(W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  {
    return HAL_ERROR;
  }
  
  return HAL_OK;
}


static uint8_t W25Q128JV_BusyFlagCheck(uint32_t Timeout)
{
  
  QSPI_CommandTypeDef     s_command;
  QSPI_AutoPollingTypeDef s_config;
  W25Q128JV_STATUS_REGISTER sr1;
  
  /* command setting */
  s_command.Instruction       = CMD_READ_SR1;
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.NbData            = 1;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
  sr1.Register_8bit = 0;
  sr1.bit0          = 1;
  
  s_config.Match           = 0x00;
  s_config.Mask            = sr1.Register_8bit;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
  
  /* Busy bit check */
//  while(HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, W25Q128JV_QSPI_TIMEOUT) != HAL_OK); 
  for(uint8_t i=0; (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, W25Q128JV_QSPI_TIMEOUT) != HAL_OK) && (i<W25Q128JV_QSPI_COUNT_LIMIT) ; i++);
  
  return HAL_OK;
}


static uint8_t W25Q128JV_Block_Erase(uint32_t BlockAddr)
{
  
  QSPI_CommandTypeDef s_command;

  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = CMD_ERASE_BLCOK64;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
  s_command.Address           = BlockAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
  if (W25Q128JV_Write_Enable() != HAL_OK) 
  {  
    return HAL_ERROR; 
  }
  
  if (HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }
  
  if(W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  {
    return HAL_ERROR;
  }
  
  return HAL_OK;
}


/**
  * @brief QUADSPI Initialization Function
  * @param None
  * @retval None
  */
static void QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  /* QUADSPI parameter configuration*/
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 32;
  hqspi.Init.FifoThreshold = 1;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi.Init.FlashSize = 23;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

uint8_t W25Q128JV_PageProgram(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
    uint8_t TimeOutCnt = 0;
    uint8_t* temp_data;
    temp_data = (uint8_t*)calloc(Size, 1);

    /* repeat Writing until completion */
    do {
        CMD_PageProgram(pData, WriteAddr, Size);
        W25Q128JV_ReadData(temp_data, WriteAddr, Size);

        if (TimeOutCnt++ > W25Q128JV_QSPI_COUNT_LIMIT)
        {
            free(temp_data);
            return 0;
        }

    } while (memcmp(pData, temp_data, Size) != 0);

    /*
      for(TimeOutCnt = 0; (memcmp(pData, temp_data, Size)!=0) && (TimeOutCnt<W25Q128JV_QSPI_COUNT_LIMIT); TimeOutCnt++)
      {
        W25Q128JV_Send(pData,     WriteAddr, Size);
        W25Q128JV_Read(temp_data, WriteAddr, Size);
      }
    */

    free(temp_data);
    return 1;
}

static uint8_t CMD_PageProgram(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{

    QSPI_CommandTypeDef s_command;

    for (uint8_t i = 0; (W25Q128JV_Sector_Erase(WriteAddr) != HAL_OK) && (i < W25Q128JV_QSPI_COUNT_LIMIT); i++);

    /* Initialization program command */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = CMD_PAGE_PROGRAM;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.NbData = Size;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;


    HAL_Delay(500);
    //    W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT);

    /* Enable write operation */
    if (W25Q128JV_Write_Enable() != HAL_OK)
    {
        asm("nop");
        //      return HAL_ERROR;
    }

    /* configuration command */
    if (HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
    {
        asm("nop");
        //      return HAL_ERROR; 
    }

    /* transfer data */
    if (HAL_QSPI_Transmit(&hqspi, pData, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
    {
        asm("nop");
        //    return HAL_ERROR; 
    }

    /* transfer data */
    //  }while(HAL_QSPI_Transmit(&hqspi, pData, W25Q128JV_QSPI_TIMEOUT) != HAL_OK);

  /* busy bit check */
    W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT);

    return HAL_OK;

}


uint8_t W25Q128JV_ReadData(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
    QSPI_CommandTypeDef s_command;

    /* Command Setting */
    s_command.Instruction = CMD_READ_DATA;
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.Address = ReadAddr;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = Size;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT);

    if (HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Receive Data */
    if (HAL_QSPI_Receive(&hqspi, pData, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}