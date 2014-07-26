/*
 * lm75.c
 *
 * Created: 7/17/2014 9:09:32 AM
 *  Author: ahmed
 */ 
#include "lm75.h"
#include "i2c_reg.h"

void lm75_init(void) {
}

float lm75_getTempF(struct i2c_master_module* module,uint16_t address) {
	float fahrenheit = (1.8 * lm75_getTempC(module, address)) + 32;
	return fahrenheit;
}

float lm75_getTempC(struct i2c_master_module* module,uint16_t address) {
	uint8_t MSB = I2CRegRead(module, address, 0);
	uint8_t LSB = I2CRegRead(module, address, 0);

	int TemperatureSum = ((MSB << 8) | LSB) >> 4;

	float celsius = TemperatureSum * 0.0625;
	return celsius;
}