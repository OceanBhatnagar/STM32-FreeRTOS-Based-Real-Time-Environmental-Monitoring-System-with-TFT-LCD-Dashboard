/*
 * bsp_lcd.h
 *
 *  Created on: 10-Apr-2026
 *      Author: Ocean
 */

#ifndef INC_BSP_LCD_H_
#define INC_BSP_LCD_H_

#include"main.h"
#include "fonts.h"
 /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
//LCD SIGNALS
#define LCD_PORT      GPIOA
#define LCD_SCL       GPIO_PIN_5
#define LCD_MISO      GPIO_PIN_6
#define LCD_MOSI      GPIO_PIN_7

#define LCD_RESX      GPIO_PIN_10
#define LCD_CSX       GPIO_PIN_9
#define LCD_D_C       GPIO_PIN_4

//LCD COMMAND
#define SW_RST                  0x01
#define DISPLAY_IDENT           0x04
#define NO_ERROR                0x05
#define DISPLAY_STATUS          0x09
#define DISPLAY_POWER_MODE      0x0A
#define DISPLAY_MADCTL          0x0B
#define PIXEL_FORMAT            0x0C
#define DISPLAY_IMAGE_MODE      0x0D
#define DISPLAY_SIGNAL_MODE     0x0E
#define DISPLAY_SELF_DIAGNOSIS  0x0F

#define SLEEP_IN                0x10
#define SLEEP_OUT               0x11
#define PARTIAL_MODE_ON         0x12
#define NORMAL_DISPLAY_MODE     0x13
#define DISPLAY_INVERSION_OFF   0x20
#define DISPLAY_INVERSION_ON    0x21
#define ALL_PIXEL_OFF           0x22
#define ALL_PIXEL_ON            0x23
#define DISPLAY_OFF             0x28
#define DISPLAY_ON              0x29

#define COLUMN_ADDRESS          0x2A
#define PAGE_ADDRESS            0x2B
#define MEMORY_WRITE            0x2C
#define MEMORY_READ             0X2E
#define PARTIAL_AREA            0x30
#define VERTCAL_SCROLL_DEFINE   0x33
#define TEARING_LINE_OFF        0X34
#define TEARING_LINE_ON         0X35
#define MEMORY_ACCESS_CONTROL   0x36
#define VERTICAL_SCROLL_START   0x37

#define IDLE_MODE_OFF           0x38
#define IDLE_MODE_ON            0x39
#define INTERFACE_PIXEL         0x3A
#define MEM_WRITE_CONTINUE      0x3C
#define MEM_READ_CONTINUE       0x3E
#define WRITE_TEAR_SCAN         0x44
#define READ_TEAR_SCAN          0x45
#define WRITE_DISPLAY_BRIGHTNESS    0x51
#define READ_DISPLAY_BRIGHTNESS 0x52
#define WRITE_CTRL_DISPLAY      0x53
#define READ_CTRL_DISPLAY       0x54
#define WRITE_CONTENT_ADAP      0x55
#define READ_CONTENT_ADAP       0x56
#define WRITE_CABC              0x5E
#define READ_CABC               0x5F
#define READ_AUTOMATIC_BRIGHT   0x68
#define READ_ID1                0xDA
#define READ_ID2                0xDB
#define READ_ID3                0xDC

//LCD EXTENDED COMMAND

#define INTERFACE_MODE_CTRL      0xB0
#define FRAME_RATE_CTRL_NM       0xB1
#define FRAME_RATE_CTRL_IDLE     0xB2
#define FRAME_RATE_CTRL_PARTIAL  0xB3
#define DISPLAY_INVERSION        0xB4
#define BLANKING_PORCH           0xB5
#define DISPLAY_FUNCTION_CTRL    0xB6
#define ENTRY_MODE_SET           0xB7
#define COLOR_ENHANCE_CTRL1      0xB9
#define COLOR_ENHANCE_CTRL2      0xBA
#define HS_LANE_CTRL             0xBE
#define PWR_CTRL1                0xC0
#define PWR_CTRL2                0xC1
#define PWR_CTRL3                0xC2
#define PWR_CTRL4                0xC3
#define PWR_CTRL5                0xC4
#define VCOM_CTRL1               0xC5
#define CABC_CTRL1               0xC6
#define CABC_CTRL2               0xC8
#define CABC_CTRL3               0xC9
#define CABC_CTRL4               0xCA
#define CABC_CTRL5               0xCB
#define CABC_CTRL6               0xCC
#define CABC_CTRL7               0xCD
#define CABC_CTRL8               0xCE
#define CABC_CTRL9               0xCF
#define NV_MEMORY_WRITE          0xD0
#define NV_MEMORY_PROTECTION     0xD1
#define NV_MEMORY_STATUS         0xD2
#define READ_ID4                 0xD3
#define ADJUST_CTRL1             0xD7
#define READ_ID4_CHECK           0xD8


//DISPLAY RESOLUTION
#define LCD_WIDTH             320
#define LCD_HEIGHT            480

//PIXEL FORMAT
#define LCD_L8       1
#define LCD_RGB565   2
#define LCD_RGB666   3
#define LCD_RGB888   4

//ORIENTATION
#define PORTRAIT    0
#define LANDSCAPE   1

#define BSP_LCD_ORIENTATION PORTRAIT   // default startup orientation

#define BSP_LCD_ACTIVE_WIDTH    320
#define BSP_LCD_ACTIVE_HEIGHT   480


//---------------- CHIP SELECT MANAGEMENT ----------------//
#define AUTO     1
#define MANUAL   0
#define BSP_LCD_CS_MANAGE    MANUAL

//AREA STRUCTURE

typedef struct{
	uint16_t x1;
	uint16_t x2;
	uint16_t y1;
	uint16_t y2;
}lcd_area_t;

//LCD HANDLE

//---------------- MAIN LCD HANDLE ----------------//

typedef struct bsp_lcd{

    uint8_t orientation;
    uint8_t pixel_format;

    // Draw buffers (for double buffering / DMA later)
    uint8_t *draw_buffer1;
    uint8_t *draw_buffer2;

    uint32_t write_length;

    uint8_t *buff_to_draw;
    uint8_t *buff_to_flush;

    lcd_area_t area;

} bsp_lcd_t;

// Text Alignment Modes
#define LEFT_MODE   0x01
#define CENTER_MODE 0x02
#define RIGHT_MODE  0x03
//FUNCTION PROTOTYPE
void BSP_LCD_Init(void);

void bsp_lcd_set_display_area(uint16_t x1, uint16_t x2, uint16_t y1 , uint16_t y2);
void lcd_set_display_area(lcd_area_t *area);
void lcd_set_orientation(uint8_t orientation);
void lcd_buffer_init(bsp_lcd_t *lcd);
void lcd_flush(bsp_lcd_t *hlcd);

void bsp_lcd_send_cmd_mem_write(void);
uint32_t bsp_lcd_convert_rgb888_to_rgb666(uint32_t rgb888);
void *bsp_lcd_get_draw_buffer1_addr(void);
void *bsp_lcd_get_draw_buffer2_addr(void);

void bsp_lcd_set_background_color(uint32_t rgb888);
void bsp_lcd_fill_rect(uint32_t rgb888, uint32_t x_start, uint32_t x_width,uint32_t y_start,uint32_t y_height);

void bsp_lcd_draw_pixel(uint16_t x, uint16_t y, uint32_t rgb888);
void bsp_lcd_draw_char(uint16_t x, uint16_t y, char ch,sFONT *font, uint32_t color, uint32_t bg_color);
void bsp_lcd_draw_string(uint16_t x, uint16_t y, const char *str, sFONT *font, uint32_t color, uint32_t bg_color);

#endif /* INC_BSP_LCD_H_ */
