/*
 * i2c_reg.c
 *
 * Created: 7/17/2014 8:31:02 AM
 *  Author: ahmed
 */ 

#include "i2c_reg.h"

uint8_t I2CRegRead(struct i2c_master_module* module,uint16_t deviceAddr,uint8_t regAddr)
{
	struct i2c_master_packet packet;
	uint8_t localBuff = regAddr;
	packet.address = deviceAddr;
	packet.data = &localBuff;
	packet.data_length = 1;
	packet.high_speed = false;
	packet.hs_master_code = 0x0;
	packet.ten_bit_address = false;
	enum status_code tmpStatus = STATUS_OK;
	tmpStatus = i2c_master_write_packet_wait_no_stop(module,&packet);
	tmpStatus = i2c_master_read_packet_wait(module,&packet);
	return localBuff;
}

uint8_t I2CRegWrite(struct i2c_master_module* module,uint16_t deviceAddr,uint8_t regAddr,uint8_t data)
{
	struct i2c_master_packet packet;
	uint8_t localBuff[2] = {0};
	localBuff[0] = regAddr;
	localBuff[1] = data;
	packet.address = deviceAddr;
	packet.data = localBuff;
	packet.data_length = 1;
	packet.high_speed = false;
	packet.hs_master_code = 0x0;
	packet.ten_bit_address = false;
	enum status_code  tmpStatus = STATUS_OK;
	tmpStatus = i2c_master_write_packet_wait(module,&packet);
	return localBuff[0];
}