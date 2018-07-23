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
#include "touch_point.h"
#include "touch_calib.h"
#include "xpt2046.h"
#include "ili9341_touch_calib.h"

enum en_modes {
	MODE_BENCHMARK,
	MODE_TOUCH_CALIBRATION
};

enum en_fps_show {
	FPS_MODE_0,		// don't trace and don't show fps
	FPS_MODE_1,		// trace but don't show
	FPS_MODE_2		// trace and show
};

/* This function overclocks stm32 to 128MHz */
extern uint32_t overclock_stm32f103(void);

void dbg_uart_parser(uint8_t *buffer, size_t bufferlen, uint8_t sender);

/* Declare glb struct and initialize buffers */
volatile struct tp_glb glb;

DECLARE_UART_DEV(dbg_uart, USART1, 115200, 256, 10, 1);

int counter = 0;
int fps = 0;

void main_loop(void)
{
	/* 10 ms timer */
	if (glb.tmr_10ms) {
		glb.tmr_10ms = 0;
		dev_uart_update(&dbg_uart);
		/* check for touch IRQ only every 10ms. This works also
		 * as a debounce. */
		if (glb.touch_irq) {
			glb.touch_irq = 0;
			/* read new x-y values from the touch controller */
			xpt2046_update();
			if (glb.touch_trace) {
				struct touch_point p;
				xpt2046_get_point_adc(&p);
				TRACEL(TRACE_LEVEL_TOUCH, ("TOUCH ADC x:%04d, y:%04d\n", p.x, p.y));
				xpt2046_get_point_pixel(&p);
				TRACEL(TRACE_LEVEL_TOUCH, ("TOUCH CAL x:%04d, y:%04d\n", p.x, p.y));
			}
		}
	}

	if (glb.trace_fps) {
		if (glb.fps_show)	// FPS_MODE_1 || FPS_MODE_2
			TRACEL(TRACE_LEVEL_LCD, ("fps: %d\n", glb.fps));
		glb.trace_fps = 0;
	}
	/* benchmark mode */
	if (glb.mode == MODE_BENCHMARK) {
		if (glb.fps_show == FPS_MODE_2) {
			LCD_fillScreen(YELLOW);
			char tmpstr[5];
			LCD_setCursor(0, 0);
			sprintf(tmpstr, "%d", glb.fps);
			LCD_writeString((uint8_t*)tmpstr);
		}
		else {
			LCD_fillScreen(GREEN);
		}
		glb.fps_cntr++;
	}
	else if (glb.mode == MODE_TOUCH_CALIBRATION) {
		/* lcd points */
		struct touch_3_points lp = {
			.p1 = {
				.x = 57,
				.y = 65,
			},
			.p2 = {
				.x = 218,
				.y = 102,
			},
			.p3 = {
				.x = 86,
				.y = 200,
			}
		};
		struct touch_3_points tp;	// touch points
		struct touch_calibration_params params;
		ili9341_touch_calib_start(&tp, &lp, &params);
		TRACEL(TRACE_LEVEL_TOUCH, (
				"Calibration params:\n"
				"\t  D: %d\n"
				"\tDx1: %d\n"
				"\tDx2: %d\n"
				"\tDx3: %d\n"
				"\tDy1: %d\n"
				"\tDy2: %d\n"
				"\tDy3: %d\n",
				params.D, params.Dx1, params.Dx2, params.Dx3,
				params.Dy1, params.Dy2, params.Dy3));
		xpt2046_set_calibration(&params);

		struct touch_point cp;
		xpt2046_calibrate_points(&tp.p1, &cp);
		TRACEL(TRACE_LEVEL_TOUCH, ("Calibrated point x1:%d, y1:%d\n",
				cp.x, cp.y));
		/* switch back to benchmark mode */
		glb.mode = MODE_BENCHMARK;
	    LCD_setTextSize(4);
	}
}

int main(void)
{
	/* overclock. Comment out for default clocks */
	SystemCoreClock = overclock_stm32f103();

	if (SysTick_Config(SystemCoreClock / 1000)) {
		/* Capture error */
		while (1);
	}
	/* Initialize the delay timer */
	delay_init(SystemCoreClock);

	/* enable/disable traces */
	set_trace_level(
			0
			| TRACE_LEVEL_DEFAULT
			| TRACE_LEVEL_LCD
			| TRACE_LEVEL_TOUCH
			| TRACE_LEVEL_UART
			,1);
	/* setup uart port */
	dev_uart_add(&dbg_uart);
	/* set callback for uart rx */
	dbg_uart.fp_dev_uart_cb = dbg_uart_parser;

	/* Initialize LCD */
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
    LCD_fillScreen(RED);
    LCD_setTextBgColor(BLACK);
    LCD_setTextSize(4);
    LCD_setTextColor(YELLOW);

    /* default mode is benchmark */
    glb.mode = MODE_BENCHMARK;
//    glb.mode = MODE_TOUCH_CALIBRATION;

    /* Initialize touch */
    struct touch_settings ts = {
    	.orientation = XPT2046_ORIENTATION_LANDSCAPE_MIRROR,
		.offset_x = 0,
		.offset_y = 0,
    };
    xpt2046_init(DEV_SPI2, &ts);

	TRACE(("stm32f103 & ili9341 & touch...\n"));

	while(1) {
		main_loop();
	}
}

/* Supported commands :
 * 	MODE=<mode>
 * 	where <mode>,
 * 		BENCH	: benchmark mode
 * 		CALIB	: touch calibrations mode
 *
 * 	FPS=<en>
 * 	where <en>:
 * 		0	: disable both trace and show FPS
 * 		1	: trace FPS on uart but disable on LCD
 * 		3	: trace and show FPS
 *
 */
void dbg_uart_parser(uint8_t *buffer, size_t bufferlen, uint8_t sender)
{
	buffer[bufferlen] = 0;
	TRACEL(TRACE_LEVEL_UART, ("dbg_uart_parser: %s\n", buffer));
	if (!strncmp((char*) buffer, "MODE=", 5)) {
		if (!strncmp((char*) &buffer[5], "BENCH", 5)) {
			/* benchmark mode */
		    LCD_setTextSize(4);
			glb.mode = MODE_BENCHMARK;
		}
		else if (!strncmp((char*) &buffer[5], "CALIB", 5)) {
			/* enter calibration mode */
			glb.mode = MODE_TOUCH_CALIBRATION;
		}
	}
	else if (!strncmp((char*) buffer, "FPS=", 4)) {
		if (buffer[4] == '1')
			glb.fps_show = FPS_MODE_1;
		else if (buffer[4] == '2')
			glb.fps_show = FPS_MODE_2;
		else
			glb.fps_show = FPS_MODE_0;

	}
	else if (!strncmp((char*) buffer, "TOUCH=", 6)) {
		if (buffer[6] == '0')
			glb.touch_trace = 0;
		else
			glb.touch_trace = 1;
	}
}
