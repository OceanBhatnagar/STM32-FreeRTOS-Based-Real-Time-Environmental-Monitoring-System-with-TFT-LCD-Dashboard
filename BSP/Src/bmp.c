/*
 * bmp.c
 *
 *  Created on: 02-Feb-2026
 *      Author: Ocean
 */

#include"bmp.h"
#include<string.h>
#include "main.h"

extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim11;
extern UART_HandleTypeDef huart2;

static BMP280_CalibData_t bmp_calib;
static void BMP_ReadCalibration(void);
static uint8_t bmp_read(uint8_t reg_addr);
static void bmp280_read_burst(uint8_t start_reg, uint8_t *buffer, uint8_t len);
static void bmp_write(uint8_t value, uint8_t reg_addr) ;
//CORE FUNCTIONS

void BMP_Init(void){

	//POWER ON DELAY
	delay_ms(2);

	//READ ID
	uint8_t ID=bmp_read(id);
	if( ID!=0X58){
		Error_Handler();
	}

	//SOFT RESET
    bmp_write(0xB6,reset);

	//POWER RESET DELAY
	delay_ms(2);

	//READ CALIBRATION DATA
	BMP_ReadCalibration();

	/*WRITE CONFIGURATION*/
	// STANDBY TIME 1000MS 101
	// DISABLE IR FILTER   000
	 // SET I2C MODE       00
	bmp_write(0xA0,config);

	/*DATA OVERSAMPLING8*/
	//TEMP OVERSAMPLINGX2   010
	//PRES OVERSAMPLINGX16  101
	//POWER MODE            11
	bmp_write(0x57,ctrl_meas);
}



void BMP_ReadData(int32_t *temp_adc, int32_t *pressure_adc){

	uint8_t raw[6];

	uint32_t timeout = 100000;
	while ((bmp_read(0xF3) & 0x08) && timeout--){};


	    // Burst read: pressure + temperature
	    bmp280_read_burst(0xF7, raw, 6);

	    *pressure_adc = (int32_t)((raw[0] << 12) |
	                              (raw[1] << 4)  |
	                              (raw[2] >> 4));

	    *temp_adc     = (int32_t)((raw[3] << 12) |
	                              (raw[4] << 4)  |
	                              (raw[5] >> 4));
}

void BMP_ProcessData(int32_t temp_adc, int32_t pressure_adc, int32_t *out_temp, int32_t *out_pressure, int32_t *out_alt)
{
    double temp = bmp280_compensate_Temperature(temp_adc);
    double pressure = bmp280_compensate_Pressure(pressure_adc);
    double pressure_hpa = pressure / 100.0;
    double altitude = 44330.0 * (1.0 - pow(pressure_hpa / 1013.25, 0.1903));

    // Store as scaled integers (x100) to preserve 2 decimal places without using floats in IPC
    *out_temp = (int32_t)(temp * 100);
    *out_pressure = (int32_t)(pressure_hpa * 100);
    *out_alt = (int32_t)(altitude * 100);
}
//CALIBRATION DATA

#define BMP280_CALIB_START_REG  0x88
#define BMP280_CALIB_LEN        24
static void BMP_ReadCalibration(void)
{
    uint8_t calib_raw[24];
    uint8_t reg = 0x88;

    // Burst read calibration registers (0x88–0xA1)
    HAL_I2C_Master_Transmit(&hi2c2,BMP_I2C_ADDR, &reg, 1, 100);
    HAL_I2C_Master_Receive(&hi2c2,BMP_I2C_ADDR , calib_raw, 24, 100);

    // Temperature calibration
    bmp_calib.dig_T1 = (uint16_t)((calib_raw[1]  << 8) | calib_raw[0]);
    bmp_calib.dig_T2 = (int16_t) ((calib_raw[3]  << 8) | calib_raw[2]);
    bmp_calib.dig_T3 = (int16_t) ((calib_raw[5]  << 8) | calib_raw[4]);

    // Pressure calibration
    bmp_calib.dig_P1 = (uint16_t)((calib_raw[7]  << 8) | calib_raw[6]);
    bmp_calib.dig_P2 = (int16_t) ((calib_raw[9]  << 8) | calib_raw[8]);
    bmp_calib.dig_P3 = (int16_t) ((calib_raw[11] << 8) | calib_raw[10]);
    bmp_calib.dig_P4 = (int16_t) ((calib_raw[13] << 8) | calib_raw[12]);
    bmp_calib.dig_P5 = (int16_t) ((calib_raw[15] << 8) | calib_raw[14]);
    bmp_calib.dig_P6 = (int16_t) ((calib_raw[17] << 8) | calib_raw[16]);
    bmp_calib.dig_P7 = (int16_t) ((calib_raw[19] << 8) | calib_raw[18]);
    bmp_calib.dig_P8 = (int16_t) ((calib_raw[21] << 8) | calib_raw[20]);
    bmp_calib.dig_P9 = (int16_t) ((calib_raw[23] << 8) | calib_raw[22]);
}


// COMPENSATION FORMULA FOR TEMPERATURE AND PRESSURE

int32_t t_fine;
double bmp280_compensate_Temperature(int32_t temp_adc)
{
  double var1, var2, T;
  var1  = ( ((double)temp_adc)/16384.0 - ((double) bmp_calib.dig_T1 )/1024.0) * ((double)bmp_calib.dig_T2);
  var2  = ((((double)temp_adc)/131072.0 - ((double)bmp_calib.dig_T1)/8192.0) *
         (((double)temp_adc)/131072.0 - ((double) bmp_calib.dig_T1)/8192.0)) * ((double)bmp_calib.dig_T3);
   t_fine = (int32_t)(var1 + var2);
       T  = (var1 + var2) / 5120.0;
       return T;
}

double bmp280_compensate_Pressure(int32_t pressure_adc)
{
    double var1, var2, p;
    var1 = ((double)t_fine/2.0) - 64000.0;
    var2 = var1 * var1 * ((double)bmp_calib.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)bmp_calib.dig_P5) * 2.0;
    var2 = (var2/4.0)+(((double)bmp_calib.dig_P4) * 65536.0);
    var1 = (((double)bmp_calib.dig_P3) * var1 * var1 / 524288.0 + ((double)bmp_calib.dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0)*((double)bmp_calib.dig_P1);
    if (var1 == 0.0)
    {
    	return 0; // avoid exception caused by division by zero
         }

     p = 1048576.0 - (double)pressure_adc;
     p = (p - (var2 / 4096.0)) * 6250.0 / var1;
     var1 = ((double)bmp_calib.dig_P9) * p * p / 2147483648.0;
     var2 = p * ((double)bmp_calib.dig_P8) / 32768.0;
     p = p + (var1 + var2 + ((double)bmp_calib.dig_P7)) / 16.0;
     return p;
  }



// I2C AND TIMER FUNCTIONS
void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim11, 0);
    while (__HAL_TIM_GET_COUNTER(&htim11) < us);
}

void delay_ms(uint16_t ms)
{
    while (ms--)
    {
        delay_us(1000);
    }
}

static void bmp_write(uint8_t value, uint8_t reg_addr) {
    uint8_t tx[2];
    tx[0] = reg_addr;
    tx[1] = value;
    // Use hi2c2 directly. Timeout set to 100ms instead of Infinite
    HAL_I2C_Master_Transmit(&hi2c2, BMP_I2C_ADDR, tx, 2, 100);
}

static uint8_t bmp_read(uint8_t reg_addr)
{
    uint8_t data = 0;
    HAL_I2C_Master_Transmit(&hi2c2, BMP_I2C_ADDR, &reg_addr, 1, 100);
    HAL_I2C_Master_Receive(&hi2c2, BMP_I2C_ADDR, &data, 1, 100);
    return data;
}

static void bmp280_read_burst(uint8_t start_reg, uint8_t *buffer, uint8_t len)
{
    HAL_I2C_Master_Transmit(&hi2c2,  BMP_I2C_ADDR, &start_reg, 1, 100);
    HAL_I2C_Master_Receive(&hi2c2,   BMP_I2C_ADDR, buffer, len, 100);
}

