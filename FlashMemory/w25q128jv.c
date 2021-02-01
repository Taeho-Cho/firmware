#define _W25Q128JV_C_
    #include "w25q128jv.h"
#undef  _W25Q128JV_C_


static uint8_t W25Q128JV_Write_Enable();
static uint8_t W25Q128JV_Send(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
static uint8_t W25Q128JV_ReadID();
static uint8_t W25Q128JV_Reset();
static uint8_t W25Q128JV_Sector_Erase(uint32_t EraseAddr);
static uint8_t W25Q128JV_Block_Erase(uint32_t BlockAddr);
static uint8_t W25Q128JV_BusyFlagCheck(uint32_t Timeout);

extern QSPI_HandleTypeDef hqspi;

uint8_t W25Q128JV_ManufacturerID;
uint8_t W25Q128JV_DeviceID;

uint8_t W25Q128JV_Init()
{
  
  QSPI_CommandTypeDef s_command;
  W25Q128JV_STATUS_REGISTER sr2;
  
  sr2.Register_8bit = 0;
  sr2.bit1 = 1;
  
  /* Enable write operation */
  if (W25Q128JV_Write_Enable() != HAL_OK) 
  {  
    return HAL_ERROR; 
  }
  
  /* Sets the four-way enabled status register to enable four-channel IO2 and IO3 pins */
  
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

  
  /* configuration command */
  if (HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }
  
  /* transfer data */
  if (HAL_QSPI_Transmit(&hqspi, &sr2.Register_8bit, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }

/******************************************************************************/
  
  QSPI_AutoPollingTypeDef s_config;
  
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
  
//  if(W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
//  {
//    return HAL_ERROR;
//  }
  
  return HAL_OK;
}


uint8_t W25Q128JV_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
  
  QSPI_CommandTypeDef s_command;
  
  /* Initialize read command */
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
  
  /* configuration command */
  if (HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }
  
  /* Receive data */
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
  
  do{
    W25Q128JV_Send(pData,     WriteAddr, Size);
    W25Q128JV_Read(temp_data, WriteAddr, Size);
    
    if(TimeOutCnt++ > W25Q128JV_QSPI_WRITE_COUNT) 
    {
      free(temp_data);
      return 0;
    } 
    
  }while( memcmp(pData, temp_data, Size) != 0 );
  
//  for(TimeOutCnt = 0; (memcmp(pData, temp_data, Size)!=0) && (TimeOutCnt<W25Q128JV_QSPI_WRITE_COUNT); TimeOutCnt++)
//  {
//    W25Q128JV_Send(pData,     WriteAddr, Size);
//    W25Q128JV_Read(temp_data, WriteAddr, Size);
//  }
  
  free(temp_data);
  return 1;
}


/** problem here **/
uint8_t W25Q128JV_Send(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
  
  QSPI_CommandTypeDef s_command;
  
  if(W25Q128JV_Sector_Erase(WriteAddr)!= HAL_OK) 
  {
    return HAL_ERROR;
  }
  
  /* Initialization program command */
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
  
  /* Enable write operation */
  if (W25Q128JV_Write_Enable() != HAL_OK) 
  {
    asm("nop");
  }
  
  /* configuration command */
  if(HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  { 
    asm("nop");
  }
  
  /* transfer data */
  if(HAL_QSPI_Transmit(&hqspi, pData, W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  { 
    asm("nop");
  }
  
  /* busy bit check */
  W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT);
  
  return HAL_OK;
}


uint8_t W25Q128JV_Write_Enable()
{
  
  QSPI_CommandTypeDef     s_command;
  
  /* Enable write operation */
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
  
  /* Configure automatic polling mode to wait for write enable */
  
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
  
  if(W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  {
    return HAL_ERROR;
  }
  
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
  
  /* configuration command */
  if (HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }
  
  
  s_command.Instruction       = CMD_RESET_DEVICE;
 
  /* configuration command */
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
  
  /* Configure automatic polling mode to wait for memory ready */
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
  
  while(HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, W25Q128JV_QSPI_TIMEOUT) != HAL_OK); 
  
  return HAL_OK;
}


static uint8_t W25Q128JV_Block_Erase(uint32_t BlockAddr)
{
  
  QSPI_CommandTypeDef s_command;
  
  /* Initialize the erase command */
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
  
  
  /* Enable write operation */
  if (W25Q128JV_Write_Enable() != HAL_OK) 
  {  
    return HAL_ERROR; 
  }
  
  /* send command */
  if (HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }
  
  /* Configure automatic polling mode to wait for erasure end */
  if(W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
  {
    return HAL_ERROR;
  }
  
  return HAL_OK;
}


//uint8_t W25Q128JV_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
//{
//  
//  QSPI_CommandTypeDef s_command;
//  
//  if(W25Q128JV_Sector_Erase(WriteAddr)!= HAL_OK) 
//  {
//    return HAL_ERROR;
//  }
//  
//  /* Initialization program command */
//  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
//  s_command.Instruction       = CMD__PAGE_PROGRAM;
//  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
//  s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
//  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//  s_command.DataMode          = QSPI_DATA_1_LINE;
//  s_command.NbData            = Size;
//  s_command.DummyCycles       = 0;
//  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
//  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
//  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
//  
//  
//  HAL_Delay(500);
//  //    W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT);
//  
//  /* Enable write operation */
//  if (W25Q128JV_Write_Enable() != HAL_OK) 
//  {
//    asm("nop");
//    //      return HAL_ERROR;
//  }
//  
//  /* configuration command */
//  if(HAL_QSPI_Command(&hqspi, &s_command, W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
//  { 
//    asm("nop");
//    //      return HAL_ERROR; 
//  }
//  
//  /* transfer data */
//  if(HAL_QSPI_Transmit(&hqspi, pData, W25Q128JV_QSPI_TIMEOUT) != HAL_OK) 
//  { 
//    asm("nop");
//    //    return HAL_ERROR; 
//  }
//    
//    /* transfer data */  
//    //  }while(HAL_QSPI_Transmit(&hqspi, pData, W25Q128JV_QSPI_TIMEOUT) != HAL_OK);
//  
//  /* busy bit check */
//  W25Q128JV_BusyFlagCheck(W25Q128JV_QSPI_TIMEOUT);
//  
//  return HAL_OK;
//}
