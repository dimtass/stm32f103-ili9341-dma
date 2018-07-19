/*
 * comm_buffer.h
 *
 *  Created on: 10 May 2018
 *      Author: Dimitris Tassopoulos
 */

#ifndef COMM_BUFFER_H_
#define COMM_BUFFER_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief This is a struct that is used for UART comms
 */
////#pragma pack(1)
struct tp_comm_buffer {
	/* tx vars */
	uint8_t 	*tx_buffer;
	size_t		tx_buffer_size;
	uint8_t 	tx_ready;
	uint16_t 	tx_ptr_in;
	uint16_t 	tx_ptr_out;
	uint16_t 	tx_length;
	uint8_t  	tx_int_en;
	/* rx vars */
	uint8_t 	*rx_buffer;
	size_t		rx_buffer_size;
	uint8_t 	rx_ready;
	uint8_t		rx_ready_tmr;
	uint16_t	rx_ptr_in;
	uint16_t	rx_ptr_out;
	uint16_t 	rx_length;
};

#define DECLARE_COMM_BUFFER(NAME, TX_SIZE, RX_SIZE) \
	uint8_t comm_tx_buffer_##NAME[TX_SIZE]; \
	uint8_t comm_rx_buffer_##NAME[RX_SIZE]; \
	volatile struct tp_comm_buffer NAME = { \
		.tx_buffer = comm_tx_buffer_##NAME, \
		.tx_buffer_size = TX_SIZE, \
		.rx_buffer = comm_rx_buffer_##NAME, \
		.rx_buffer_size = RX_SIZE \
	}
#endif /* COMM_BUFFER_H_ */
