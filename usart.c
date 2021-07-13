/*
 * usart.c
 *
 * Created: 07.07.2021 09:45:13
 *  Author: bursztyn
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 1000000UL

#include <util/delay.h>

#include "usart.h"
#include "kernel.h"

tcb_t* tcb;
volatile uint8_t tx_status = 1;
volatile uint8_t rx_status = 1;
volatile uint8_t* tx_buffer;
volatile uint8_t* rx_buffer;
volatile uint8_t tx_b_idx = 0;
volatile uint8_t rx_b_idx = 0;

uint8_t tx_reserve_size = 0;
uint8_t rx_reserve_size = 0;

uint8_t usart_state = USART_IDLE;

uint8_t c_reserve_type = RX;

void enable_rxcie(){
	UCSRB |= (1 << RXCIE);
}

void disable_rxcie(){
	UCSRB &= ~(1 << RXCIE);
}

uint8_t get_usart_state(){
	return usart_state;
}

void setup_usart(uint8_t *tx_b, uint8_t tx_size, uint8_t *rx_b, uint8_t rx_size, uint8_t type){
	tx_buffer = tx_b;
	tx_reserve_size = tx_size;
	
	rx_buffer = rx_b;
	rx_reserve_size = rx_size;
	
	c_reserve_type = type;
	
	if (type == RX){
		usart_rx();
	}else if(type == TX || type == TX_RX){
		usart_tx(type);
	}
}

void usart_tx(uint8_t type){
	tcb = get_current_tcb();
	if (usart_state == USART_BUSY){
		// put to blocked queue
		block_task(USART_BLOCKED);
	}
	
	usart_state = USART_BUSY;
	
	UDR = *tx_buffer;
	
	UCSRB |= (1 << UDRIE);
	
	if (type == TX){
		while(tx_status);
	}else{
		while(tx_status);
		while(rx_status);
	}
	
	tx_status = 1;
	rx_status = 1;
	
	tx_b_idx = 0;
	rx_b_idx = 0;
	
	usart_state = USART_IDLE;
}

void usart_rx(){
	tcb = get_current_tcb();
	if(usart_state == USART_BUSY){
		block_task(USART_BLOCKED);
	}
	enable_rxcie();
	while(rx_status);
	disable_rxcie();
}

void usart_write(uint8_t data){
	while(!(UCSRA & (1 << UDRE)));
	
	UDR = data;
}

void usart_hex(uint8_t b){
	b +=  b > 9 ? 'A' - 10 : '0';
	usart_write(b);
}

void usart_init(){
	UBRRH = (unsigned char)(BAUD >> 8);
	UBRRL = (unsigned char) BAUD;
	
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
	UCSRA = (1 << U2X);
}

ISR(USART_UDRE_vect){
	UCSRB &= ~(1 << UDRIE);
	
	if(tx_b_idx == tx_reserve_size){
		tx_status = 0;
	}else{
		++tx_b_idx;
		++tx_buffer;
		
		UDR = *tx_buffer;
		
		UCSRB |= (1 << UDRIE);
	}
}

ISR(USART_RXC_vect){
	if (c_reserve_type != RX){
		if (rx_b_idx == rx_reserve_size){
			rx_status = 0;
		}else{
			*rx_buffer = UDR;
			++rx_b_idx;
			++rx_buffer;
		}
	}else{
		uint8_t tmp __attribute__((unused)) = UDR;
	}
}