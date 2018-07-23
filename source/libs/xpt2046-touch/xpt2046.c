/*
 * xpt2046.c
 *
 *  Created on: Jul 21, 2018
 *      Author: Dimitris Tassopoulos
 */

#include "xpt2046.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"

#define XPT2046_IRQ_PIN		GPIO_Pin_8
#define XPT2046_IRQ_PORT	GPIOA

static struct dev_spi gspi;
static struct dev_spi * m_spi = &gspi;
static struct touch_point m_tp = {0};
static struct touch_settings m_ts = {0};
static struct touch_calibration_params m_cal = {0};


#define MAX_ADC_VAL 2047 //(1<<12)-1

enum en_xpt2046_commands {
	REQ_IN3 = 0xA0,		// Aux ADC input (not used)
	REQ_IN4 = 0xE0,		// Aux ADC input (not used)
	REQ_X = 0x90,
	REQ_Y = 0xD0,
	REQ_X_CONT = 0x91,
	REQ_Y_CONT = 0xD1,
	REQ_Y_POWERDN = 0xD0,
};

uint16_t xpt2046_read(uint8_t control);
void xpt2046_irq_enable(void);
void xpt2046_irq_disable(void);

void xpt2046_init(enum en_spi_port port, struct touch_settings * settings)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    memcpy(&m_ts, settings, sizeof(struct touch_settings));

    /* Initialize SPI */
	dev_spi_init(m_spi, DEV_SPI2, SPI_MODE_0, SPI_BaudRatePrescaler_32);

	/* Enable GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* Configure PA0 pin as input floating */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = XPT2046_IRQ_PIN;
	GPIO_Init(XPT2046_IRQ_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	/* Connect EXTI8 Line to PA.08 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);

	xpt2046_irq_enable();

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* do a dummy read */
	xpt2046_read(REQ_X);
}

void xpt2046_irq_enable(void)
{
    EXTI->IMR |= EXTI_Line8;
    EXTI->EMR |= EXTI_Line8;
    EXTI->FTSR |= EXTI_Line8;
    EXTI->RTSR &= ~EXTI_Line8;
}

void xpt2046_irq_disable(void)
{

    EXTI->IMR &= ~EXTI_Line8;
    EXTI->EMR &= ~EXTI_Line8;
    EXTI->RTSR &= ~EXTI_Line8;
    EXTI->FTSR &= ~EXTI_Line8;
}

uint16_t xpt2046_read(uint8_t control)
{
	dev_spi_set8(m_spi);
	dev_spi_start(m_spi);
	dev_spi_send8(m_spi, &control, 1);
	dev_spi_wait(m_spi);

	uint16_t value = 0;
	uint16_t dummy = 0xFFFF;

	dev_spi_set16(m_spi);
	dev_spi_send16(m_spi, &dummy, 1);
	dev_spi_recv16(m_spi, &value, 1);
	dev_spi_wait(m_spi);

	dev_spi_stop(m_spi);

	return value;
}


void xpt2046_set_calibration(const struct touch_calibration_params * params)
{
	memcpy(&m_cal, params, sizeof(struct touch_calibration_params));
}

void xpt2046_get_calibration(const struct touch_calibration_params * params)
{
	params = &m_cal;
}

void xpt2046_convert_to_orientation(const uint16_t x, const uint16_t y, uint16_t *x_conv, uint16_t *y_conv)
{

	switch(m_ts.orientation) {
	case XPT2046_ORIENTATION_PORTRAIT:
		*x_conv = MAX_ADC_VAL - x;
		*y_conv = y;
		break;
	case XPT2046_ORIENTATION_PORTRAIT_MIRROR:
		*x_conv = x;
		*y_conv = MAX_ADC_VAL - y;
		break;
	case XPT2046_ORIENTATION_LANDSCAPE:
		*x_conv = y;
		*y_conv = x;
		break;
	case XPT2046_ORIENTATION_LANDSCAPE_MIRROR:
		*x_conv = MAX_ADC_VAL - y;
		*y_conv = MAX_ADC_VAL - x;
		break;
	}
}

void xpt2046_polling(struct touch_point * p)
{
	uint16_t touch_cntr = 0;
	uint8_t done = 0;
	uint32_t x_sum = 0;
	uint32_t y_sum = 0;

	//xpt2046_irq_disable();

	TRACEL(TRACE_LEVEL_TOUCH, ("start polling\n"));
	while (!done) {
		delay_ms(1);
		if (!GPIO_ReadInputDataBit(XPT2046_IRQ_PORT, XPT2046_IRQ_PIN)) {
			/* discard first 50 readings */
			if (touch_cntr < 50) {
				xpt2046_read(REQ_X);
				xpt2046_read(REQ_Y);
				x_sum = y_sum = 0;
			}
			else {
				x_sum += xpt2046_read(REQ_X) >> 4;
				y_sum += xpt2046_read(REQ_Y) >> 4;
				if (touch_cntr >= 250) {
					x_sum = x_sum / (touch_cntr- 50);
					y_sum = y_sum / (touch_cntr- 50);
					xpt2046_convert_to_orientation(x_sum, y_sum, &p->x, &p->y);
					TRACEL(TRACE_LEVEL_TOUCH, ("POLL x:%04d, y:%04d\n", p->x, p->y));
					done = 1;
				}
			}
			touch_cntr++;
		}
		else {
			touch_cntr = 0;
		}
	}
	//xpt2046_irq_enable();
}


void xpt2046_update(void)
{
	xpt2046_irq_disable();
	xpt2046_read(0x91);
	xpt2046_read(0xD1);

	uint16_t x = xpt2046_read(REQ_X) >> 4;
	uint16_t y = xpt2046_read(REQ_Y) >> 4;

	/* Only update when there's really a touch (IRQPIN low) */
	if (GPIO_ReadInputDataBit(XPT2046_IRQ_PORT, XPT2046_IRQ_PIN))
		goto exit_irq_enable;

	/* remove noise */
	if ((x==0) || (y==0) || (x==0x7FF8) || (y==0x7FF8))
		goto exit_irq_enable;

	xpt2046_convert_to_orientation(x, y, &m_tp.x, &m_tp.y);

	m_tp.x += m_ts.offset_x;
	m_tp.y += m_ts.offset_y;

	exit_irq_enable:
	xpt2046_irq_enable();
}


void xpt2046_get_point_adc(struct touch_point * tp)
{
	tp->x = m_tp.x;
	tp->y = m_tp.y;
}


void xpt2046_get_point_pixel(struct touch_point * tp)
{
	tp->x = ((m_cal.Dx1 * m_tp.x) + (m_cal.Dx2 * m_tp.x) + m_cal.Dx3) / m_cal.D;
	tp->y = ((m_cal.Dy1 * m_tp.y) + (m_cal.Dy2 * m_tp.y) + m_cal.Dy3) / m_cal.D;
}

void xpt2046_calibrate_points(const struct touch_point * tp, struct touch_point * cp)
{
	cp->x = ((m_cal.Dx1 * tp->x) + (m_cal.Dx2 * tp->x) + m_cal.Dx3) / m_cal.D;
	cp->y = ((m_cal.Dy1 * tp->y) + (m_cal.Dy2 * tp->y) + m_cal.Dy3) / m_cal.D;
}
