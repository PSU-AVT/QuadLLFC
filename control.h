/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#ifndef CONTROL_H
#define CONTROL_H

#include "state.h"

void control_init(void);
void control_reset(void);
void control_set_enabled(int value);
int control_enabled(void);
void control_update(void);

state_t *control_get_p_gains(void);
state_t *control_get_i_gains(void);
state_t *control_get_d_gains(void);

#endif
