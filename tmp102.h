/*
 * tmp102.h
 *
 * Created: 14.07.2021 18:07:34
 *  Author: bursztyn
 */ 

#include <avr/io.h>

#ifndef TMP102_H_
#define TMP102_H_

#define GND							0
#define VCC							1
#define SDA							2
#define SCL							3

#define TEMPERATURE_REGISTER		0x00
#define CONFIGURATION_REGISTER		0x01
#define T_LOW_REGISTER				0x02
#define T_HIGH_REGISTER				0x03

#define ADDRESS_GND					0x90
#define ADDRESS_VCC					0x92
#define ADDRESS_SDA					0x94
#define ADDRESS_CLK					0x96

#define	TMP102_IDLE					0
#define TMP102_BUSY					1

#define CONST						0.0625

uint8_t tmp102_init(uint8_t a0_conn);
float tmp102_get_temp();

struct tmp102{
	uint8_t address;
	uint8_t b_temp[2];
};

#endif /* TMP102_H_ */