#include "ili9341_spi.h"

#define DMA_BUF_SIZE 2048
u16 dmaBufIndex = 0;
u16 dmaBuffer[DMA_BUF_SIZE];

struct dev_spi gspi;
struct dev_spi * m_spi = &gspi;

void ili9341_spi_init(void)
{
	dev_spi_init(m_spi, DEV_SPI1_GPIOA);

	GPIO_InitTypeDef gpioStructure;

	// GPIO speed by default
	gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;

	// GPIO for CS/DC/LED/RESET
	gpioStructure.GPIO_Pin = ILI9341_DC_PIN | ILI9341_RESET_PIN
			| ILI9341_LED_PIN;
	gpioStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &gpioStructure);
}

inline void ili9341_sendCmd(u8 cmd)
{
	ILI9341_DC_RESET;
	dev_spi_start(m_spi);
	dev_spi_send8(m_spi, &cmd, 1);
	dev_spi_wait(m_spi);
	dev_spi_stop(m_spi);
}

inline void ili9341_sendCmdCont(u8 cmd)
{
	ILI9341_DC_RESET;
	dev_spi_send8(m_spi, &cmd, 1);
	dev_spi_wait(m_spi);
}

inline void ili9341_receiveDataCont8(u8 *data) {
    u8 dummy = 0xFF;
    dev_spi_send8(m_spi, &dummy, 1);
    dev_spi_recv8(m_spi, data, 1);
	dev_spi_wait(m_spi);
}

inline void ili9341_sendData8(u8 *data, u32 n)
{
	ILI9341_DC_SET;
	dev_spi_start(m_spi);
	dev_spi_send8(m_spi, data, n);
	dev_spi_wait(m_spi);
	dev_spi_stop(m_spi);
}

inline void ili9341_sendDataCont8(u8 *data, u32 n)
{
	ILI9341_DC_SET;
	dev_spi_send8(m_spi, data, n);
	dev_spi_wait(m_spi);
}

inline void ili9341_sendData16(u16 *data, u32 n)
{
	ILI9341_DC_SET;
	dev_spi_start(m_spi);
	dev_spi_send16(m_spi, data, n);
	dev_spi_wait(m_spi);
	dev_spi_stop(m_spi);
}

inline void ili9341_sendDataCont16(u16 *data, u32 n)
{
	ILI9341_DC_SET;
	dev_spi_send16(m_spi, data, n);
	dev_spi_wait(m_spi);
}

inline void ili9341_sendDataBuf16()
{
	if (dmaBufIndex == 0)
		return;
	ILI9341_DC_SET;
	dev_spi_send16(m_spi, dmaBuffer, dmaBufIndex);
	dmaBufIndex = 0;
	dev_spi_wait(m_spi);
}

inline void ili9341_sendDataContBuf16(u16 *data, u32 n)
{
	while (n--)
	{
		dmaBuffer[dmaBufIndex] = *data++;
		if (dmaBufIndex == DMA_BUF_SIZE - 1)
		{
			ili9341_sendDataBuf16();
		}
		dmaBufIndex++;
	}
}

static inline void ili9341_sendDataCircular16(u16 *data, u32 n)
{
	ILI9341_DC_SET;
	dev_spi_sendCircular16(m_spi, data, n);
	dev_spi_wait(m_spi);
}

//</editor-fold>

inline void ili9341_fill16(u16 color, u32 n)
{
	dev_spi_start(m_spi);
	ili9341_sendCmdCont(LCD_GRAM);
	while (n != 0)
	{
		u16 ts = (u16) (n > UINT16_MAX ? UINT16_MAX : n);
		ili9341_sendDataCircular16(&color, ts);
		n -= ts;
	}
	dev_spi_stop(m_spi);
}
