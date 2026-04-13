/*
 * bmp.h
 *
 *  Created on: 02-Feb-2026
 *      Author: Ocean
 */

#ifndef BSP_BMP280_INC_BMP_H_
#define BSP_BMP280_INC_BMP_H_

#include<stdint.h>
#include<stm32f4xx_hal.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
//#define BMP_I2C_ADDR  0x76
#define BMP_I2C_ADDR  (0x76 << 1)

//BMP280 REGISTERS
#define temp_xlsb   0xFC
#define temp_lsb    0xFB
#define temp_msb    0xFA
#define press_xlsb  0xF9
#define press_lsb   0xF8
#define press_msb   0xF7
#define config      0xF5
#define ctrl_meas   0xF4
#define status      0xF3
#define reset       0xE0
#define id          0xD0

//CALIBRATION
typedef struct
{
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

} BMP280_CalibData_t;





//UART TIMER FUNCTIONS
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);


//BMP CALIBRATION AND COMPENSATION FUNCTIONS
double bmp280_compensate_Temperature(int32_t temp_adc);
double bmp280_compensate_Pressure(int32_t pressure_adc);


//CORE FUNCTIONS
void BMP_Init(void);
void BMP_ReadData(int32_t *temp_adc, int32_t *pressure_adc);
void BMP_ProcessData(int32_t temp_adc, int32_t pressure_adc, int32_t *out_temp, int32_t *out_pressure, int32_t *out_alt);
/* Link to the handle defined in main.c */
extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart2;

#endif /* BSP_BMP280_INC_BMP_H_ */
