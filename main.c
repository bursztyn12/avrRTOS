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

int main(void){
	//_delay_ms(3000);
	init_kernel();
	
	create_task(idle_0, PERODIC, 70);
	create_task(idle_1, PERODIC, 15);
	
	start_kernel();
}

