/*
 * avrRTOS.c
 *
 * Created: 15.07.2021 22:12:53
 * Author : bursztyn
 */ 

#include <avr/io.h>
#include "println.h"
#include "kernel.h"
#include "sds011.h"
#include "tmp102.h"

void idle_0(){
	PORTA ^= (1<<1);
}

void idle_1(){
	println_msg("P1");
	println_flo(123.245);
}

void idle_3(){
	sds011_sleep();
}

void idle_4(){
	PORTA ^= (1 << 5);
	sds011_measure();
	sds011_process_measurment();
	sds011_sleep();
}

void tmp(){
	PORTA ^= (1 << 5);
	tmp102_init(GND);
	float f = tmp102_get_temp();
	println_flo(f);
}

int main(void){
	//DDRA = 0xFF;
	//_delay_ms(10000);
	init_kernel();
	
	//create_task(idle_0, PERODIC, 2);
	//create_task(idle_4, PERODIC, 3);
	//create_task(idle_1, PERODIC, 4);
	create_task(tmp, PERODIC, 500);
	
	start_kernel();
}