/*
 *  linux/drivers/misc/switches.h
 *
 *  Copyright (C) 2001 John Dorsey
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  19 December 2001 - created.
 */

#if !defined(_SWITCHES_H)
# define _SWITCHES_H

#include <linux/switches.h>

#define SWITCHES_NAME "switches"

extern int  switches_event(switches_mask_t *mask);

extern int  switches_sa1100_init(void);
extern void switches_sa1100_exit(void);

extern int  switches_ucb1x00_init(void);
extern void switches_ucb1x00_exit(void);

#endif  /* !defined(_SWITCHES_H) */
