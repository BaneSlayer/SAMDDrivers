/*
 * lm75.h
 *
 * Created: 7/17/2014 9:06:25 AM
 *  Author: ahmed
 */ 


#ifndef LM75_H_
#define LM75_H_
#include "i2c_master.h"

void lm75_init(void);
float lm75_getTempC(struct i2c_master_module*,uint16_t);
float lm75_getTempF(struct i2c_master_module*,uint16_t);

#endif /* LM75_H_ */