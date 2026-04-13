/*
 * MQ135.h
 *
 *  Created on: 15-Feb-2026
 *      Author: Ocean
 */

#ifndef BSP_INC_MQ135_H_
#define BSP_INC_MQ135_H_
#include<stm32f4xx.h>
#include<math.h>

void MQ135_Init(void);
void MQ135_Read(int32_t *ppm);


#endif /* BSP_INC_MQ135_H_ */
