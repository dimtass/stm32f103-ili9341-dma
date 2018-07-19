/**
 * This is a demo for the ILI9341 spi display.
 * 
 * st-flash --reset write build-stm32/stm32f103-ili9341-dma.bin 0x8000000
 * 
 *  Created on: Jul 5, 2018
 *      Author: Dimitris Tassopoulos
*/

#include <stdio.h>
#include "platform_config.h"
#include "ili9341_spi.h"
#include "ili9341_core.h"
#include "ili9341_graph.h"
#include "ili9341_text.h"

/* This function overclocks stm32 to 128MHz */
extern uint32_t overclock_stm32f103(void);

void dbg_uart_parser(uint8_t *buffer, size_t bufferlen, uint8_t sender);

/* Declare glb struct and initialize buffers */
struct tp_glb glb;

DECLARE_UART_DEV(dbg_uart, USART1, 115200, 256, 10, 1);

int counter = 0;
int fps = 0;

void main_loop(void)
{
	/* 1 ms timer */
	if (glb.tmr_1ms) {
		glb.tmr_1ms = 0;
		dev_uart_update(&dbg_uart);
	}

	if (glb.fps_done) {
		TRACE(("fps: %d\n", glb.fps_done));
		glb.fps_done = 0;
	}
}

int main(void)
{
	/* overclock */
	SystemCoreClock = overclock_stm32f103();

	if (SysTick_Config(SystemCoreClock / 1000)) {
		/* Capture error */
		while (1);
	}

	set_trace_level(
			0
			| TRACE_LEVEL_DEFAULT
			| TRACE_LEVEL_ADC
			,1);
	dev_uart_add(&dbg_uart);
	dbg_uart.fp_dev_uart_cb = dbg_uart_parser;

    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
    LCD_fillScreen(RED);
    LCD_setTextBgColor(BLACK);
    LCD_setTextSize(3);

	TRACE(("stm32f103 & ili9341 ...\n"));

	while(1) {
		main_loop();
        LCD_setCursor(0, 0);
        LCD_fillScreen(YELLOW);
        glb.fps++;
	}
}

void dbg_uart_parser(uint8_t *buffer, size_t bufferlen, uint8_t sender)
{
	buffer[bufferlen] = 0;
	TRACE(("dbg_uart_parser: %s\n", buffer));
	if (!strncmp((char*) buffer, "TEST", 4)) {
		//

	}
	else if (!strncmp((char*) buffer, "CMD1", 4)) {
//		uint8_t data = 0xAA;
//		ili9341_sendCmd(&data);
	}
	else if (!strncmp((char*) buffer, "CMD2", 4)) {
		//
	}
}
