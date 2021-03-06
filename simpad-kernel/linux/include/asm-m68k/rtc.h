/* asm-m68k/rtc.h
 *
 * Copyright Richard Zidlicky
 * implementation details for genrtc/q40rtc driver
 */
/* permission is hereby granted to copy, modify and redistribute this code
 * in terms of the GNU Library General Public License, Version 2 or later,
 * at your option.
 */

#ifndef _ASM_RTC_H
#define _ASM_RTC_H

#ifdef __KERNEL__

#include <linux/config.h>
#include <linux/rtc.h>
#include <linux/delay.h>
#include <asm/machdep.h>

#define RTC_PIE 0x40		/* periodic interrupt enable */
#define RTC_AIE 0x20		/* alarm interrupt enable */
#define RTC_UIE 0x10		/* update-finished interrupt enable */

extern void gen_rtc_interrupt(unsigned long);

/* some dummy definitions */
#define RTC_SQWE 0x08		/* enable square-wave output */
#define RTC_DM_BINARY 0x04	/* all time/date values are BCD if clear */
#define RTC_24H 0x02		/* 24 hour mode - else hours bit 7 means pm */
#define RTC_DST_EN 0x01	        /* auto switch DST - works f. USA only */


#endif /* __KERNEL__ */

#endif /* _ASM__RTC_H */
