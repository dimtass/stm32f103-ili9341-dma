/*
 * stm32_delay.h
 *
 * This delay driver is for cortex-M cores that support DWT.
 * DWT is the data watchpoint and trace unit, but it can be
 * used as a precision timer with it's own resources.
 *
 * Be aware that this is not supported in old CMSIS libraries,
 * therefore you need to get one of the latest ones.
 *
 * Free of use blah, blah, MIT, harvard e.t.c.
 *
 *  Created on: Jul 9, 2018
 *      Author: Dimitris Tassopoulos
 */
#include "cortexm_delay.h"

static uint32_t m_system_core_clock = 1;

/**
 * DWT high precise timer to use for delays
 */
void delay_init(uint32_t system_core_clock)
{
	m_system_core_clock = system_core_clock;
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}


static inline uint32_t DWT_Get(void)
{
    return DWT->CYCCNT;
}


static inline uint8_t DWT_Compare(int32_t tp)
{
    return (((int32_t)DWT_Get() - tp) < 0);
}


void delay_us(uint32_t us)
{
    int32_t tp = DWT_Get() + us * (m_system_core_clock/1000000);
    while (((int32_t)DWT_Get() - tp) < 0);
}


void delay_ms(uint32_t ms)
{
    int32_t tp = DWT_Get() + ms * (m_system_core_clock/1000);
    while (((int32_t)DWT_Get() - tp) < 0);
}
