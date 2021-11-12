/*
 * adxl355.h
 *
 *  Created on: Nov 10, 2021
 *      Author: chotaeho
 */

#ifndef DEVICE_ADXL355_ADXL355_H_
#define DEVICE_ADXL355_ADXL355_H_

/*
 * ADXL355 (Accelerometer)
 *
 * when the sensor is not in motion, the sum of the acceleration values from X-Axis, Y-Axis, Z-Axis is the gravity.
 *
 * the unit of the value for ADXL355 is G
 * --> 1G = 9.8m/s2
 *
 * in the Sensitivity section in the Table 2 in the datasheet,
 * binary-to-acceleration conversion is found (refer to the datasheet)
 *
 * for the Evaluation Board ADXL355, the STBY pin on the board is connected to the RESERVED pin on the device
 *
 * ADXL355 device supports SPI and I2C communications.
 *
 * I2C mode
 * - external pull-up resistors are needed for I2C bus (SCL, SDA).
 * - to enable I2C mode, the SCLK/VSSIO pin must be connected to GND.
 *
 * I2C speed
 * - I2C_HS = 0 -> Fast Mode (up to 1 MHz)
 * - I2C_HS = 1 -> High Speed Mode (up to 3.4 MHz)
 *
 * I2C address
 * - 0x1D with the MISO/ASEL pin low
 * - 0x53 with the MISO/ASEL pin high
 *
 */

#include "main.h"
#include "macro_variables.h"



/* @defgroup RANGE
 * @{
 */
#define RANGE_2G  0b01
#define RANGE_4G  0b10
#define RANGE_8G  0b11
/*
 * @}
 */


/* @defgroup LSB_TO_G_SCALE_FACTOR
 * @{
 */
#define LSB_TO_G_SCALE_FACTOR_2G  0.0000039f
#define LSB_TO_G_SCALE_FACTOR_4G  0.0000078f
#define LSB_TO_G_SCALE_FACTOR_8G  0.0000156f
/*
 * @}
 */


#if ADXL355_RANGE == RANGE_2G
	#define	LSB_TO_G_SCALE_FACTOR  LSB_TO_G_SCALE_FACTOR_2G
#elif ADXL355_RANGE == RANGE_4G
	#define	LSB_TO_G_SCALE_FACTOR  LSB_TO_G_SCALE_FACTOR_4G
#else
	#define	LSB_TO_G_SCALE_FACTOR  LSB_TO_G_SCALE_FACTOR_8G
#endif /* ADXL355_RANGE == RANGE_xG */


/* @defgroup TEMPERATURE
 * @{
 */
#define TEMPERATURE_NOMINAL_INTERCEPT_C    25.0f
#define TEMPERATURE_NOMINAL_INTERCEPT_LSB  1885U
#define TEMPERATURE_NOMINAL_SLOPE         -9.05f /*!<  LSB/C  */
/*
 * @}
 */

/* @defgroup DEVICE_ADDRESS
 * @{
 */
#define DEVICE_ADDRESS_ASEL_LOW   0x1D
#define DEVICE_ADDRESS_ASEL_HIGH  0x53
/*
 * @}
 */


/* @defgroup REGISTER_ADDRESS
 * @{
 */
#define REGISTER_ADDRESS_DEVID_AD      0x00 // R
#define REGISTER_ADDRESS_DEVID_MST     0x01 // R
#define REGISTER_ADDRESS_PARTID        0x02 // R
#define REGISTER_ADDRESS_REVID         0x03 // R
#define REGISTER_ADDRESS_STATUS        0x04 // R
#define REGISTER_ADDRESS_FIFO_ENTRIES  0x05 // R
#define REGISTER_ADDRESS_TEMP2         0x06 // R
#define REGISTER_ADDRESS_TEMP1         0x07 // R
#define REGISTER_ADDRESS_XDATA3        0x08 // R
#define REGISTER_ADDRESS_XDATA2        0x09 // R
#define REGISTER_ADDRESS_XDATA1        0x0A // R
#define REGISTER_ADDRESS_YDATA3        0x0B // R
#define REGISTER_ADDRESS_YDATA2        0x0C // R
#define REGISTER_ADDRESS_YDATA1        0x0D // R
#define REGISTER_ADDRESS_ZDATA3        0x0E // R
#define REGISTER_ADDRESS_ZDATA2        0x0F // R
#define REGISTER_ADDRESS_ZDATA1        0x10 // R
#define REGISTER_ADDRESS_FIFO_DATA     0x11 // R
/* no auto-increment */
#define REGISTER_ADDRESS_OFFSET_X_H    0x1E // R/W
#define REGISTER_ADDRESS_OFFSET_X_L    0x1F // R/W
#define REGISTER_ADDRESS_OFFSET_Y_H    0x20 // R/W
#define REGISTER_ADDRESS_OFFSET_Y_L    0x21 // R/W
#define REGISTER_ADDRESS_OFFSET_Z_H    0x22 // R/W
#define REGISTER_ADDRESS_OFFSET_Z_L    0x23 // R/W
#define REGISTER_ADDRESS_ACT_EN	       0x24 // R/W
#define REGISTER_ADDRESS_ACT_THRESH_H  0x25 // R/W
#define REGISTER_ADDRESS_ACT_THRESH_L  0x26 // R/W
#define REGISTER_ADDRESS_ACT_COUNT     0x27 // R/W
#define REGISTER_ADDRESS_FILTER        0x28 // R/W
#define REGISTER_ADDRESS_FIFO_SAMPLES  0x29 // R/W
#define REGISTER_ADDRESS_INT_MAP       0x2A // R/W
#define REGISTER_ADDRESS_SYNC          0x2B // R/W
#define REGISTER_ADDRESS_RANGE         0x2C // R/W
#define REGISTER_ADDRESS_POWER_CTL     0x2D // R/W
#define REGISTER_ADDRESS_SELF_TEST     0x2E // R/W
#define REGISTER_ADDRESS_RESET         0x2F //   W
/*
 * @}
 */

#define ADXL355_DEVICE_ID  0xED

#define ADXL355_MEMORY_ADDRESS_SIZE	 I2C_MEMADD_SIZE_8BIT


typedef union {
	struct {
		uint8_t bit0:1;
		uint8_t bit1:1;
		uint8_t bit2:1;
		uint8_t bit3:1;
		uint8_t bit4:1;
		uint8_t bit5:1;
		uint8_t bit6:1;
		uint8_t bit7:1;
	};
	uint8_t value;

} uREGISTER_8BIT_t;

typedef enum {
	 eERROR_NONE,
	 eERROR_DEVICE_NOT_READY,
	 eERROR_WRONG_DEVICE_ID,
	 eERROR_READ_WRITE_FAILED

} eERROR_t;

typedef struct {
	float X_Axis;
	float Y_Axis;
	float Z_Axis;
} sADXL355_AXIS_DATA_t;

typedef struct {
	 uint8_t DevAddr;
	 sADXL355_AXIS_DATA_t AxisData;
	 eERROR_t error;
} sADXL355_HANDLER_t;


bool ADXL355_init();

bool ADXL355_updateXYZ();
bool ADXL355_updateTemp(); /*!< Not Accurate. shouldn't use */


#endif /* DEVICE_ADXL355_ADXL355_H_ */
