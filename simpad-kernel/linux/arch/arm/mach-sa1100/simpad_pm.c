/*
* Powermanagement layer for SIMPad.
*
* Copyright 2003 Peter Pregler
* Copyright 2000,2001 Compaq Computer Corporation.
*
* Use consistent with the GNU GPL is permitted,
* provided that this copyright notice is
* preserved in its entirety in all copies and derived works.
*
* COMPAQ COMPUTER CORPORATION MAKES NO WARRANTIES, EXPRESSED OR IMPLIED,
* AS TO THE USEFULNESS OR CORRECTNESS OF THIS CODE OR ITS
* FITNESS FOR ANY PARTICULAR PURPOSE.
*
* Author: Peter Pregler (based on work for ipaq by Andrew Christian)
* May, 2003
*/

#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <asm/uaccess.h>        /* get_user,copy_to_user */
#include <linux/string.h>
#include <linux/interrupt.h>
#include <linux/sysctl.h>
#include <linux/console.h>
#include <linux/devfs_fs_kernel.h>

#include <linux/tqueue.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/proc_fs.h>
#include <linux/apm_bios.h>
#include <linux/kmod.h>

#include <asm/hardware.h>
#include <asm/arch-sa1100/simpad_pm.h>

MODULE_AUTHOR("Peter Pregler");
MODULE_DESCRIPTION("Power manamgement abstraction layer for the SIMpad");

/****************************************************************************/
/*  Functions exported for use by the kernel and kernel modules             */
/****************************************************************************/

int simpad_apm_get_power_status(u_char *ac_line_status,
			       u_char *battery_status, 
			       u_char *battery_flag, 
			       u_char *battery_percentage, 
			       u_short *battery_life)
{
	struct simpad_battery bstat;
	unsigned char ac    = APM_AC_UNKNOWN;
	unsigned char level = APM_BATTERY_STATUS_UNKNOWN;
	int status, result;

	result = simpad_get_battery(&bstat);
	if (result) {
		printk("%s: unable to access battery information: result=%d\n", __FUNCTION__, result);
		return 0;
	}

	switch (bstat.ac_status) {
	case SIMPAD_AC_STATUS_AC_OFFLINE:
		ac = APM_AC_OFFLINE;
		break;
	case SIMPAD_AC_STATUS_AC_ONLINE:
		ac = APM_AC_ONLINE;
		break;
	case SIMPAD_AC_STATUS_AC_BACKUP:
		ac = APM_AC_BACKUP;
		break;
	}

	if (ac_line_status != NULL)
		*ac_line_status = ac;

	status = bstat.status;
	if (status & (SIMPAD_BATT_STATUS_CHARGING | SIMPAD_BATT_STATUS_CHARGE_MAIN))
		level = APM_BATTERY_STATUS_CHARGING;
	else if (status & (SIMPAD_BATT_STATUS_HIGH | SIMPAD_BATT_STATUS_FULL))
		level = APM_BATTERY_STATUS_HIGH;
	else if (status & SIMPAD_BATT_STATUS_LOW)
		level = APM_BATTERY_STATUS_LOW;
	else if (status & SIMPAD_BATT_STATUS_CRITICAL)
		level = APM_BATTERY_STATUS_CRITICAL;

	if (battery_status != NULL)
		*battery_status = level;

	if (battery_percentage != NULL)
		*battery_percentage = bstat.percentage;

	/* we have a dumb battery - so we know nothing */
	if (battery_life != NULL) {
		*battery_life = APM_BATTERY_LIFE_UNKNOWN;
	}
                        
#if 0
	printk("apm_get_power: ac: %02x / bs: %02x / bf: %02x / perc: %02x / life: %d\n",
	       *ac_line_status, *battery_status, *battery_flag,
	       *battery_percentage, *battery_life );
#endif
	return 1;
}

EXPORT_SYMBOL(simpad_apm_get_power_status);


/***********************************************************************************/
/*       Initialization                                                            */
/***********************************************************************************/

#ifdef CONFIG_FB_MQ200
extern void (*mq200_blank_helper)(int blank);
#endif

int __init simpad_hal_init_module(void)
{
	int i;
        printk(KERN_INFO "SIMpad Registering HAL abstraction layer\n");

	/* Request the appropriate underlying module to provide services */

#ifdef CONFIG_FB_SA1100
	sa1100fb_blank_helper = simpad_hal_backlight_helper;
#endif

	return 0;
}

void simpad_hal_cleanup_module(void)
{
	int i;
	printk(KERN_INFO "SIMpad shutting down HAL abstraction layer\n");

#ifdef CONFIG_FB_SA1100
	sa1100fb_blank_helper = NULL;
#endif
}

module_init(simpad_hal_init_module);
module_exit(simpad_hal_cleanup_module);
