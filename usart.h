/*
 * usart.h
 *
 * Created: 07.07.2021 09:40:16
 *  Author: bursztyn
 */ 
#include <avr/io.h>

#ifndef USART_H_
#define USART_H_

#define BAUD				12
#define BUFFER_SIZE			20

#define USART_IDLE			0
#define USART_BUSY			1

#define TX					0
#define RX					1
#define TX_RX				2

void usart_tx(uint8_t type);
void usart_rx();
void usart_write(uint8_t data);
void usart_hex(uint8_t b);
void usart_init();
void setup_usart(uint8_t *tx_b, uint8_t tx_size, uint8_t *rx_b, uint8_t rx_size, uint8_t type);
uint8_t get_usart_state();

#endif /* USART_H_ */