/*
 *  linux/drivers/misc/ucb1x00-assabet.c
 *
 *  Copyright (C) 2001 Russell King, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 *  We handle the machine-specific bits of the UCB1x00 driver here.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>

#include <asm/dma.h>

#include "ucb1x00.h"

static struct proc_dir_entry *dir;
static struct ucb1x00 *ucb;

static int ucb1x00_assabet_read_vbatt(struct ucb1x00 *ucb)
{
	int val;
	ucb1x00_adc_enable(ucb);
	val = ucb1x00_adc_read(ucb, UCB_ADC_INP_AD1, UCB_NOSYNC);
	ucb1x00_adc_disable(ucb);

	return val;
}

static int ucb1x00_assabet_read_vcharger(struct ucb1x00 *ucb)
{
	int val;
	ucb1x00_adc_enable(ucb);
	val = ucb1x00_adc_read(ucb, UCB_ADC_INP_AD0, UCB_NOSYNC);
	ucb1x00_adc_disable(ucb);

	return val;
}

static int ucb1x00_assabet_read_batt_temp(struct ucb1x00 *ucb)
{
	int val;
	ucb1x00_adc_enable(ucb);
	val = ucb1x00_adc_read(ucb, UCB_ADC_INP_AD2, UCB_NOSYNC);
	ucb1x00_adc_disable(ucb);

	return val;
}

static int ucb_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	char *p = page;
	int (*fn)(struct ucb1x00 *) = data;
	int v, len;

	v = fn(ucb);

	p += sprintf(p, "%d\n", v);

	len = (p - page) - off;
	if (len < 0)
		len = 0;

	*eof = (len <= count) ? 1 : 0;
	*start = page + off;

	return len;
}

static int __init ucb1x00_assabet_init(void)
{
	struct proc_dir_entry *res;

	ucb = ucb1x00_get();

	if (!ucb)
		return -ENODEV;

	dir = proc_mkdir("ucb1x00", NULL);
	if (!dir)
		return -ENOMEM;

	res = create_proc_read_entry("vbatt", S_IRUGO, dir, ucb_read, ucb1x00_assabet_read_vbatt);
	if (!res)
		return -ENOMEM;

	res = create_proc_read_entry("vcharger", S_IRUGO, dir, ucb_read, ucb1x00_assabet_read_vcharger);
	if (!res)
		return -ENOMEM;

	res = create_proc_read_entry("batt_temp", S_IRUGO, dir, ucb_read, ucb1x00_assabet_read_batt_temp);
	if (!res)
		return -ENOMEM;

	return 0;
}

static void __exit ucb1x00_assabet_exit(void)
{
	remove_proc_entry("vbatt", dir);
	remove_proc_entry("vcharger", dir);
	remove_proc_entry("batt_temp", dir);
}

module_init(ucb1x00_assabet_init);
module_exit(ucb1x00_assabet_exit);

MODULE_AUTHOR("Russell King <rmk@arm.linux.org.uk>");
MODULE_DESCRIPTION("Assabet noddy testing only example ADC driver");
MODULE_LICENSE("GPL");
