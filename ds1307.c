/*
 * ds1206.c
 *
 * Created: 7/17/2014 8:05:09 AM
 *  Author: ahmed
 */ 

#include "ds1307.h"
#include "i2c_reg.h"



uint8_t bcd2bin(uint8_t val) {
	return val - 6 * (val >> 4);
}
uint8_t bin2bcd(uint8_t val) {
	return val + 6 * (val / 10);
}

void ds1307_init(struct i2c_master_module* module,uint16_t deviceAddr)
{
	I2CRegWrite(module,deviceAddr,0,0);
}

void ds1307_now(struct i2c_master_module* module,uint16_t deviceAddr,DateTime* date) {
	
	struct i2c_master_packet packet;
	uint8_t buffer[7];
	packet.address = deviceAddr;
	packet.data = buffer;
	packet.data_length = 7;
	packet.high_speed = false;
	packet.hs_master_code = 0x0;
	packet.ten_bit_address = false;
	enum status_code tmpStatus = STATUS_OK;
	tmpStatus = i2c_master_read_packet_wait(module,&packet);
	
	date->Minute= bcd2bin(buffer[1]);
	date->Hour= bcd2bin(buffer[2]);
	date->Day= bcd2bin(buffer[4]);
	date->Month= bcd2bin(buffer[5]);
	date->Year= bcd2bin(buffer[6])+ 2000;
}

void ds1307_adjust(struct i2c_master_module* module,uint16_t deviceAddr,DateTime* dt){

	I2CRegWrite(module, deviceAddr,0,bin2bcd(dt->Second));
	I2CRegWrite(module, deviceAddr,1,bin2bcd(dt->Minute));
	I2CRegWrite(module, deviceAddr,2,bin2bcd(dt->Hour));
	I2CRegWrite(module, deviceAddr,4,bin2bcd(dt->Day));
	I2CRegWrite(module, deviceAddr,5,bin2bcd(dt->Month));
	I2CRegWrite(module, deviceAddr,6,bin2bcd(dt->Year-2000));

}

