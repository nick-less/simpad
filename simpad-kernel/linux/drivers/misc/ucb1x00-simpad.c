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

#include <asm/arch-sa1100/simpad_pm.h>

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
//#define MIN_SUPPLY            14000 /* Less then 14V means no powersupply */
#define MIN_SUPPLY            12000 /* Less then 12V means no powersupply */
#endif

/*
 * Charging Current 
 * if value is >= 50 then charging is on
 */
#define CALIBRATE_CHARGING(a)    (((a)* 1000)/(152/4)))
//#define CHARGING_LED_LEVEL     50

#ifdef CONFIG_SA1100_SIMPAD_SINUSPAD

#define CHARGING_LED_LEVEL     12
#define CHARGING_MAX_LEVEL     120
#define BATT_FULL	8100
#define BATT_LOW	7300
#define BATT_CRITICAL	6700
#define BATT_EMPTY	6400


#else // CONFIG_SA1100_SIMPAD_SINUSPAD

#define CHARGING_LED_LEVEL     28
#define CHARGING_MAX_LEVEL     265
#define BATT_FULL	8300
#define BATT_LOW	7400
#define BATT_CRITICAL	6800
#define BATT_EMPTY	6500

#endif // CONFIG_SA1100_SIMPAD_SINUSPAD


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

/****************************************************************************/
/*  Functions exported for use by the kernel and kernel modules             */
/****************************************************************************/

int simpad_get_battery(struct simpad_battery *bstat)
{
  int icharger, vcharger, vbatt;

  if ( ucb ) {
  	icharger = ucb1x00_simpad_read_icharger( ucb );
  	vcharger = ucb1x00_simpad_read_vcharger( ucb );
  	vbatt    = ucb1x00_simpad_read_vbatt( ucb );
  } else {
  	bstat->ac_status = SIMPAD_AC_STATUS_AC_UNKNOWN;
	bstat->status = SIMPAD_BATT_STATUS_UNKNOWN;
	bstat->percentage = 0x64; /* lets say 100% */
	bstat->life = 360;	/* lets say a long time */
	return 0;
  }

  /* AC status */
  bstat->ac_status = SIMPAD_AC_STATUS_AC_OFFLINE;
  if ( vcharger>MIN_SUPPLY ) {
  	bstat->ac_status = SIMPAD_AC_STATUS_AC_ONLINE;
  }

  /* charging */
  bstat->status = 0x0;
  if ( icharger > CHARGING_LED_LEVEL ) {
  	bstat->status = SIMPAD_BATT_STATUS_CHARGING;
  }

  if ( vbatt > BATT_LOW )
      bstat->status |= SIMPAD_BATT_STATUS_HIGH;
  else if ( vbatt < BATT_CRITICAL )
      bstat->status |= SIMPAD_BATT_STATUS_CRITICAL;
  else
      bstat->status |= SIMPAD_BATT_STATUS_LOW;

  if (bstat->status & SIMPAD_BATT_STATUS_CHARGING) {
      if (icharger > CHARGING_MAX_LEVEL)  icharger = CHARGING_MAX_LEVEL;
      if (icharger < CHARGING_LED_LEVEL)  icharger = CHARGING_LED_LEVEL;
      bstat->percentage = 100 - 100 * (icharger - CHARGING_LED_LEVEL) /
	(CHARGING_MAX_LEVEL - CHARGING_LED_LEVEL);
  } else {
      if (vbatt > BATT_FULL)  vbatt = BATT_FULL;
      if (vbatt < BATT_EMPTY) vbatt = BATT_EMPTY;
      bstat->percentage = 100 * (vbatt - BATT_EMPTY) / (BATT_FULL - BATT_EMPTY);
  }

  /* let's assume: full load is 7h */
  /* bstat->life = 420*bstat->percentage/100; */
  bstat->life = 0;

#if 0
  printk("get_battery: ac: %02x / ch: %02x /  perc: %02x / life: %d\n",
	 bstat->ac_status, bstat->status,
	 bstat->percentage, bstat->life );
#endif

  return 0;
}

EXPORT_SYMBOL(simpad_get_battery);

/****************************************************************************/
/*  sample proc interface                                                   */
/****************************************************************************/
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
