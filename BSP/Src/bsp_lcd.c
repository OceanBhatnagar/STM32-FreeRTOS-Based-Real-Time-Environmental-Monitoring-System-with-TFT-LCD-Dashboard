/*
 * bsp_lcd.c
 *
 *  Created on: 10-Apr-2026
 *      Author: Ocean
 */


#include"main.h"
#include"stm32f4xx.h"
#include"bsp_lcd.h"
#include "fonts.h" // Ensure you have standard STM32 fonts included
#include<stdbool.h>
#include "cmsis_os.h"

#include"main.h"
#include"stm32f4xx.h"
#include"bsp_lcd.h"

//DEFINALTION
extern SPI_HandleTypeDef hspi1;
bsp_lcd_t lcd;
bsp_lcd_t *hlcd = &lcd;

//FUNCTION PROTOTYPE
void LCD_Write_Cmd(uint8_t cmd);
void LCD_Write_Data(uint8_t *buffer, uint32_t len);
void LCD_Config(void);
void LCD_RESET(void);

//FUNCTION IMPLEMENTATION


//INITIALIZATION FUNCTIONS

void BSP_LCD_Init(void){
	//GPIO INITIALIZATION
	//SET CSX PIN AS HIGH
	 HAL_GPIO_WritePin(LCD_PORT,LCD_CSX,GPIO_PIN_SET);
	//SET RESX AS HIGH
	 HAL_GPIO_WritePin(LCD_PORT,LCD_RESX,GPIO_PIN_SET);
	//SET D/C AS HIGH
	 HAL_GPIO_WritePin(LCD_PORT,LCD_D_C,GPIO_PIN_SET);

	//ORIENTATION AND PIXEL FORMAT
	lcd.orientation=BSP_LCD_ORIENTATION;
	lcd.pixel_format=LCD_RGB666;


	//LCD RESET
	LCD_RESET();

	//LCD CONFIG
	LCD_Config();

	hlcd->area.x1=0;
	hlcd->area.x2=BSP_LCD_ACTIVE_WIDTH-1;
	hlcd->area.y1 = 0;
	hlcd->area.y2 = BSP_LCD_ACTIVE_HEIGHT-1;

	//LCD SET DISPLAY AREA
	lcd_set_display_area(&hlcd->area);

	//LCD SET ORIENTATION
	lcd_set_orientation(hlcd->orientation);

	//LCD BUFFER
	lcd_buffer_init(hlcd);
}

void LCD_RESET(void){
	//SET RESX AS LOW
	HAL_GPIO_WritePin(LCD_PORT,LCD_RESX,GPIO_PIN_RESET);
	osDelay(10);
	HAL_GPIO_WritePin(LCD_PORT,LCD_RESX,GPIO_PIN_SET);
	osDelay(120);
}

void LCD_Config(void)
{
	uint8_t param[15];

	//SOFTWARE RESET
    LCD_Write_Cmd(SW_RST);
    osDelay(120);

    //SLEEP OUT
    /*WAKES DISPLAY FROM LOW POWER*/
    LCD_Write_Cmd(SLEEP_OUT);
    osDelay(120);

    //POWER CONTROL1
    /*CONTROL INTERNAL VOLTAGE REGULATOR*/
    LCD_Write_Cmd(PWR_CTRL1);
    param[0]=0x17;
    param[1]=0x15;
    LCD_Write_Data(param, 2);

    //POWER CONTROL2
    /*PANEL DRIVING VOLTAGE*/
    LCD_Write_Cmd(PWR_CTRL2);
    param[0]=0x41;
    LCD_Write_Data(param, 1);

    //VCOM CONTROL
    LCD_Write_Cmd(VCOM_CTRL1);
    param[0]=0x00;
    param[1]=0x12;
    param[2]=0x80;
    LCD_Write_Data(param,3);

    //MEMORY ACCESS CONTROL
    /*CONTROL RGB ORDER*/
    LCD_Write_Cmd(MEMORY_ACCESS_CONTROL);
    param[0]=0x48;
    LCD_Write_Data(param,1);

    //PIXEL FORMAT
    LCD_Write_Cmd(INTERFACE_PIXEL);
    param[0]=0x66; //55 RGB565, 66 RGB 666
    LCD_Write_Data(param,1);

    //INTERFACE MODE CONTROL
    LCD_Write_Cmd(INTERFACE_MODE_CTRL);
    param[0]=0x00;
    LCD_Write_Data(param, 1);

    //FRAME RATE CONTROL
    LCD_Write_Cmd(FRAME_RATE_CTRL_NM);
    param[0]=0xA0;
    LCD_Write_Data(param, 1);

    //DISPLAY FUNCTION CONTROL
    /*CONTROL SCAN MODE, LINE INVERSION, DISPLAY TIMING*/
    LCD_Write_Cmd(DISPLAY_FUNCTION_CTRL);
    param[0]=0x02;
    param[1]=0x02;
    LCD_Write_Data(param,2);

    //POSITIVE GAMMA CORRECTION
    /*Define Brigthness curve for positive voltage */
    LCD_Write_Cmd(0xE0);
    uint8_t gamma_pos[15] = {
        0x00,0x04,0x0E,0x08,0x17,0x0A,0x40,0x79,
        0x4D,0x07,0x0E,0x0A,0x1A,0x1D,0x0F
    };
    LCD_Write_Data(gamma_pos,15);

    //NEGATIVE GAMMA CORRECTION
    /*Define Brigthness curve for Negative voltage */
    LCD_Write_Cmd(0xE1);
    uint8_t gamma_neg[15] = {
        0x00,0x1B,0x1F,0x02,0x10,0x05,0x32,0x34,
        0x43,0x02,0x0A,0x09,0x33,0x37,0x0F
    };
    LCD_Write_Data(gamma_neg,15);

    //COLUMN ADDRESS SET
    /*Define X Axis Range:*/
    LCD_Write_Cmd(COLUMN_ADDRESS);
    param[0]=0x00;  //START=0
    param[1]=0x00;
    param[2]=0x01; //END=319
    param[3]=0x3F;
    LCD_Write_Data(param,4);

    //PAGE ADDRESS SET
    /*Define Y Axis Range*/
    LCD_Write_Cmd(PAGE_ADDRESS);
    param[0]=0x00;
    param[1]=0x00; //START=0
    param[2]=0x01;
    param[3]=0xDF; //END=479
    LCD_Write_Data(param,4);

    //LCD DISPLAY ON
    LCD_Write_Cmd(DISPLAY_ON);
    osDelay(50);
}

void LCD_Write_Cmd(uint8_t cmd){
	//SET CSX PIN AS LOW
	HAL_GPIO_WritePin(LCD_PORT,LCD_CSX,GPIO_PIN_RESET);

	//SET D/C AS LOW
	HAL_GPIO_WritePin(LCD_PORT,LCD_D_C,GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi1,&cmd,1,HAL_MAX_DELAY);

	//SET CSX PIN AS HIGH
    HAL_GPIO_WritePin(LCD_PORT,LCD_CSX,GPIO_PIN_SET);

    //SET D/C AS HIGH
     HAL_GPIO_WritePin(LCD_PORT,LCD_D_C,GPIO_PIN_SET);

}


void LCD_Write_Data(uint8_t *buffer, uint32_t len)
{
	// SET D/C PIN AS HIGH (DATA)
	    HAL_GPIO_WritePin(LCD_PORT, LCD_D_C, GPIO_PIN_SET);

	//SET CSX PIN AS LOW
    HAL_GPIO_WritePin(LCD_PORT,LCD_CSX,GPIO_PIN_RESET);

    HAL_SPI_Transmit(&hspi1, buffer, len, HAL_MAX_DELAY);

    //SET CSX PIN AS HIGH
    HAL_GPIO_WritePin(LCD_PORT,LCD_CSX,GPIO_PIN_SET);
}

//MAIN FUNCTIONS

//DISPLAY AREA
void bsp_lcd_set_display_area(uint16_t x1, uint16_t x2, uint16_t y1 , uint16_t y2){
	lcd_area_t area;
	area.x1 = x1;
	area.x2 = x2;
	area.y1 = y1;
	area.y2 = y2;
	lcd_set_display_area(&area);
}

void lcd_set_display_area(lcd_area_t *area)
{
    uint8_t params[4];

    //  X (COLUMN)
    params[0] = (area->x1 >> 8) & 0xFF;
    params[1] = (area->x1) & 0xFF;
    params[2] = (area->x2 >> 8) & 0xFF;
    params[3] = (area->x2) & 0xFF;

    LCD_Write_Cmd(COLUMN_ADDRESS);
    LCD_Write_Data(params, 4);

    // Y (PAGE)
    params[0] = (area->y1 >> 8) & 0xFF;
    params[1] = (area->y1) & 0xFF;
    params[2] = (area->y2 >> 8) & 0xFF;
    params[3] = (area->y2) & 0xFF;

    LCD_Write_Cmd(PAGE_ADDRESS);
    LCD_Write_Data(params, 4);
}

//ORIENTATION
void lcd_set_orientation(uint8_t orientation)
{
    uint8_t param;

    // Save the orientation to the global struct!
    hlcd->orientation = orientation;

    if (orientation == LANDSCAPE)
        param = 0x28;   // correct landscape
    else
        param = 0x48;   // correct portrait

    LCD_Write_Cmd(MEMORY_ACCESS_CONTROL);
    LCD_Write_Data(&param, 1);
}
//BUFFER AREA

#define LCD_BPP            3  // RGB666 → 3 bytes per pixel
#define LCD_BUF_LINES      2  // Number of lines per buffer
#define LCD_WIDTH          320 // ILI9488

// Draw buffer (front)
uint8_t bsp_db[LCD_WIDTH * LCD_BUF_LINES * LCD_BPP];
// Write/flush buffer (back)
uint8_t bsp_wb[LCD_WIDTH * LCD_BUF_LINES * LCD_BPP];

void lcd_buffer_init(bsp_lcd_t *lcd)
{
    lcd->draw_buffer1 = (uint8_t*)bsp_db;
    lcd->draw_buffer2 = (uint8_t*)bsp_wb;

    lcd->buff_to_draw  = lcd->draw_buffer1;
    lcd->buff_to_flush = lcd->draw_buffer2;

    lcd->write_length = LCD_WIDTH * LCD_BUF_LINES * LCD_BPP;
}

void lcd_flush(bsp_lcd_t *hlcd)
{
    // 1. Set GRAM window (X + Y)
    lcd_set_display_area(&hlcd->area);

    // 2. Memory Write command
    LCD_Write_Cmd(MEMORY_WRITE);

    // 3. Send pixel data (blocking SPI)
    HAL_GPIO_WritePin(LCD_PORT, LCD_CSX, GPIO_PIN_RESET);   // CS LOW
    HAL_GPIO_WritePin(LCD_PORT, LCD_D_C, GPIO_PIN_SET);     // DATA MODE

    HAL_SPI_Transmit(&hspi1,
                     hlcd->buff_to_flush,
                     hlcd->write_length,
                     HAL_MAX_DELAY);

    HAL_GPIO_WritePin(LCD_PORT, LCD_CSX, GPIO_PIN_SET);     // CS HIGH

    // 4. Mark buffer free
    hlcd->buff_to_flush = NULL;
}

void bsp_lcd_send_cmd_mem_write(void)
{
	LCD_Write_Cmd(MEMORY_WRITE);
}

void *bsp_lcd_get_draw_buffer1_addr(void)
{
    return (void*)hlcd->draw_buffer1;
}
void *bsp_lcd_get_draw_buffer2_addr(void)
{
	return (void*)hlcd->draw_buffer2;
}


uint32_t bsp_lcd_convert_rgb888_to_rgb666(uint32_t rgb888) {
    uint32_t r, g, b;

    // Extract the 6 MSB of each channel and keep them in their 8-bit position
    // Masking with 0xFC (1111 1100) removes the bottom 2 bits
    r = (rgb888 & 0xFF0000) & 0xFC0000;
    g = (rgb888 & 0x00FF00) & 0x00FC00;
    b = (rgb888 & 0x0000FF) & 0x0000FC;

    // Returns a 24-bit value: [RED_6bit][00][GREEN_6bit][00][BLUE_6bit][00]
    return (r | g | b);
}

void bsp_lcd_set_background_color(uint32_t rgb888)
{
    bsp_lcd_fill_rect(rgb888,
                      0, BSP_LCD_ACTIVE_WIDTH,
                      0, BSP_LCD_ACTIVE_HEIGHT);
}

void bsp_lcd_fill_rect(uint32_t rgb888, uint32_t x_start,
		uint32_t x_width,uint32_t y_start,uint32_t y_height)
{
    // Dynamically determine the maximum X and Y based on orientation
    uint32_t max_x = (hlcd->orientation == LANDSCAPE) ? BSP_LCD_ACTIVE_HEIGHT : BSP_LCD_ACTIVE_WIDTH;
    uint32_t max_y = (hlcd->orientation == LANDSCAPE) ? BSP_LCD_ACTIVE_WIDTH : BSP_LCD_ACTIVE_HEIGHT;

    if ((x_start + x_width) > max_x) return;
    if ((y_start + y_height) > max_y) return;

    // 1. Set full window ONCE
    bsp_lcd_set_display_area(x_start,
                             x_start + x_width - 1,
                             y_start,
                             y_start + y_height - 1);

    // 2. Start GRAM write
    LCD_Write_Cmd(MEMORY_WRITE);

    // 3. Convert color once
    uint32_t color666 = bsp_lcd_convert_rgb888_to_rgb666(rgb888);

    // Extract individual bytes for the SPI payload
    uint8_t r = (color666 >> 16) & 0xFF;
    uint8_t g = (color666 >> 8) & 0xFF;
    uint8_t b = color666 & 0xFF;

    // 4. Fill draw buffer with the 3-byte color sequence
    // Changed: Divide by 3 because LCD_BPP is 3
    uint32_t buffer_pixels = hlcd->write_length / 3;

    // Changed: Treat buffer as uint8_t, NOT uint16_t
    uint8_t *buf = (uint8_t *)hlcd->draw_buffer1;

    for (uint32_t i = 0; i < buffer_pixels; i++)
    {
        buf[i * 3]     = r;
        buf[i * 3 + 1] = g;
        buf[i * 3 + 2] = b;
    }

    // 5. Total pixels to send
    uint32_t total_pixels = x_width * y_height;
    uint32_t pixels_sent = 0;

    while (pixels_sent < total_pixels)
    {
        uint32_t chunk_pixels = buffer_pixels;

        if ((total_pixels - pixels_sent) < buffer_pixels)
        {
            chunk_pixels = total_pixels - pixels_sent;
        }

        // Send chunk
        HAL_GPIO_WritePin(LCD_PORT, LCD_CSX, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LCD_PORT, LCD_D_C, GPIO_PIN_SET);

        // Changed: Multiply by 3 for byte count (chunk_pixels * 3)
        HAL_SPI_Transmit(&hspi1,
                         buf,
                         chunk_pixels * 3,
                         HAL_MAX_DELAY);

        HAL_GPIO_WritePin(LCD_PORT, LCD_CSX, GPIO_PIN_SET);

        pixels_sent += chunk_pixels;
    }
}
// Draw a single character using the draw buffer for maximum SPI throughput
void bsp_lcd_draw_char(uint16_t x, uint16_t y, char ch, sFONT *font, uint32_t color, uint32_t bg_color)
{
    uint32_t max_x = (hlcd->orientation == LANDSCAPE) ? BSP_LCD_ACTIVE_HEIGHT : BSP_LCD_ACTIVE_WIDTH;
    uint32_t max_y = (hlcd->orientation == LANDSCAPE) ? BSP_LCD_ACTIVE_WIDTH : BSP_LCD_ACTIVE_HEIGHT;

    // Check bounds
    if ((x + font->Width) > max_x || (y + font->Height) > max_y) return;

    // Set the GRAM window to the exact size of the character
    bsp_lcd_set_display_area(x, x + font->Width - 1, y, y + font->Height - 1);
    LCD_Write_Cmd(MEMORY_WRITE);

    // Convert foreground and background to RGB666
    uint32_t fg_666 = bsp_lcd_convert_rgb888_to_rgb666(color);
    uint8_t fg_r = (fg_666 >> 16) & 0xFF;
    uint8_t fg_g = (fg_666 >> 8) & 0xFF;
    uint8_t fg_b = fg_666 & 0xFF;

    uint32_t bg_666 = bsp_lcd_convert_rgb888_to_rgb666(bg_color);
    uint8_t bg_r = (bg_666 >> 16) & 0xFF;
    uint8_t bg_g = (bg_666 >> 8) & 0xFF;
    uint8_t bg_b = bg_666 & 0xFF;

    uint8_t *buf = (uint8_t *)hlcd->draw_buffer1;
    uint32_t pixel_idx = 0;

    // Calculate how many bytes make up one row of the character
    // Example: Width 11 = (11+7)/8 = 2 bytes per row.
    uint8_t bytes_per_row = (font->Width + 7) / 8;

    // Find the starting byte for this specific character in the array
    uint32_t char_offset = (ch - ' ') * font->Height * bytes_per_row;

    // Iterate through the rows (Height)
    for (uint16_t i = 0; i < font->Height; i++)
    {
        // Pointer to the start of the current row's bytes
        const uint8_t *row_data = &font->table[char_offset + (i * bytes_per_row)];

        // Iterate through the pixels in the row (Width)
        for (uint16_t j = 0; j < font->Width; j++)
        {
            uint8_t byte_idx = j / 8; // Which byte in the row?
            uint8_t bit_idx = j % 8;  // Which bit in that byte?

            // ST fonts store the leftmost pixel in the Most Significant Bit (0x80)
            if (row_data[byte_idx] & (0x80 >> bit_idx))
            {
                buf[pixel_idx * 3]     = fg_r;
                buf[pixel_idx * 3 + 1] = fg_g;
                buf[pixel_idx * 3 + 2] = fg_b;
            }
            else
            {
                buf[pixel_idx * 3]     = bg_r;
                buf[pixel_idx * 3 + 1] = bg_g;
                buf[pixel_idx * 3 + 2] = bg_b;
            }
            pixel_idx++;
        }
    }

    // Blast the assembled character buffer over SPI in one go
    HAL_GPIO_WritePin(LCD_PORT, LCD_CSX, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_PORT, LCD_D_C, GPIO_PIN_SET);
    HAL_SPI_Transmit(&hspi1, buf, pixel_idx * 3, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_PORT, LCD_CSX, GPIO_PIN_SET);
}
// Draw a full string, managing text wrapping
void bsp_lcd_draw_string(uint16_t x, uint16_t y, const char *str, sFONT *font, uint32_t color, uint32_t bg_color)
{
    uint32_t max_x = (hlcd->orientation == LANDSCAPE) ? BSP_LCD_ACTIVE_HEIGHT : BSP_LCD_ACTIVE_WIDTH;
    uint16_t start_x = x;

    while (*str)
    {
        // Handle newline character
        if (*str == '\n')
        {
            y += font->Height;
            x = start_x;
            str++;
            continue;
        }

        // Auto-wrap if character goes off the right edge
        if (x + font->Width > max_x)
        {
            x = start_x;
            y += font->Height;
        }

        bsp_lcd_draw_char(x, y, *str, font, color, bg_color);

        x += font->Width; // Move cursor for next character
        str++;
    }
}
