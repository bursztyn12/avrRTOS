/*
 * tmp102.c
 *
 * Created: 14.07.2021 18:07:48
 *  Author: bursztyn
 */ 

#include "tmp102.h"
#include "twi.h"
#include "kernel.h"

struct tmp102 tmp102;

uint16_t temp_v = 0;
uint8_t c_reg = 0;
uint8_t tmp102_status = TMP102_IDLE;

uint8_t tmp102_init(uint8_t a0_conn){
	if (tmp102_status == TMP102_BUSY){
		task_block(TMP102_BLOCKED);
	}
	
	if (a0_conn < GND && a0_conn > SCL){
		return 1;
	}
	
	if (a0_conn == GND){
		tmp102.address = ADDRESS_GND;
	}else if (a0_conn == VCC){
		tmp102.address = ADDRESS_VCC;
	}else if (a0_conn == SDA){
		tmp102.address = ADDRESS_SDA;
	}else{
		tmp102.address = ADDRESS_CLK;
	}
	
	return 0;
}

float tmp102_get_temp(){
	if (tmp102_status == TMP102_BUSY){
		task_block(TMP102_BLOCKED);
	}
	
	c_reg = TEMPERATURE_REGISTER;
	twi_setup(tmp102.address, &c_reg, tmp102.b_temp, 1, 2, MULTIPLE_BYTE_READ);

	PORTA |= (1 << 6);
	
	temp_v = ((*tmp102.b_temp) << 4) | (*(tmp102.b_temp+1) >> 4);
	
	tmp102_status = TMP102_IDLE;
	
	return temp_v * CONST;
}