/*
* st7565.c
*
*  Created on: May 17, 2014
*      Author: ahmed
*/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "st7565.h"
#include "asf.h"
#include "st7565_font.h"
#include "startup_logo.h"

uint8_t is_reversed = 1;

int pagemap[] = { 7, 6, 5, 4, 3, 2, 1, 0 };

uint8_t tmpBuffer[1];

uint8_t displayBuffer[LCD_BUFFER_SIZE];
struct spi_module			lcd_module;
struct spi_slave_inst		slave;
void st7565_sendCmd(uint8_t data);
void st7565_sendData(uint8_t* data, int count);

void st7565_init(void) {
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
	/* Configure and initialize software device instance of peripheral slave */
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = SLAVE_SELECT_PIN;
	spi_attach_slave(&slave, &slave_dev_config);
	/* Configure, initialize and enable SERCOM SPI module */
	spi_get_config_defaults(&config_spi_master);
	config_spi_master.mode = SPI_MODE_MASTER;
	config_spi_master.data_order = SPI_DATA_ORDER_MSB;
	config_spi_master.character_size = SPI_CHARACTER_SIZE_8BIT;
	config_spi_master.generator_source = GCLK_GENERATOR_0;
	config_spi_master.mode_specific.master.baudrate = 50000;
	
	config_spi_master.mux_setting = SPI_SIGNAL_MUX_SETTING_E;
	/* Configure pad 0 for data in */
	config_spi_master.pinmux_pad0 = PINMUX_UNUSED;
	/* Configure pad 1 as unused */
	config_spi_master.pinmux_pad1 = PINMUX_UNUSED;
	/* Configure pad 2 for data out */
	config_spi_master.pinmux_pad2 = PINMUX_PA10D_SERCOM2_PAD2;
	/* Configure pad 3 for SCK */
	config_spi_master.pinmux_pad3 = PINMUX_PA11D_SERCOM2_PAD3;
	spi_init(&lcd_module, SERCOM2, &config_spi_master);
	spi_enable(&lcd_module);
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(RESET_PIN,&config_port_pin);
	port_pin_set_config(DATA_PIN,&config_port_pin);
	//port_pin_set_config(SLAVE_SELECT_PIN,&config_port_pin);
	//port_pin_set_output_level(SLAVE_SELECT_PIN,true); 
	// Reset
	port_pin_set_output_level(RESET_PIN,false);   // Set reset low
	delay_ms(500);             // Wait 500mS
	port_pin_set_output_level(RESET_PIN,true);    // Set reset high
	//delayms(5);
	// Configure Display
	st7565_sendCmd(ST7565_CMD_SET_BIAS_7);                    // LCD Bias Select
	st7565_sendCmd(ST7565_CMD_SET_ADC_NORMAL);                    // ADC Select
	st7565_sendCmd(ST7565_CMD_SET_COM_REVERSE);                    // SHL Select
	st7565_sendCmd(ST7565_CMD_SET_DISP_START_LINE);      // Initial Display Line
	st7565_sendCmd(ST7565_CMD_SET_POWER_CONTROL | 0x04); // Turn on voltage converter (VC=1, VR=0, VF=0)
	delay_ms(50);                 // Wait 50mS
	st7565_sendCmd(ST7565_CMD_SET_POWER_CONTROL | 0x06); // Turn on voltage regulator (VC=1, VR=1, VF=0)
	delay_ms(50);                   // Wait 50mS
	st7565_sendCmd(ST7565_CMD_SET_POWER_CONTROL | 0x07); // Turn on voltage follower
	delay_ms(50);                  // Wait 10mS
	st7565_sendCmd(ST7565_CMD_SET_RESISTOR_RATIO | 0x6); // Set LCD operating voltage

	st7565_sendCmd(ST7565_CMD_SET_DISP_NORMAL);
	// Turn display on
	st7565_sendCmd(ST7565_CMD_DISPLAY_ON);

	st7565_sendCmd(ST7565_CMD_SET_ALLPTS_NORMAL);

	st7565_sendCmd(ST7565_CMD_SET_PAGE);

	st7565_sendCmd(ST7565_CMD_SET_COLUMN_UPPER);
	st7565_sendCmd(ST7565_CMD_SET_COLUMN_LOWER);

	st7565_sendCmd(ST7565_CMD_SET_VOLUME_FIRST);
	st7565_sendCmd(ST7565_CMD_SET_VOLUME_SECOND);

	st7565_clear();
}

void st7565_sendCmd(uint8_t data) {
	//port_pin_set_output_level(SLAVE_SELECT_PIN,false);
	port_pin_set_output_level(DATA_PIN,false);
	spi_select_slave(&lcd_module, &slave, true);
	spi_write_buffer_wait(&lcd_module, &data, 1);
	spi_select_slave(&lcd_module, &slave, false);
	//port_pin_set_output_level(SLAVE_SELECT_PIN,true);
}

void st7565_sendData(uint8_t* data, int count) {
	//port_pin_set_output_level(SLAVE_SELECT_PIN,false);
	port_pin_set_output_level(DATA_PIN,true);
	spi_select_slave(&lcd_module, &slave, true);
	spi_write_buffer_wait(&lcd_module, data, count);
	spi_select_slave(&lcd_module, &slave, false);
	//port_pin_set_output_level(SLAVE_SELECT_PIN,true);
}

void st7565_flush(void) {
	unsigned char p;
	for (p = 0; p < 8; p++) {
		st7565_sendCmd(ST7565_CMD_SET_PAGE | pagemap[p]);
		st7565_sendCmd(ST7565_CMD_SET_COLUMN_LOWER);
		st7565_sendCmd(ST7565_CMD_SET_COLUMN_UPPER);
		st7565_sendCmd(ST7565_CMD_RMW);
		st7565_sendData(&displayBuffer[(128 * p)], 128);
	}
}

void st7565_clear(void) {
	memset(displayBuffer, 0x00, LCD_BUFFER_SIZE);
	st7565_flush();
}

void st7565_setBrightness(uint8_t val) {
	st7565_sendCmd(ST7565_CMD_SET_VOLUME_FIRST);
	st7565_sendCmd(ST7565_CMD_SET_VOLUME_SECOND | (val & 0x3f));
}

void st7565_drawPixel(uint8_t x, uint8_t y) {
	if ((x >= 128) || (y >= 64))
	return;
	// x is which column
	displayBuffer[x + (y / 8) * 128] |= (1 << (7 - (y % 8)));
}

void st7565_clearPixel(uint8_t x, uint8_t y) {
	if ((x >= 128) || (y >= 64))
	return;

	// x is which column
	displayBuffer[x + (y / 8) * 128] &= ~(1 << (7 - (y % 8)));
}
void st7565_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	uint8_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int8_t err = dx / 2;
	int8_t ystep;

	if (y0 < y1) {
		ystep = 1;
		} else {
		ystep = -1;
	}

	for (; x0 < x1; x0++) {
		if (steep) {
			st7565_drawPixel(y0, x0);
			} else {
			st7565_drawPixel(x0, y0);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void st7565_drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
	uint8_t y1, x1;
	y1 = y + h - 1;
	x1 = x + w - 1;
	st7565_drawLine(x, y, x1, y);
	st7565_drawLine(x1, y1, x1, y);
	st7565_drawLine(x, y, x, y1);
	st7565_drawLine(x, y1, x1, y1);
}

void st7565_drawChar(uint8_t x, uint8_t line, char c) {
	uint8_t i;
	for (i = 0; i < 5; i++) {
		displayBuffer[x + (line * 128)] = FONT57[(c * 5) + i];
		x++;
	}
}

void st7565_drawString(uint8_t x, uint8_t line, char *c) {
	while (c[0] != 0) {
		st7565_drawChar(x, line, c[0]);
		c++;
		x += 6; // 6 pixels wide
		if (x + 6 >= LCDWIDTH) {
			x = 0;    // ran out of this line
			line++;
		}
		if (line >= (LCDHEIGHT / 8))
		return;        // ran out of space :(
	}
}

void st7565_showBitmap(uint8_t* bmp,uint16_t size, uint8_t x, uint8_t line) {
	memcpy(displayBuffer, bmp, size);
	st7565_flush();
}

void st7565_showLogo(void) {
	st7565_showBitmap(logo_buffer,1024, 0, 0);
}

