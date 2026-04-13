/*
 * dht.h
 *
 *  Created on: 25-Jan-2026
 *      Author: Ocean
 */
#ifndef DHT11_BSP_DHT_H_
#define DHT11_BSP_DHT_H_

#include<stm32f4xx_hal.h>
#include<stdint.h>
#include<string.h>
#include<stdio.h>

void DHT11_Init(void);
int  DHT11_ReadData(uint32_t pin,uint8_t *temp, uint8_t *humidity);
void UART_ProcessData(uint8_t temp, uint8_t humidity,uint16_t second);

void delay_us1(uint16_t us);
void delay_ms1(uint16_t ms);

extern UART_HandleTypeDef uart2;
extern TIM_HandleTypeDef htim6;

#endif /* DHT11_BSP_DHT_H_ */

