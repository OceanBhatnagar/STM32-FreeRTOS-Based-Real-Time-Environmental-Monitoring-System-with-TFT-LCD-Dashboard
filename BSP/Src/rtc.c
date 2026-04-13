/*
 * rtc.c
 *
 *  Created on: 06-Apr-2026
 *      Author: Ocean
 */

#include"main.h"
#include"string.h"
extern RTC_HandleTypeDef hrtc;
#include"stdio.h"


/* ---------------- SET TIME ---------------- */
void rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    RTC_TimeTypeDef rtc_time = {0};

    rtc_time.Hours   = hours;
    rtc_time.Minutes = minutes;
    rtc_time.Seconds = seconds;
    rtc_time.TimeFormat = RTC_HOURFORMAT12_AM;  // or PM

    HAL_RTC_SetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
}

/* ---------------- SET DATE ---------------- */
void rtc_set_date(uint8_t date, uint8_t month, uint8_t year, uint8_t weekday)
{
    RTC_DateTypeDef rtc_date = {0};

    rtc_date.Date    = date;
    rtc_date.Month   = month;
    rtc_date.Year    = year;
    rtc_date.WeekDay = weekday;

    HAL_RTC_SetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
}

/* ---------------- GET + PRINT ---------------- */
// Reads the RTC and stores the raw values in the provided pointers
void rtc_get_time_date(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *date, uint8_t *month, uint8_t *year)
{
    RTC_TimeTypeDef rtc_time;
    RTC_DateTypeDef rtc_date;

    /* IMPORTANT: Time FIRST, then Date (Required by STM32 HAL to unlock shadow registers) */
    HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);

    // Pass the values back to the calling task
    if (hour != NULL) *hour = rtc_time.Hours;
    if (min != NULL) *min = rtc_time.Minutes;
    if (sec != NULL) *sec = rtc_time.Seconds;

    if (date != NULL) *date = rtc_date.Date;
    if (month != NULL) *month = rtc_date.Month;
    if (year != NULL) *year = rtc_date.Year;
}
