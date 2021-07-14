/*
 * tmp102.c
 *
 * Created: 14.07.2021 18:07:48
 *  Author: bursztyn
 */ 

#include "tmp102.h"
#include "twi.h"

struct tmp102 tmp102;

uint16_t temp_v;

uint8_t tmp102_init(uint8_t address){
	if (address < 0x48 && address > 0x4B){
		return 1;
	}
	tmp102.address = address;
	return 0;
}

void tmp102_write(uint8_t reg){
	
}

void tmp102_read(uint8_t reg){
	uint8_t tx_buffer[2] = {tmp102.address, reg};
	twi_setup(tmp102.address, tx_buffer, tmp102.b_temp, 2, 2);
}

void tmp102_get_temp(){
	tmp102_read(TEMPERATURE_REGISTER);
}