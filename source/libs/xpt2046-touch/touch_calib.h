/*
 * touch_calib.h
 *
 * This code is calculates the 3-points calibration method definitions
 * for 3 independent points. To use that you need to retrieve 3 points
 * which they are not depended (aka they don't create a line) and then
 * pass these points to the function that returns the calculated cali-
 * bration points and use them in your code to calibrate you LCD.
 *
 * The method is described here:
 * `"Calibration in touch-screen systems"
 * 	http://www.ti.com/lit/an/slyt277/slyt277.pdf
 *
 *  Created on: Jul 22, 2018
 *      Author: Dimitris Tassopoulos
 */

#ifndef TOUCH_CALIB_H_
#define TOUCH_CALIB_H_

#include <stdint.h>
#include "touch_point.h"

struct touch_calibration_params {
	int D;
	int Dx1;
	int Dx2;
	int Dx3;
	int Dy1;
	int Dy2;
	int Dy3;
};

struct touch_3_points {
	struct touch_point p1;
	struct touch_point p2;
	struct touch_point p3;
};

void touch_calculate_calib_data(const struct touch_3_points * adc_points,
							const struct touch_3_points * lcd_points,
							struct touch_calibration_params * params);

#endif /* TOUCH_CALIB_H_ */
