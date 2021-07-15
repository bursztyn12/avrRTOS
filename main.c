/*
 * avr_kernel.c
 *
 * Created: 05.07.2021 13:32:20
 * Author : bursztyn
 */ 

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "println.h"
#include "kernel.h"
#include "sds011.h"
#include "tmp102.h"

void idle_0(){
	PORTA ^= (1<<1);
}

void idle_1(){
	PORTA ^= (1<<2);
}

void idle_2(){
	while(1){
		for(uint8_t i=0;i<10;i++){
			println("P2: @", i);
		}
	}
}

void idle_3(){
	sds011_sleep();
}

void idle_4(){
	sds011_measure();
	//sds011_process_measurment();
	sds011_sleep();
}

void tmp(){
	tmp102_init(GND);
	tmp102_get_temp();
}

int main(void){
	//DDRA = 0xFF;
	//_delay_ms(10000);
	init_kernel();
	
	//create_task(idle_, SINGLE, 0);
	create_task(idle_1, PERODIC, 20);
	create_task(tmp, SINGLE, 100);
	
	start_kernel();
}

