/*
 * twi.c
 *
 * Created: 13.07.2021 13:07:22
 *  Author: bursztyn
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "twi.h"
#include "kernel.h"

struct twi_packet twi_packet;
static struct tcb *twi_tcb;

//volatile uint8_t flag = 1;
uint8_t twi_mode = SINGLE_BYTE_READ;
uint8_t twi_state = TWI_IDLE;
uint8_t twi_master_action = TWI_START;

void twi_init(){
	TWSR = 0x00;
	TWBR = 0x02;
}

void twi_start(){
	sei();
	
	twi_tcb->w_state = WORK_S;
	
	TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWSTA);
	
	task_suspend();
	
	TWCR = 0x00;
}

void twi_setup(uint8_t address, uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t tx_length, uint8_t rx_length, uint8_t mode){
	if (twi_state == TWI_BUSY){
		// put to blocked queue
		task_block(TWI_BLOCKED);
	}
	
	twi_state = TWI_BUSY;
	
	twi_tcb = get_current_tcb();
	
	twi_packet.address = address;
	twi_packet.tx_buffer = tx_buffer;
	twi_packet.tx_length = tx_length;
	twi_packet.rx_buffer = rx_buffer;
	twi_packet.rx_length = rx_length;
	twi_packet.tx_idx = 0;
	twi_packet.rx_idx = 0;
	
	twi_mode = mode;
	
	twi_start();
	
	twi_state = TWI_IDLE;
}

ISR(TWI_vect){
	uint8_t status = TWSR & 0xF8;
	
	twi_packet.is_error = TWI_SUCCESS;
	
	if(status == TWI_STA_TX || status == TWI_RP_STA_TX){
		//enter MT mode write SLA + W to TWDR	
		if (twi_master_action == TWI_START){
			twi_master_action = TWI_ADDRESS_W;
		}else{
			twi_packet.address += 1;
			twi_master_action = TWI_ADDRESS_R;
		}
		
		TWDR = twi_packet.address;
		TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT);
	}else if (status == TWI_SLA_W_TX_ACK){
		//load data byte
		TWDR = *(twi_packet.tx_buffer);
		
		++twi_packet.tx_buffer;
		++twi_packet.tx_idx;
		
		TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT);
	}else if(status == TWI_PACKET_TX_ACK){
		if (twi_packet.tx_idx == twi_packet.tx_length){
			if (twi_mode == SINGLE_BYTE_WRITE || twi_mode == MULTIPLE_BYTE_WRITE){
				//stop
				TWCR = (1 << TWEN) | (1 << TWSTO) | (1 << TWINT);
				uart_tcb->w_state = WORK_F;
				task_notify(twi_tcb);
			}else{
				//reapeted start
				twi_master_action = TWI_RP_START;
				TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWSTA) | (1 << TWINT);
			}
		}else{
			TWDR = *(twi_packet.tx_buffer);
			
			++twi_packet.tx_buffer;
			++twi_packet.tx_idx;
			
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT);
		}
	}else if (status == TWI_SLA_R_TX_ACK){
		TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
	}else if(status == TWI_PACKET_RX_ACK){
		*(twi_packet.rx_buffer) = TWDR;
		++twi_packet.rx_buffer;
		++twi_packet.rx_idx;
		if (twi_packet.rx_idx == twi_packet.rx_length){
			PORTA |= (1 << 0);
			TWCR = (1 << TWEN) | (1 << TWSTO) | (1 << TWINT);
			uart_tcb->w_state = WORK_F;
			task_notify(twi_tcb);
		}else{
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
		}
	}else{
		twi_packet.is_error = 1;
		twi_packet.error_code = status;
	}
}
