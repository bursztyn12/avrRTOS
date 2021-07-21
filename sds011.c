/*
 * sds011.c
 *
 * Created: 08.07.2021 13:44:33
 *  Author: bursztyn
 */ 
#include <stddef.h>
#include "sds011.h"
#include "usart.h"
#include "kernel.h"
#include "println.h"
#include "mutex.h"

static struct mutex mtx;

uint8_t sds011_state = SDS011_IDLE;
uint8_t sds011_status = SDS011_WORK;

uint8_t new_measurment_available = 0;

uint8_t m_sleep[19] = {0xAA, 0xB4, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0xAB};
uint8_t m_measure[19] = {0xAA, 0xB4, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x02, 0xAB};
uint8_t m_work[19] = {0xAA, 0xB4, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xFF, 0x06, 0xAB};

uint8_t a_response[10] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
	
measurment_t m = {
	.pm2_5 = 0,
	.pm10 = 0
};

struct tcb* tcb;

void sds011_sleep(){	
	if (sds011_state == SDS011_BUSY){
		//put task to blocked queue
		task_block(SDS011_BLOCKED, &mtx);
	}
	
	sds011_state = SDS011_BUSY;
	
	tcb = get_current_tcb();
	
	setup_usart(m_sleep, 18, 0, 0, TX);
	
	sds011_status = SDS011_SLEEP;
	sds011_state = SDS011_IDLE;
}

void sds011_measure(){
	tcb = get_current_tcb();
	if (sds011_state == SDS011_BUSY){
		//put task to blocked queue
		task_block(SDS011_BLOCKED, &mtx);
	}
	
	tcb->state = SUSPENDED;
	
	sds011_state = SDS011_BUSY;
	
	setup_usart(m_measure, 18, a_response, 9, TX_RX);
	
	sds011_status = SDS011_MEASURE;
	sds011_state = SDS011_IDLE;
	
	new_measurment_available = 1;
}

void sds011_work(){
	tcb = get_current_tcb();
	if (sds011_state == SDS011_BUSY){
		//put task to blocked queue
		task_block(SDS011_BLOCKED, &mtx);
	}
	
	sds011_state = SDS011_BUSY;
	
	setup_usart(m_work, 18, 0, 0, TX);
	
	sds011_status = SDS011_WORK;
	sds011_state = SDS011_IDLE;
}

measurment_t* sds011_process_measurment(){
	uint8_t pm2_5h = a_response[3];
	uint8_t pm2_5l = a_response[2];
	uint8_t pm10h = a_response[5];
	uint8_t pm10l = a_response[4];

	float pm2_5v = ((pm2_5h*256) + pm2_5l)/10;
	float pm10v = ((pm10h*256) + pm10l)/10;
	
	m.pm2_5 = pm2_5v;
	m.pm10 = pm10v;
	
	println_flo(pm2_5v);
	println_flo(pm10v);
	
	return &m;
}