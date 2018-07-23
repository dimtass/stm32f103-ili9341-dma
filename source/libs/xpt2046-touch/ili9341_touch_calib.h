/*
 * ili9341_calibration.h
 *
 *  Created on: Jul 23, 2018
 *      Author: dimtass
 */

#ifndef ILI9341_TOUCH_CALIB_H_
#define ILI9341_TOUCH_CALIB_H_

#include "platform_config.h"
#include "ili9341_spi.h"
#include "ili9341_core.h"
#include "ili9341_graph.h"
#include "ili9341_text.h"
#include "touch_point.h"
#include "touch_calib.h"
#include "xpt2046.h"

void ili9341_touch_calib_start(struct touch_3_points * ap, struct touch_3_points * lp,
		struct touch_calibration_params * params);

#endif /* ILI9341_TOUCH_CALIB_H_ */
