/*
 * i2c_reg.h
 *
 * Created: 7/17/2014 8:30:29 AM
 *  Author: ahmed
 */ 


#ifndef I2C_REG_H_
#define I2C_REG_H_
#include "i2c_master.h"

uint8_t I2CRegRead(struct i2c_master_module*,uint16_t,uint8_t);
uint8_t I2CRegWrite(struct i2c_master_module*,uint16_t,uint8_t,uint8_t);

#endif /* I2C_REG_H_ */