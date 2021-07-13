/*
 * sds011.h
 *
 * Created: 08.07.2021 13:40:05
 *  Author: bursztyn
 */ 

#include <avr/io.h>

#ifndef SDS011_H_
#define SDS011_H_

//device state
#define SDS011_BUSY			0
#define SDS011_IDLE			1

//device status
#define SDS011_MEASURE		0
#define SDS011_SLEEP		1
#define SDS011_WORK			2

typedef struct measurment_t{
	float pm2_5;
	float pm10;
}measurment_t;

void sds011_sleep();
void sds011_measure();
void sds011_work();
measurment_t* sds011_process_measurment();

#endif /* SDS011_H_ */