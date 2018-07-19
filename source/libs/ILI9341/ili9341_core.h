#ifndef ILI9341_CORE_H_
#define ILI9341_CORE_H_

#include <stm32f10x.h>
#include "cortexm_delay.h"
#include "ili9341_commands.h"
#include "ili9341_spi.h"

#define LCD_setAddressWindowToWrite(x1,y1,x2,y2) \
    LCD_setAddressWindow(x1, y1, x2, y2); \
    ili9341_sendCmd(LCD_GRAM)

#define LCD_setAddressWindowToRead(x1,y1,x2,y2) \
    LCD_setAddressWindow(x1, y1, x2, y2); \
    ili9341_sendCmd(LCD_RAMRD)

u16 LCD_getWidth();
u16 LCD_getHeight();

void LCD_init();

void LCD_setOrientation(u8 o);
void LCD_setAddressWindow(u16 x1, u16 y1, u16 x2, u16 y2);

#endif //ILI9341_CORE_H_
