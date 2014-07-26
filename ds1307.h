/*
 * ds1206.h
 *
 * Created: 7/17/2014 8:04:03 AM
 *  Author: ahmed
 */ 


#ifndef DS1206_H_
#define DS1206_H_
#include "i2c_master.h"

typedef struct {
	uint8_t Day;
	uint8_t Month;
	uint16_t Year;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
} DateTime;

uint8_t bcd2bin(uint8_t);
uint8_t bin2bcd(uint8_t);
void ds1307_init(struct i2c_master_module*,uint16_t);
void ds1307_adjust(struct i2c_master_module*,uint16_t,DateTime*);
void ds1307_now(struct i2c_master_module*,uint16_t,DateTime*);

#endif /* DS1206_H_ */