/*
 * dev_spi.h
 *
 *  Created on: 14 May 2018
 *      Author: Dimitris Tassopoulos
 */

#ifndef DEV_SPI_H_
#define DEV_SPI_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h"
#include "platform_config.h"

enum en_spi_mode {
	SPI_MODE_0,		// CPOL=0, CPHA=0
	SPI_MODE_1,		// CPOL=0, CPHA=1
	SPI_MODE_2,		// CPOL=1, CPHA=0
	SPI_MODE_3		// CPOL=1, CPHA=1
};

enum en_spi_port {
	DEV_SPI1_GPIOA,
	DEV_SPI1_GPIOB,
	DEV_SPI2
};

#define DECLARE_SPI_CHANNEL(NAME, OWNER, CHANNEL) \
	struct dev_spi NAME = { \
		.channel = CHANNEL, \
	}

struct dev_spi {
	SPI_TypeDef * 	spi;
	GPIO_TypeDef *  port;
	uint16_t		miso;
	uint16_t		mosi;
	uint16_t		sck;
	uint16_t		nss;
	DMA_Channel_TypeDef * dma_tx_ch;
	uint32_t		dma_tx_flags;
	uint32_t		dma_tx_iqrn;
	DMA_Channel_TypeDef * dma_rx_ch;
	uint32_t		dma_rx_flags;
	uint32_t		dma_rx_iqrn;
	DMA_InitTypeDef dma_struct;
	SPI_InitTypeDef spi_struct;
};

/* Init functions */
void* dev_spi_init(struct dev_spi * spi, enum en_spi_port port, enum en_spi_mode mode, uint16_t spi_baudrate_prescaller);
void dev_spi_remove(struct dev_spi * spi);

/* Control functions */
void dev_spi_start(struct dev_spi * spi);
void dev_spi_stop(struct dev_spi * spi);
void dev_spi_wait(struct dev_spi * spi);
void dev_spi_set8(struct dev_spi * spi);
void dev_spi_set16(struct dev_spi * spi);

/* 8-bit send/receive functions */
void dev_spi_send8(struct dev_spi * spi, uint8_t * data, size_t data_len);
void dev_spi_sendCircular8(struct dev_spi * spi, uint8_t * data, size_t data_len);
void dev_spi_recv8(struct dev_spi * spi, uint8_t * data, size_t data_len);
void dev_spi_recvCircular8(struct dev_spi * spi, uint8_t * data, size_t data_len);

/* 16-bit functions */
void dev_spi_send16(struct dev_spi * spi, uint16_t *data, size_t data_len);
void dev_spi_sendCircular16(struct dev_spi * spi, uint16_t *data, size_t data_len);
void dev_spi_recv16(struct dev_spi * spi, uint16_t * data, size_t data_len);
void dev_spi_recvCircular16(struct dev_spi * spi, uint8_t * data, size_t data_len);

#endif /* DEV_SPI_H_ */
