/*
 * tmp102.c
 *
 * Created: 14.07.2021 18:07:48
 *  Author: bursztyn
 */ 

#include "tmp102.h"
#include "twi.h"

struct tmp102 tmp102;

uint16_t temp_v = 0;
uint8_t c_reg = 0;

uint8_t tmp102_init(uint8_t address){
	if (address < 0x48 && address > 0x4B){
		return 1;
	}
	tmp102.address = address;
	return 0;
}



float tmp102_get_temp(){
	c_reg = TEMPERATURE_REGISTER;
	twi_setup(tmp102.address, &c_reg, tmp102.b_temp, 1, 2);
	
	temp_v = ((*tmp102.b_temp) << 4) | (*(tmp102.b_temp+1) >> 4);
	return temp_v * CONST;
}