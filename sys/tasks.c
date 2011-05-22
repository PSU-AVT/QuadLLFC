/*
 * Author: Gregory Haynes <greg@greghaynes.net>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2010, Gregory Haynes
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the
 * names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "tasks.h"
#include "../systick/systick.h"

static struct task_t *tasks;
static struct task_t *tasks_tail;

void tasks_add_task(struct task_t *task_arg)
{
	task_arg->list.next = 0;
	task_arg->last_exec = 0;
	task_arg->next_exec = 0;
	if(tasks)
	{
		tasks_tail->list.next = &task_arg->list;
		tasks_tail = task_arg;
	}
	else
	{
		tasks = task_arg;
		tasks_tail = task_arg;
	}
}

void tasks_loop(void)
{
	struct task_t *task_itr;
	uint32_t cur_ticks;

	task_itr = tasks;

	while(task_itr)
	{
		cur_ticks = systickGetTicks();

		if(task_itr->next_exec <= cur_ticks)
		{
			if(task_itr->last_exec == 0)
				task_itr->last_exec = systickGetTicks();

			// Exec the task
			task_itr->handler(task_itr);
			task_itr->next_exec = cur_ticks + task_itr->msecs;
		}

		task_itr = (struct task_t*)task_itr->list.next;
	}
}

float task_get_dt(struct task_t *task) {
	float dt = (systickGetTicks() - task->last_exec) * .001;
	task->last_exec = systickGetTicks();
	return dt;
}
