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

uint8_t r3 = 0;
uint8_t r2 = 0;
uint8_t r1 = 0;

void println(char* msg, uint8_t n){
	cli();
	while(*msg != '\0'){
		if (*msg == '@'){
			if (n > 99){
				r3 = n / 100;
				r1 = n % 10;
				r2 = n/10;
				r2 = r2 % 10;
				usart_hex(r3);
				usart_hex(r2);
				usart_hex(r1);
			}else if(n > 9){
				uint8_t r1 = n % 10;
				uint8_t r2 = n/10;
				usart_hex(r2);
				usart_hex(r1);
				}else{
				usart_hex(n);
			}
			}else{
			usart_write(*msg);
		}
		msg++;
	}
	msg = 0;
	usart_write('\n');
	sei();
} 
