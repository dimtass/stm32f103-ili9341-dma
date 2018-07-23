/*
 * xpt2046.h
 *
 * XPT2046 is the same controller with TI's ADS7843.
 * The manual of ADS7843 can be found here:
 * 	http://www.ti.com/lit/ds/symlink/ads7843.pdf
 *
 * There'a a document that provides some tips for the
 * touch controller:
 * 	"TOUCH SCREEN CONTROLLER TIPS"
 * 	http://www.ti.com/lit/an/sbaa036/sbaa036.pdf
 *
 * For calibration methods see TI's document
 * 	"Calibration in touch-screen systems".
 * 	http://www.ti.com/lit/an/slyt277/slyt277.pdf
 *
 *
 *  Created on: Jul 21, 2018
 *      Author: Dimitris Tassopoulos
 */

#ifndef XPT2046_ADS7843_H_
#define XPT2046_ADS7843_H_

#include "platform_config.h"
#include "stm32f10x.h"
#include "dev_spi.h"
#include "cortexm_delay.h"
#include "touch_point.h"
#include "touch_calib.h"

/* use the same orientation values as the ili9341 */
enum en_xpt2046_orientation {
	XPT2046_ORIENTATION_PORTRAIT = 0x48,
	XPT2046_ORIENTATION_LANDSCAPE = 0x28,
	XPT2046_ORIENTATION_PORTRAIT_MIRROR = 0x88,
	XPT2046_ORIENTATION_LANDSCAPE_MIRROR = 0xE8,
};

struct touch_settings {
	enum en_xpt2046_orientation orientation;
	int offset_x;
	int offset_y;
};

void xpt2046_init(enum en_spi_port port, struct touch_settings * settings);
void xpt2046_set_orientation(enum en_xpt2046_orientation orientation);

/* This should be called when an interrupt is triggered */
void xpt2046_update(void);
uint8_t xpt2046_is_busy(void);
void xpt2046_polling(struct touch_point * p);

void xpt2046_get_point_adc(struct touch_point * tp);
void xpt2046_get_point_pixel(struct touch_point * tp);

/* 3-point calibration. */
void xpt2046_set_calibration(const struct touch_calibration_params * params);
void xpt2046_get_calibration(const struct touch_calibration_params * params);

void xpt2046_calibrate_points(const struct touch_point * tp, struct touch_point * cp);

#endif /* XPT2046_ADS7843_H_ */
