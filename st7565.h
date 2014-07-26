/*
 * st7565.h
 *
 *  Created on: May 17, 2014
 *      Author: ahmed
 */

#ifndef ST7565_H_
#define ST7565_H_

#define LCDWIDTH 128
#define LCDHEIGHT 64
#define LCD_BUFFER_SIZE 1024

#define SLAVE_SELECT_PIN	PIN_PA09#define RESET_PIN			PIN_PA14#define DATA_PIN			PIN_PA15

// Commands
#define ST7565_CMD_DISPLAY_OFF            0xAE
#define ST7565_CMD_DISPLAY_ON             0xAF
#define ST7565_CMD_SET_DISP_START_LINE    0x40
#define ST7565_CMD_SET_PAGE               0xB0
#define ST7565_CMD_SET_COLUMN_UPPER       0x10
#define ST7565_CMD_SET_COLUMN_LOWER       0x00
#define ST7565_CMD_SET_ADC_NORMAL         0xA0
#define ST7565_CMD_SET_ADC_REVERSE        0xA1
#define ST7565_CMD_SET_DISP_NORMAL        0xA6
#define ST7565_CMD_SET_DISP_REVERSE       0xA7
#define ST7565_CMD_SET_ALLPTS_NORMAL      0xA4
#define ST7565_CMD_SET_ALLPTS_ON          0xA5
#define ST7565_CMD_SET_BIAS_9             0xA2
#define ST7565_CMD_SET_BIAS_7             0xA3
#define ST7565_CMD_RMW                    0xE0
#define ST7565_CMD_RMW_CLEAR              0xEE
#define ST7565_CMD_INTERNAL_RESET         0xE2
#define ST7565_CMD_SET_COM_NORMAL         0xC0
#define ST7565_CMD_SET_COM_REVERSE        0xC8
#define ST7565_CMD_SET_POWER_CONTROL      0x28
#define ST7565_CMD_SET_RESISTOR_RATIO     0x20
#define ST7565_CMD_SET_VOLUME_FIRST       0x81
#define ST7565_CMD_SET_VOLUME_SECOND      0x0F
#define ST7565_CMD_SET_STATIC_OFF         0xAC
#define ST7565_CMD_SET_STATIC_ON          0xAD
#define ST7565_CMD_SET_STATIC_REG         0x0
#define ST7565_CMD_SET_BOOSTER_FIRST      0xF8
#define ST7565_CMD_SET_BOOSTER_234        0
#define ST7565_CMD_SET_BOOSTER_5          1
#define ST7565_CMD_SET_BOOSTER_6          3
#define ST7565_CMD_NOP                    0xE3
#define ST7565_CMD_TEST                   0xF0

#define swap(a, b) { uint8_t t = a; a = b; b = t; }
#include <stdint.h>

void st7565_init(void);
void st7565_flush(void);
void st7565_clear(void);
void st7565_setBrightness(uint8_t val);

void st7565_clearPixel(uint8_t x, uint8_t y);
void st7565_drawPixel(uint8_t x, uint8_t y);
void st7565_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void st7565_drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

void st7565_showLogo(void);
void st7565_drawChar(uint8_t x, uint8_t line, char c);
void st7565_drawString(uint8_t x, uint8_t line, char *c);
void st7565_showBitmap(uint8_t* bmp,uint16_t size, uint8_t x, uint8_t line);
#endif /* ST7565_H_ */
