/*
 * twi.c
 *
 * Created: 13.07.2021 13:07:22
 *  Author: bursztyn
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "twi.h"

uint8_t twi_master_action = TWI_START;
struct twi_packet twi_packet;

void twi_start(){
	TWCR &= ~(1 << TWSTO);
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
}

void twi_setup(uint8_t address, uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t tx_length, uint8_t rx_length){
	TWSR = 0x00;
	TWBR = 0x02;
	
	twi_packet.address = address;
	twi_packet.tx_buffer = tx_buffer;
	twi_packet.tx_length = tx_length;
	twi_packet.rx_buffer = rx_buffer;
	twi_packet.rx_length = rx_length;
	twi_packet.tx_idx = 0;
	twi_packet.rx_idx = 0;
	
	twi_start();
}

ISR(TWI_vect){
	uint8_t status = TWSR & 0xF8;
	
	if(status == TWI_STA_TX || status == TWI_RP_STA_TX){
		//enter MT mode write SLA + W to TWDR
		
		TWCR &= ~(1 << TWSTA);
		TWCR &= ~(1 << TWSTO);
		
		if (twi_master_action == TWI_START){
			twi_packet.address &= ~(0 << 0);
			twi_master_action = TWI_ADDRESS_W;
		}else{
			twi_packet.address |= (1 << 0);
			twi_master_action = TWI_ADDRESS_R;
		}
		
		TWDR = twi_packet.address;
		TWCR = (1 << TWINT) | (1 << TWEN);
	}else if (status == TWI_SLA_W_TX_ACK){
		//load data byte
		TWDR = *(twi_packet.tx_buffer);
		
		++twi_packet.tx_buffer;
		++twi_packet.tx_idx;
		
		TWCR &= ~(1 << TWSTA);
		TWCR &= ~(1 << TWSTO);
		TWCR = (1 << TWINT) | (1 << TWEN);
	}else if(status == TWI_PACKET_TX_ACK){
		if (twi_packet.tx_idx  == twi_packet.tx_length){
			//reapeted start
			twi_master_action = TWI_RP_START;
			TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWEN);
		}else{
			TWDR = *(twi_packet.tx_buffer);
			
			++twi_packet.tx_buffer;
			++twi_packet.tx_idx;
			
			TWCR &= ~(1 << TWSTA);
			TWCR &= ~(1 << TWSTO);
			TWCR = (1 << TWINT) | (1 << TWEN);
		}
	}else if (status == TWI_SLA_R_TX_ACK){
		TWCR &= ~(1 << TWSTA);
		TWCR &= ~(1 << TWSTO);
		TWCR = (1 << TWINT) | (1 << TWEN);
	}else if(status == TWI_PACKET_RX_ACK){
		*(twi_packet.rx_buffer) = TWDR;
		if (twi_packet.rx_idx == twi_packet.rx_length){
			TWCR &= ~(1 << TWSTA);
			TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
		}else{
			++twi_packet.rx_buffer;
			++twi_packet.rx_idx;
			TWCR &= ~(1 << TWSTA);
			TWCR &= ~(1 << TWSTO);
			TWCR = (1 << TWINT) | (1 << TWEN);
		}
	}
}