/*
 * println.c
 *
 * Created: 07.07.2021 09:47:48
 *  Author: bursztyn
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "println.h"
#include "usart.h"

uint8_t nl = '\n';
uint8_t neg = '-';
uint8_t dot = '.';

uint8_t b[PRINTLN_SIZE];
uint8_t b_idx = PRINTLN_SIZE;

void println_msg(char* msg){
	while(*msg != '\0'){
		
		setup_usart((uint8_t*)msg, 1, 0, 0, TX);
		
		msg++;
	}
	setup_usart(&nl, 1, 0, 0, TX);
}

void print_msg(char* msg){
	while(*msg != '\0'){
		
		setup_usart((uint8_t*)msg, 1, 0, 0, TX);
		
		msg++;
	}
}

void println_num(long num){
	if (num < 0){
		num *= -1;
		setup_usart(&neg, 1, 0, 0, TX);
	}
	while(num > 0){
		uint8_t d = num % 10;
		d +=  d > 9 ? 'A' - 10 : '0';
		--b_idx;
		b[b_idx] = d;
		num /= 10;
	}
	setup_usart(&b[b_idx], PRINTLN_SIZE-b_idx, 0, 0, TX);
	setup_usart(&nl, 1, 0, 0, TX);
	b_idx = PRINTLN_SIZE;
}

void print_num(long num){
	if (num < 0){
		num *= -1;
		setup_usart(&neg, 1, 0, 0, TX);
	}
	while(num > 0){
		uint8_t d = num % 10;
		d +=  d > 9 ? 'A' - 10 : '0';
		--b_idx;
		b[b_idx] = d;
		num /= 10;
	}
	setup_usart(&b[b_idx], PRINTLN_SIZE-b_idx, 0, 0, TX);
	b_idx = PRINTLN_SIZE;
}

void println_flo(float flo){
	if (flo < 0){
		flo *= -1;
		setup_usart(&neg, 1, 0, 0, TX);
	}
	long num = (long) flo;
	float frac = flo - num;
	print_num(num);
	setup_usart(&dot, 1, 0, 0, TX);
	num = frac * 100;
	println_num(num);
}
