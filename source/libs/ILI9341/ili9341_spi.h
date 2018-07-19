#ifndef ILI9341_SPI_H_
#define ILI9341_SPI_H_

#include "platform_config.h"
#include "dev_spi.h"
#include "ili9341_commands.h"

#define ILI9341_LED_PIN     GPIO_Pin_0
#define ILI9341_RESET_PIN   GPIO_Pin_2
#define ILI9341_DC_PIN      GPIO_Pin_3

#define ILI9341_DC_SET      GPIO_SetBits(GPIOA, ILI9341_DC_PIN);
#define ILI9341_DC_RESET    GPIO_ResetBits(GPIOA, ILI9341_DC_PIN);

#define ILI9341_RST_SET     GPIO_SetBits(GPIOA, ILI9341_RESET_PIN);
#define ILI9341_RST_RESET   GPIO_ResetBits(GPIOA, ILI9341_RESET_PIN);

#define ILI9341_LED_SET      GPIO_SetBits(GPIOA, ILI9341_LED_PIN);
#define ILI9341_LED_RESET    GPIO_ResetBits(GPIOA, ILI9341_LED_PIN);

void ili9341_spi_init();

void ili9341_sendCmd(u8 cmd);
void ili9341_sendCmdCont(u8 cmd);

void ili9341_receiveDataCont8(u8 *data);

void ili9341_sendData8(u8 *data, u32 n);
void ili9341_sendData16(u16 *data, u32 n);

void ili9341_sendDataCont8(u8 *data, u32 n);
void ili9341_sendDataCont16(u16 *data, u32 n);

void ili9341_sendDataBuf16();
void ili9341_sendDataContBuf16(u16 *data, u32 n);

void ili9341_fill16(u16 color, u32 n);

extern struct dev_spi * m_spi;

#endif //ILI9341_SPI_H_
