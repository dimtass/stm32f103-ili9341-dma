/*
 * ili9341_touch_calib.c
 *
 *  Created on: Jul 23, 2018
 *      Author: Dimitris Tassopoulos
 */

#include "ili9341_touch_calib.h"
#include "cortexm_delay.h"

#define DELAY_BETWEEN_TOUCH_MS 1500

enum en_calib_state {
	STATE_DRAW_SCREEN_START,
	STATE_DRAW_P1,
	STATE_WAIT_FOR_P1,
	STATE_DRAW_P2,
	STATE_WAIT_FOR_P2,
	STATE_DRAW_P3,
	STATE_WAIT_FOR_P3,
	STATE_CREATE_CALIB_DATA,
	STATE_END
};

static int m_state = STATE_DRAW_SCREEN_START;

static inline void draw_cross(struct touch_point * p)
{
	LCD_drawLine(p->x, p->y-10, p->x, p->y+10, WHITE);
	LCD_drawLine(p->x-10, p->y, p->x+10, p->y, WHITE);
}

static void state_draw_screen_start(void)
{
    LCD_fillScreen(BLACK);
    LCD_setTextBgColor(BLACK);
    LCD_setTextSize(1);
    LCD_setTextColor(WHITE);
	LCD_setCursor(0, 0);
	char tmpstr[100];
	sprintf(tmpstr, "Press the center of the cross.");
	LCD_writeString((uint8_t*)tmpstr);
	m_state = STATE_DRAW_P1;
}

void ili9341_touch_calib_start(struct touch_3_points * ap, struct touch_3_points * lp,
		struct touch_calibration_params * params)
{
	if (!ap || !lp) return;

	m_state = STATE_DRAW_SCREEN_START;
	while (m_state != STATE_END) {
		switch (m_state) {
		case STATE_DRAW_SCREEN_START:
			TRACEL(TRACE_LEVEL_TOUCH, ("Calibration started...\n"));
			state_draw_screen_start();
			break;
		case STATE_DRAW_P1:
			TRACEL(TRACE_LEVEL_TOUCH, ("Drawing first point...\n"));
			draw_cross(&lp->p1);
			m_state = STATE_WAIT_FOR_P1;
			break;
		case STATE_WAIT_FOR_P1:
			xpt2046_polling(&ap->p1);
			m_state = STATE_DRAW_P2;
			break;
		case STATE_DRAW_P2:
			TRACEL(TRACE_LEVEL_TOUCH, ("Drawing second point...\n"));
			state_draw_screen_start();
			draw_cross(&lp->p2);
			delay_ms(DELAY_BETWEEN_TOUCH_MS);
			m_state = STATE_WAIT_FOR_P2;
			break;
		case STATE_WAIT_FOR_P2:
			xpt2046_polling(&ap->p2);
			m_state = STATE_DRAW_P3;
			break;
		case STATE_DRAW_P3:
			TRACEL(TRACE_LEVEL_TOUCH, ("Drawing third point...\n"));
			state_draw_screen_start();
			draw_cross(&lp->p3);
			delay_ms(DELAY_BETWEEN_TOUCH_MS);
			m_state = STATE_WAIT_FOR_P3;
			break;
		case STATE_WAIT_FOR_P3:
			xpt2046_polling(&ap->p3);
			m_state = STATE_CREATE_CALIB_DATA;
			break;
		case STATE_CREATE_CALIB_DATA: {
			/* here we have all calibration data */
			touch_calculate_calib_data(ap, lp, params);
			m_state = STATE_END;
			}
			break;
		case STATE_END:
			return;
		}
	}
}

