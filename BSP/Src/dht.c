/*
 * dht.c
 *
 *  Created on: 25-Jan-2026
 *      Author: Ocean
 */


#include "dht.h"
#include<string.h>
#include "main.h"
uint8_t data[5] = {0};
uint8_t i, j;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim11;

void DHT11_Init(void){
	 if(HAL_UART_Init(&huart2) != HAL_OK){
	 		//Error
	 		Error_Handler();
	 	}
	   if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
	    {
	      Error_Handler();
	    }

	   HAL_TIM_Base_Start(&htim11);

}

int DHT11_ReadData(uint32_t pin, uint8_t *temp, uint8_t *humidity) {
    uint8_t data[5] = {0}; // Local data buffer cleared every time
    uint16_t timeout = 0;

    //CONFIGURE GPIO PIN
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    //MCU SEND START SIGNAL
    HAL_GPIO_WritePin(GPIOA, pin, GPIO_PIN_RESET);
    delay_ms1(18); // DHT11 needs at least 18ms

    //MCU RELEASE THE BUS
    HAL_GPIO_WritePin(GPIOA, pin, GPIO_PIN_SET);
    delay_us1(30);

    // 2. SWITCH TO INPUT
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // DHT11 RESPONSE
    timeout = 0;
    while(HAL_GPIO_ReadPin(GPIOA, pin)) { // Wait for DHT to pull Low
        if (++timeout > 10000) return -1;
    }

    timeout = 0;
    while(!HAL_GPIO_ReadPin(GPIOA, pin)) { // Wait for DHT to pull High
        if (++timeout > 10000) return -1;
    }

    timeout = 0;
    while(HAL_GPIO_ReadPin(GPIOA, pin)) { // Wait for DHT to pull Low to start bit 0
        if (++timeout > 10000) return -1;
    }

    //  READ 40 BITS
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 8; i++) {
            // Wait for pin to go HIGH (start of pulse)
            timeout = 0;
            while (!HAL_GPIO_ReadPin(GPIOA, pin)) {
                if (++timeout > 10000) return -1;
            }

            delay_us1(40); // Pulse is 26us for '0' and 70us for '1'

            if (HAL_GPIO_ReadPin(GPIOA, pin)) {
                data[j] |= (1 << (7 - i));

                // CRITICAL: If it was a '1', we are still HIGH.
                // We MUST wait for the pin to go LOW before starting the next bit loop.
                timeout = 0;
                while (HAL_GPIO_ReadPin(GPIOA, pin)) {
                    if (++timeout > 10000) return -1;
                }
            }
        }
    }

    // DHT11 RELEASE LINE AUTOMATICALLY

    //IDLE MODE
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOA, pin, GPIO_PIN_SET);

    //CHECKSUM
    if (data[4] != (uint8_t)(data[0] + data[1] + data[2] + data[3])) {
        return -2; // Checksum error
    }

    *humidity = data[0];
    *temp = data[2];
    return 0;
}

void UART_ProcessData(uint8_t temp, uint8_t humidity,uint16_t second){
	char str1[50];
	sprintf(str1, "Temp: %d C, Humidity: %d %%\r\n", temp, humidity);
	HAL_UART_Transmit(&huart2, (uint8_t*)str1, strlen(str1), 100);
	delay_ms1(second);
}

void delay_us1(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim11, 0);
    while (__HAL_TIM_GET_COUNTER(&htim11) < us);
}

void delay_ms1(uint16_t ms)
{
    while (ms--)
    {
        delay_us1(1000);
    }
}
