/*
 * platform_config.h
 *
 *  Created on: 8 May 2018
 *      Author: Dimitris Tassopoulos
 */

#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include "stm32f10x.h"
#include "dev_uart.h"

/**
 * Trace levels for this project.
 * Have in mind that these are bit flags!
 */
typedef enum {
	TRACE_LEVEL_DEFAULT = 	(1 << 0),
	TRACE_LEVEL_ADC = 	(1 << 1),
} en_trace_level;

#define DEBUG_TRACE

#ifdef DEBUG_TRACE
#define TRACE(X) TRACEL(TRACE_LEVEL_DEFAULT, X)
#define TRACEL(TRACE_LEVEL, X) do { if (glb.trace_levels & TRACE_LEVEL) printf X;} while(0)
#else
#define TRACE(X)
#define TRACEL(X,Y)
#endif

/* LED patterns */
enum {
	LED_PATTERN_IDLE = 0b10100000,
};

/* GPIOA */
#define ADC1_PIN 		GPIO_Pin_0
#define ADC1_GPIO_Port 	GPIOA
#define ADC2_PIN 		GPIO_Pin_1
#define ADC2_GPIO_Port 	GPIOA

/* GPIOC */
#define PIN_STATUS_LED 		GPIO_Pin_13
#define PORT_STATUS_LED 	GPIOC

struct tp_glb {
	volatile uint16_t tmr_1ms;
	volatile uint16_t tmr_10ms;
	volatile uint16_t tmr_1000ms;
	volatile uint16_t fps;
	volatile uint8_t fps_done;
	en_trace_level trace_levels;

	/* ADC values */
	volatile uint32_t 	adc1_temp;
	volatile uint16_t 	adc1_val;
	volatile uint8_t	adc1_counter;
	volatile uint8_t	adc1_ready;

	volatile uint32_t 	adc2_temp;
	volatile uint16_t 	adc2_val;
	volatile uint8_t	adc2_counter;
	volatile uint8_t	adc2_ready;
};

extern struct tp_glb glb;

static inline void set_trace_level(en_trace_level level, uint8_t enable)
{
	if (enable) {
		glb.trace_levels |= (uint32_t) level;
	}
	else {
		glb.trace_levels &= ~((uint32_t) level);
	}
}

#endif /* __PLATFORM_CONFIG_H */
