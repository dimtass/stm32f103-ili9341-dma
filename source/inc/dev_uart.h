/*
 * debug_uart.h
 *
 *  Created on: 1 Apr 2017
 *      Author: dimtass
 *
 * Add this line to an 1ms timer interrupt:
 * 		if (m_buff.rx_ready) m_buff.rx_ready_tmr++;
 *
 * Add this line to the __io_putchar(int ch) function (used for printf)
 * 		debug_uart_send(ch);
 *
 * Add this line to the main loop in order to check for incoming data
 * 		debug_uart_rx_poll()
 *
 * Add this function in the interrupt handler of the used UART port
 * 		debug_uart_irq()
 */

#ifndef DEV_UART_H_
#define DEV_UART_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h"
#include "comm_buffer.h"

#define DECLARE_UART_DEV(NAME, PORT, BAUDRATE, BUFFER_SIZE, TIMEOUT_MS, DEBUG) \
	struct dev_uart NAME = { \
		.port = PORT, \
		.config = { \
			.USART_BaudRate = BAUDRATE, \
			.USART_WordLength = USART_WordLength_8b, \
			.USART_StopBits = USART_StopBits_1, \
			.USART_Parity = USART_Parity_No, \
			.USART_Mode = USART_Mode_Rx | USART_Mode_Tx, \
			.USART_HardwareFlowControl = USART_HardwareFlowControl_None, \
		}, \
		.uart_buff = { \
			.rx_buffer_size = BUFFER_SIZE, \
			.tx_buffer_size = BUFFER_SIZE, \
		}, \
		.timeout_ms = TIMEOUT_MS, \
		.debug = DEBUG, \
		.fp_dev_uart_cb = NULL, \
	}

/**
 * @brief Callback function definition for reception
 * @param[in] buffer Pointer to the RX buffer
 * @param[in] bufferlen The length of the received data
 */
typedef void (*dev_uart_cb)(uint8_t *buffer, size_t bufferlen, uint8_t sender);

struct dev_uart {
	USART_TypeDef*		port;
	USART_InitTypeDef	config;
	NVIC_InitTypeDef	nvic;
	uint8_t				debug;
	uint8_t				timeout_ms;
	uint8_t				available;
	volatile struct tp_comm_buffer uart_buff;
	/**
	* @brief Callback function definition for reception
	* @param[in] buffer Pointer to the RX buffer
	* @param[in] bufferlen The length of the received data
	*/
	dev_uart_cb fp_dev_uart_cb;
};

/**
 * @brief Initialize the debugging UART interface
 * @param[in] dev_uart A pointer to the UART device
 */
void dev_uart_add(struct dev_uart * uart);

/**
 * @brief Configure the STM32 uart port (including the port pins)
 * @param[in] dev_uart A pointer to the UART device
 */
void* dev_uart_probe(struct dev_uart * dev);

/**
 * @brief Remove the STM32 uart port (including the port pins)
 * @param[in] dev_uart A pointer to the UART device
 */
void dev_uart_remove(struct dev_uart * dev);

/**
 * @brief IRQ handler for the debug interface
 * @param[in] dev_uart A pointer to the UART device
 */
void dev_uart_irq(struct dev_uart * dev);

/**
 * @brief Send a single byte on the debug uart. This should be used with the
 * 		syscalls.c file to implement the printf() function
 * @param[in] dev_uart A pointer to the UART device
 * @param[in] ch The byte to send
 * @return int The byte sent
 */
int dev_uart_send_ch(struct dev_uart * dev, int ch);

size_t dev_uart_send_buffer(struct dev_uart * dev, uint8_t * buffer, size_t buffer_len);

int dev_uart_receive(struct dev_uart * dev);


void dev_uart_set_baud_rate(struct dev_uart * dev, uint32_t baudrate);


/**
 * @brief Poll the RX buffer for new data. If new data are found then
 * 		the fp_debug_uart_cb will be called.
 * @param[in] dev_uart A pointer to the UART device
 */
void dev_uart_update(struct dev_uart * uart);



#endif /* DEBUG_UART_H_ */
