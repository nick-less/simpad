/*
 *  linux/drivers/misc/ucb1x00-simpad.c
 *
 *  Modified by Juergen Messerer for SIMpad
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

/*
 * Conversion from AD -> mV
 * 7.5V = 1023   7.3313mV/Digit
 *
 * 400 Units == 9.7V
 * a     = ADC value
 * 21    = ADC error
 * 12600 = Divident to get 2*7.3242
 * 860   = Divider to get 2*7.3242
 * 170   = Voltagedrop over
 */
#define CALIBRATE_BATTERY(a)   ((((a + 21)*12600)/860) + 170)

/*
 * We have two types of batteries a small and a large one
 * To get the right value we to distinguish between those two
 * 450 Units == 15 V
 */
#ifdef SMALL_BATTERY 
#define CALIBRATE_SUPPLY(a)   (((a) * 1500) / 51)
#define MIN_SUPPLY            8500 /* Less then 8.5V means no powersupply */
#else
#define CALIBRATE_SUPPLY(a)   (((a) * 1500) / 45)
#define MIN_SUPPLY            14000 /* Less then 14V means no powersupply */
#endif

/*
 * Charging Current 
 * if value is >= 50 then charging is on
 */
#define CALIBRATE_CHARGING(a)    (((a)* 1000)/(152/4)))
#define CHARGING_LED_LEVEL     50


static struct proc_dir_entry *dir;
static struct ucb1x00 *ucb;

static int ucb1x00_simpad_read_vbatt(struct ucb1x00 *ucb)
{
	int val;
	ucb1x00_adc_enable(ucb);
	val = ucb1x00_adc_read(ucb, UCB_ADC_INP_AD1, UCB_NOSYNC);
	ucb1x00_adc_disable(ucb);

	return CALIBRATE_BATTERY(val);
}

static int ucb1x00_simpad_read_vcharger(struct ucb1x00 *ucb)
{
	int val;
	ucb1x00_adc_enable(ucb);
	val = ucb1x00_adc_read(ucb, UCB_ADC_INP_AD2, UCB_NOSYNC);
	ucb1x00_adc_disable(ucb);

	return CALIBRATE_SUPPLY(val);
}

static int ucb1x00_simpad_read_icharger(struct ucb1x00 *ucb)
{
	int val;
	ucb1x00_adc_enable(ucb);
	val = ucb1x00_adc_read(ucb, UCB_ADC_INP_AD3, UCB_NOSYNC);
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

static int __init ucb1x00_simpad_init(void)
{
	struct proc_dir_entry *res;

	ucb = ucb1x00_get();

	if (!ucb)
		return -ENODEV;

	dir = proc_mkdir("ucb1x00", NULL);
	if (!dir)
		return -ENOMEM;

	res = create_proc_read_entry("vbatt", S_IRUGO, dir, ucb_read, ucb1x00_simpad_read_vbatt);
	if (!res)
		return -ENOMEM;

	res = create_proc_read_entry("vcharger", S_IRUGO, dir, ucb_read, ucb1x00_simpad_read_vcharger);
	if (!res)
		return -ENOMEM;

	res = create_proc_read_entry("icharger", S_IRUGO, dir, ucb_read, ucb1x00_simpad_read_icharger);
	if (!res)
		return -ENOMEM;

	return 0;
}

static void __exit ucb1x00_simpad_exit(void)
{
	remove_proc_entry("vbatt", dir);
	remove_proc_entry("vcharger", dir);
	remove_proc_entry("icharger", dir);
}

module_init(ucb1x00_simpad_init);
module_exit(ucb1x00_simpad_exit);

MODULE_AUTHOR("Juergen Messerer <juergen.messerer@freesurf.ch>");
MODULE_DESCRIPTION("SIMpad noddy testing only example ADC driver");
MODULE_LICENSE("GPL");