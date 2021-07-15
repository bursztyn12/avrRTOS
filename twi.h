/*/*
 * twi.h
 *
 * Created: 13.07.2021 13:02:11
 *  Author: bursztyn
 */ 


#ifndef TWI_H_
#define TWI_H_

//master actions
#define TWI_START					0
#define TWI_STOP					1
#define TWI_STOP_START				2
#define TWI_WRITE_PAYLOAD			3
#define TWI_ADDRESS_W				4
#define TWI_ADDRESS_R				5
#define TWI_READ_PAYLOAD			6
#define TWI_RP_START				7

#define TWI_SUCCESS					0

//twi status codes for master tx
#define TWI_STA_TX					0x08 //A START condition has been transmitted.
#define TWI_RP_STA_TX				0x10 //A repeated START condition has been transmitted.
#define TWI_SLA_W_TX_ACK			0x18 //SLA+W has been transmitted, ACK has been received.
#define TWI_SLA_W_TX_NACK			0x20 //SLA+W has been transmitted; NOT ACK has been received.
#define TWI_PACKET_TX_ACK			0x28 //Data byte has been transmitted; ACK has been received.
#define TWI_PACKET_TX_NACK			0x30 //Data byte has been transmitted; NACK has been received.
#define TWI_ARB_LOST				0x38 //Arbitration lost in SLA+W or data bytes.

//twi status codes for master rx
#define TWI_SLA_R_TX_ACK			0x40 //SLA+R has been transmitted; ACK has been received
#define TWI_SLA_R_TX_NACK			0x48 //SLA+R has been transmitted; NACK has been received
#define TWI_PACKET_RX_ACK			0x50 //Data byte has been received; ACK has been returned
#define TWI_PACKET_RX_NACK			0x58 //Data byte has been received; NACK has been returned

#define TWI_IDLE					0
#define TWI_BUSY					1

//twi modes
#define SINGLE_BYTE_READ			0
#define MULTIPLE_BYTE_READ			1
#define SINGLE_BYTE_WRITE			2
#define MULTIPLE_BYTE_WRITE			3

struct twi_packet{
	uint8_t address;
	uint8_t *tx_buffer;
	uint8_t *rx_buffer;
	uint8_t tx_length;
	int8_t rx_length;
	uint8_t tx_idx;
	uint8_t rx_idx;
	uint8_t error_code;
	uint8_t is_error;
};


void twi_init();
void twi_start();
void twi_setup(uint8_t address, uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t tx_length, uint8_t rx_length, uint8_t mode);

#endif /* TWI_H_ */