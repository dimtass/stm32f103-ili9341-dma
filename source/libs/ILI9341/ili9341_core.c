#include "ili9341_core.h"

static u16 screen_width = LCD_PIXEL_WIDTH, screen_height = LCD_PIXEL_HEIGHT;

static const uint8_t init_commands[] =
{
		// Power control A
		6, LCD_POWERA, 0x39, 0x2C, 0x00, 0x34, 0x02,
		// Power control B
		4, LCD_POWERB, 0x00, 0xC1, 0x30,
		// Driver timing control A
		4, LCD_DTCA, 0x85, 0x00, 0x78,
		// Driver timing control B
		3, LCD_DTCB, 0x00, 0x00,
		// Power on sequence control
		5, LCD_POWER_SEQ, 0x64, 0x03, 0x12, 0x81,
		// Pump ratio control
		2, LCD_PRC, 0x20,
		// Power control 1
		2, LCD_POWER1, 0x23,
		// Power control 2
		2, LCD_POWER2, 0x10,
		// VCOM control 1
		3, LCD_VCOM1, 0x3E, 0x28,
		// VCOM cotnrol 2
		2, LCD_VCOM2, 0x86,
		// Memory access control
		2, LCD_MAC, 0x48,
		// Pixel format set
		2, LCD_PIXEL_FORMAT, 0x55,
		// Frame rate control
		3, LCD_FRMCTR1, 0x00, 0x18,
		// Display function control
		4, LCD_DFC, 0x08, 0x82, 0x27,
		// 3Gamma function disable
		2, LCD_3GAMMA_EN, 0x00,
		// Gamma curve selected
		2, LCD_GAMMA, 0x01,
		// Set positive gamma
		16, LCD_PGAMMA, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37,
		0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, 16, LCD_NGAMMA, 0x00, 0x0E, 0x14,
		0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
		0 };

void LCD_reset()
{
	ILI9341_RST_RESET;
	delay_ms(10);
	ILI9341_RST_SET;
	delay_ms(50);
}

void LCD_exitStandby()
{
	ili9341_sendCmd(LCD_SLEEP_OUT);
	delay_ms(150);
	ili9341_sendCmd(LCD_DISPLAY_ON);
}

static void LCD_configure()
{
	u8 count;
	u8 *address = (u8 *) init_commands;

	dev_spi_start(m_spi);
	while (1)
	{
		count = *(address++);
		if (count-- == 0)
			break;
		ili9341_sendCmdCont(*(address++));
		ili9341_sendDataCont8(address, count);
		address += count;
	}
	dev_spi_stop(m_spi);
	LCD_setOrientation(0);
}

void LCD_init()
{
	ili9341_spi_init();

	LCD_reset();
	LCD_exitStandby();
	LCD_configure();

	ILI9341_LED_SET;
}

void LCD_setOrientation(u8 o)
{
	if (o == ORIENTATION_LANDSCAPE || o == ORIENTATION_LANDSCAPE_MIRROR)
	{
		screen_height = LCD_PIXEL_WIDTH;
		screen_width = LCD_PIXEL_HEIGHT;
	}
	else
	{
		screen_height = LCD_PIXEL_HEIGHT;
		screen_width = LCD_PIXEL_WIDTH;
	}
	dev_spi_start(m_spi);
	ili9341_sendCmdCont(LCD_MAC);
	ili9341_sendDataCont8(&o, 1);
	dev_spi_stop(m_spi);
}

inline void LCD_setAddressWindow(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 pointData[2];

	dev_spi_start(m_spi);
	ili9341_sendCmdCont(LCD_COLUMN_ADDR);
	pointData[0] = x1;
	pointData[1] = x2;
	dev_spi_set16(m_spi);
	ili9341_sendDataCont16(pointData, 2);
	dev_spi_set8(m_spi);

	ili9341_sendCmdCont(LCD_PAGE_ADDR);
	pointData[0] = y1;
	pointData[1] = y2;
	dev_spi_set16(m_spi);
	ili9341_sendDataCont16(pointData, 2);
	dev_spi_set8(m_spi);
	dev_spi_stop(m_spi);
}

inline u16 LCD_getWidth()
{
	return screen_width;
}

inline u16 LCD_getHeight()
{
	return screen_height;
}
