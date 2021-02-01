#ifndef _W25Q128JV_H_
#define _W25Q128JV_H_

#include "main.h"

#ifdef _W25Q128JV_C_
    #define W25Q128JV_EXT
#else 
    #define W25Q128JV_EXT extern
#endif

#define BLOCK0      0x000000
#define BLOCK1      0x010000
#define BLOCK2      0x020000
#define BLOCK3      0x030000
#define BLOCK4      0x040000
#define BLOCK5      0x050000
#define BLOCK6      0x060000
#define BLOCK7      0x070000
#define BLOCK8      0x080000
#define BLOCK9      0x090000
#define BLOCK10     0x0A0000
#define BLOCK11     0x0B0000
#define BLOCK12     0x0C0000
#define BLOCK13     0x0D0000
#define BLOCK14     0x0E0000
#define BLOCK15     0x0F0000

#define SECTOR0     0x000000
#define SECTOR1     0x001000
#define SECTOR2     0x002000
#define SECTOR3     0x003000
#define SECTOR4     0x004000
#define SECTOR5     0x005000
#define SECTOR6     0x006000
#define SECTOR7     0x007000
#define SECTOR8     0x008000
#define SECTOR9     0x009000
#define SECTOR10    0x00A000
#define SECTOR11    0x00B000
#define SECTOR12    0x00C000
#define SECTOR13    0x00D000
#define SECTOR14    0x00E000
#define SECTOR15    0x00F000

/** don't use the unit of Page **/
#define PAGE0       0x000000
#define PAGE1       0x000100
#define PAGE2       0x000200
#define PAGE3       0x000300
#define PAGE4       0x000400
#define PAGE5       0x000500
#define PAGE6       0x000600
#define PAGE7       0x000700
#define PAGE8       0x000800
#define PAGE9       0x000900
#define PAGE10      0x000A00
#define PAGE11      0x000B00
#define PAGE12      0x000C00
#define PAGE13      0x000D00
#define PAGE14      0x000E00
#define PAGE15      0x000F00

#define W25Q128FV_PAGE_SIZE                 (0x100)
#define W25Q128FV_SECTOR_SIZE               (0x1000)
#define W25Q128FV_BLOCK_SIZE                (0x10000)

#define CMD_READ_ID_CMD                     (0x90)

#define CMD_WRITE_ENABLE                    (0x06)

#define CMD_ERASE_SECTOR                    (0x20)
#define CMD_ERASE_BLCOK32                   (0x52)
#define CMD_ERASE_BLCOK64                   (0xD8)
#define CMD_ERASE_CHIP                      (0xC7)

#define CMD_ENABLE_RESET                    (0x66)
#define CMD_RESET_DEVICE                    (0x99)

#define CMD_READ_SR1                        (0x05)
#define CMD_WRITE_SR1                       (0x01)
#define CMD_READ_SR2                        (0x35)
#define CMD_WRITE_SR2                       (0x31)
#define CMD_READ_SR3                        (0x15)
#define CMD_WRITE_SR3                       (0x11)

#define CMD_FAST_READ_QUAD_OUTPUT           (0x6B)
#define CMD_QUAD_INPUT_PAGE_PROGRAM         (0x32)

#define CMD__PAGE_PROGRAM                   (0x02)

#define W25Q128JV_QSPI_TIMEOUT              (5000)
#define W25Q128JV_QSPI_WRITE_COUNT          (40)


typedef __packed union{
  struct {
  uint8_t bit0 :1;
  uint8_t bit1 :1;
  uint8_t bit2 :1;
  uint8_t bit3 :1;
  uint8_t bit4 :1;
  uint8_t bit5 :1;
  uint8_t bit6 :1;
  uint8_t bit7 :1;
  };
  uint8_t Register_8bit;
}W25Q128JV_STATUS_REGISTER;

W25Q128JV_EXT uint8_t W25Q128JV_Init();

W25Q128JV_EXT uint8_t W25Q128JV_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);

W25Q128JV_EXT uint8_t W25Q128JV_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);


#endif