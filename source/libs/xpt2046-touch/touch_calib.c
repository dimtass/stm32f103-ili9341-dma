/*
 * touch_calib.c
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

#include "touch_calib.h"

/* These functions are from page 9 in http://www.ti.com/lit/an/slyt277/slyt277.pdf */

static int touch_calc_D(const struct touch_3_points * a)
{
	int ret = (a->p1.x - a->p3.x)*(a->p2.y - a->p3.y) - (a->p2.x - a->p3.x)*(a->p1.y - a->p3.y);
	return ret;
}

static int touch_calc_Dx1(const struct touch_3_points * a,
							const struct touch_3_points * l)
{
	int ret = (l->p1.x - l->p3.x)*(a->p2.y - a->p3.y) - (l->p2.x - l->p3.x)*(a->p1.y - a->p3.y);
	return ret;
}

static int touch_calc_Dx2(const struct touch_3_points * a,
							const struct touch_3_points * l)
{
	int ret = (a->p1.x - a->p3.x)*(l->p2.x - l->p3.x) - (a->p2.x - a->p3.x)*(l->p1.x - l->p3.x);
	return ret;
}

static int touch_calc_Dx3(const struct touch_3_points * a,
							const struct touch_3_points * l)
{
	int ret = l->p1.x * (a->p2.x * a->p3.y - a->p3.x * a->p2.y) - l->p2.x * (a->p1.x * a->p3.y - a->p3.x*a->p1.y)
			+ l->p3.x * (a->p1.x*a->p2.y - a->p2.x*a->p1.y);
	return ret;
}

static int touch_calc_Dy1(const struct touch_3_points * a,
							const struct touch_3_points * l)
{
	int ret = (l->p1.y - l->p3.y)*(a->p2.y - a->p3.y) - (l->p2.y - l->p3.y)*(a->p1.y - a->p3.y);
	return ret;
}

static int touch_calc_Dy2(const struct touch_3_points * a,
							const struct touch_3_points * l)
{
	int ret = (a->p1.x - a->p3.x)*(l->p2.y - l->p3.y) - (a->p2.x - a->p3.x)*(l->p1.y - l->p3.y);
	return ret;
}

static int touch_calc_Dy3(const struct touch_3_points * a,
							const struct touch_3_points * l)
{
	int ret = l->p1.y * (a->p2.x * a->p3.y - a->p3.x * a->p2.y) - l->p2.y * (a->p1.x * a->p3.y - a->p3.x * a->p1.y)
			+ l->p3.y * (a->p1.x * a->p2.y - a->p2.x * a->p1.y);
	return ret;
}

void touch_calculate_calib_data(const struct touch_3_points * adc_points,
							const struct touch_3_points * lcd_points,
							struct touch_calibration_params * params)
{
	params->D = touch_calc_D(adc_points);
	params->Dx1 = touch_calc_Dx1(adc_points, lcd_points);
	params->Dx2 = touch_calc_Dx2(adc_points, lcd_points);
	params->Dx3 = touch_calc_Dx3(adc_points, lcd_points);
	params->Dy1 = touch_calc_Dy1(adc_points, lcd_points);
	params->Dy2 = touch_calc_Dy2(adc_points, lcd_points);
	params->Dy3 = touch_calc_Dy3(adc_points, lcd_points);
}
