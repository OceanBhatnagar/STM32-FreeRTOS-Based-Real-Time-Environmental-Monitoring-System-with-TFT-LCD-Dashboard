/*
 * MQ135.c
 *
 *  Created on: 15-Feb-2026
 *      Author: Ocean
 */

#include<main.h>
#include<math.h>
#include"MQ135.h"

extern ADC_HandleTypeDef hadc1;
uint32_t rawValue;
uint32_t voltage_mv;

void MQ135_Init(void){
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
}

void MQ135_Read(int32_t *ppm)
{
    double voltage;
    double Rs;
    double ratio;
    double ppm1;
    uint32_t rawValue;

    /* Start ADC and wait for conversion */
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    rawValue = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    /* Convert to integers for printing (scaled values) */
    int32_t voltage_mv;

    /* Convert ADC to voltage (0-3.3V) */
    voltage_mv = (rawValue * 3300) / 4095;  // in millivolts
    voltage = voltage_mv / 1000.0;

    /* Calculate sensor resistance Rs
       Circuit: VCC ---[RL]--- AOUT ---[Rs(sensor)]--- GND
       RL = 10kΩ (load resistor on MQ135 module)
       Rs = ((VCC - Vout) / Vout) * RL
    */
    if (voltage > 0.1) {
        Rs = ((3.3 - voltage) / voltage) * 10.0;  // Rs in kΩ
    } else {
        Rs = 999.0;
    }

    /* Calculate Rs/R0 ratio
       R0 = sensor resistance in clean air (calibrate this!)
       Typical R0 for MQ135 ≈ 76 kΩ in clean air
    */
    double R0 = 76.0;
    ratio = Rs / R0;

    /* Estimate PPM (simplified - for CO2)
       PPM = 116.6020682 * (Rs/R0)^-2.769034857
    */
    if (ratio > 0.01) {
        ppm1 = (116.6020682 * pow(ratio, -2.769034857));

        // Limit PPM to reasonable range
        if (ppm1 > 5000) ppm1 = 5000;
        if (ppm1 < 0) ppm1 = 0;

        *ppm = (int32_t)ppm1;
    } else {
        *ppm = 5000;
    }

}
