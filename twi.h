/*
 * twi.h
 *
 * Created: 13.07.2021 13:02:11
 *  Author: bursztyn
 */ 


#ifndef TWI_H_
#define TWI_H_

void twi_write_address(uint8_t address);
void twi_write_packet(uint8_t packet);
uint8_t twi_read_packet();
void twi_stop_start();
void twi_stop();
void start_twi();
void twi_init();

#endif /* TWI_H_ */