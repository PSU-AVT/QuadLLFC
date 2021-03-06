/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#ifndef ESC_H
#define ESC_H

typedef enum ESC {
	ESC_FRONT = 0,
	ESC_RIGHT = 1,
	ESC_BACK = 2,
	ESC_LEFT = 3
} ESC_LABEL;

#define ESC_CNT 4

void esc_init(ESC_LABEL esc);
void esc_init_all(void);

/* value is between 0 and 1 where 1 is full throttle
 * If a any esc throttle is greater than 1 then all esc's
 * are rescaled to maintain relative throttles with highest
 * throttle being 1
 */
void esc_set_throttle(ESC_LABEL esc, float value);

/* Takes an array of float values the size of ESC_CNT
   and sets each esc in the order listed in ESC_LABEL
 */
void esc_set_all_throttles(float *values);

float esc_get_throttle(ESC_LABEL esc);
void esc_arm_all(void);

/* call this periodically */
void esc_update(void);

#endif
