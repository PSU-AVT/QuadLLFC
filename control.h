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
void control_update(void);

void control_set_p_gains(const state_t *gains);
void control_set_i_gains(const state_t *gains);
void control_set_d_gains(const state_t *gains);

#endif
