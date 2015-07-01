/*
* mpr121.c
*
* Created: 6/10/2015 9:05:19 PM
*  Author: Tanveer
*/
#include "mpr121.h"
#include <delay.h>

#define TIMEOUT 1000
#define DATA_LENGTH 2
static uint8_t buffer[DATA_LENGTH] = {0};uint16_t timeout = 0;

bool mpr121_init(struct i2c_master_module *module, uint8_t address)
{
	mpr121_writeRegister(module,address,MPR121_SOFTRESET,0x63);
	delay_ms(1);
	
	//if(mpr121_readRegister8(module,address,MPR121_SOFTRESET) != 0x63)
	//return false;
	
	mpr121_writeRegister(module,address,MPR121_ECR,0x0);
	
	if(mpr121_readRegister8(module,address,MPR121_CONFIG2) != 0x24)
	return false;
	
	mpr121_setThreshold(module,address,12,6);
	
	mpr121_writeRegister(module,address,MPR121_MHDR, 0x01);
	mpr121_writeRegister(module,address,MPR121_NHDR, 0x01);
	mpr121_writeRegister(module,address,MPR121_NCLR, 0x0E);
	mpr121_writeRegister(module,address,MPR121_FDLR, 0x00);
	
	mpr121_writeRegister(module,address,MPR121_MHDF, 0x01);
	mpr121_writeRegister(module,address,MPR121_NHDF, 0x05);
	mpr121_writeRegister(module,address,MPR121_NCLF, 0x01);
	mpr121_writeRegister(module,address,MPR121_FDLF, 0x00);
	
	mpr121_writeRegister(module,address,MPR121_NHDT, 0x00);
	mpr121_writeRegister(module,address,MPR121_NCLT, 0x00);
	mpr121_writeRegister(module,address,MPR121_FDLT, 0x00);
	
	mpr121_writeRegister(module,address,MPR121_DEBOUNCE, 0);
	mpr121_writeRegister(module,address,MPR121_CONFIG1, 0x10); // default, 16uA charge current
	mpr121_writeRegister(module,address,MPR121_CONFIG2, 0x20); // 0.5uS encoding, 1ms period
	mpr121_writeRegister(module,address,MPR121_ECR, 0x8F);  // start with first 5 bits of baseline tracking
	
	return true;
}

uint8_t mpr121_readRegister8(struct i2c_master_module *module, uint8_t address, uint8_t registerAddress)
{
	timeout = 0;
	uint8_t localBuff = registerAddress;
	struct i2c_master_packet packet = {
		.address = address,
		.data_length = 1,
		.data = &localBuff,
		.ten_bit_address = false,
		.high_speed = false,
		.hs_master_code = 0x0,
	};		while(i2c_master_write_packet_wait_no_stop((struct i2c_master_module *const)module,&packet)!= STATUS_OK)	{		if (timeout++ == TIMEOUT) {
			break;
		}	}		while(i2c_master_read_packet_wait((struct i2c_master_module *const)module,&packet)!= STATUS_OK)	{		if (timeout++ == TIMEOUT) {
			break;
		}	}		return localBuff;
}

uint16_t mpr121_readRegister16(struct i2c_master_module *module, uint8_t address, uint8_t registerAddress)
{
	timeout = 0;
	uint8_t localBuff = registerAddress;
	struct i2c_master_packet packet = {
		.address = address,
		.data_length = 1,
		.data = &localBuff,
		.ten_bit_address = false,
		.high_speed = false,
		.hs_master_code = 0x0,
	};		while(i2c_master_write_packet_wait_no_stop((struct i2c_master_module *const)module,&packet)!= STATUS_OK)	{		if (timeout++ == TIMEOUT) {
			break;
		}	}		packet.data = buffer;	packet.data_length = 2;		while(i2c_master_read_packet_wait((struct i2c_master_module *const)module,&packet)!= STATUS_OK)	{		if (timeout++ == TIMEOUT) {
			break;
		}	}		uint16_t v =packet.data[0];
	v |=  ((uint16_t) packet.data[1]) << 8;
	return v;
}

void mpr121_writeRegister(struct i2c_master_module *module, uint8_t address, uint8_t registerAddress, uint8_t data)
{
	timeout = 0;
	buffer[0] = registerAddress;
	buffer[1] = data;
	struct i2c_master_packet packet = {
		.address = address,
		.data_length = 2,
		.data = buffer,
		.ten_bit_address = false,
		.high_speed = false,
		.hs_master_code = 0x0,
	};		while(i2c_master_write_packet_wait((struct i2c_master_module *const)module,&packet)!= STATUS_OK)	{		if (timeout++ == TIMEOUT) {
			break;
		}	}
}

void mpr121_setThreshold(struct i2c_master_module *module, uint8_t address, uint8_t touchChannel, uint8_t release)
{
	for (uint8_t i=0; i<12; i++) {
		mpr121_writeRegister(module,address, MPR121_TOUCHTH_0 + 2*i, touchChannel);
		mpr121_writeRegister(module,address,MPR121_RELEASETH_0 + 2*i, release);
	}
}

uint16_t mpr121_touched(struct i2c_master_module *module, uint8_t address)
{
	uint16_t t = mpr121_readRegister16(module,address,MPR121_TOUCHSTATUS_L);
	return t & 0x0FFF;
}

uint16_t mpr121_filteredData(struct i2c_master_module *module, uint8_t address,uint8_t data)
{
	if (data > 12) return 0;
	return mpr121_readRegister16(module,address,MPR121_FILTDATA_0L + data*2);
}

uint16_t mpr121_baselineData(struct i2c_master_module *module, uint8_t address,uint8_t data)
{
	if (data > 12) return 0;
	uint16_t bl = mpr121_readRegister16(module,address,MPR121_BASELINE_0 + data);
	return (bl << 2);
}