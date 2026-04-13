/*
 * rtc.h
 *
 *  Created on: 12-Apr-2026
 *      Author: Ocean
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

#include"main.h"
#include"string.h"

void uart_print(char *msg);
void rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
void rtc_set_date(uint8_t date, uint8_t month, uint8_t year, uint8_t weekday);
void rtc_get_time_date(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *date, uint8_t *month, uint8_t *year);

#endif /* INC_RTC_H_ */
