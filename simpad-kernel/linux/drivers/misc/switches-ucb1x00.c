/*
 *  linux/drivers/misc/switches-ucb1x00.c
 *
 *  Copyright (C) 2001 John Dorsey
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  19 December 2001 - created from sa1100_switches.c.
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>

#include <asm/dma.h>
#include <asm/hardware.h>
#include <asm/irq.h>

#ifdef CONFIG_SA1100_ASSABET
#include <asm/arch/assabet.h>
#endif

#ifdef CONFIG_SA1100_SIMPAD
#include <asm/arch/simpad.h>
#endif

#include "switches.h"
#include "ucb1x00.h"


static struct ucb1x00 *ucb1x00;

static void switches_ucb1x00_handler(int irq, void *devid);


#ifdef CONFIG_SA1100_ASSABET

/* Assabet
 * ^^^^^^^
 * Six switches are routed to GPIO pins on the UCB1300: S3 -- S8.
 * This code sets bits in the range [3, 8] in the mask that we
 * return to userland. Note that we transpose signals SW7 and SW8;
 * see assabet_switches_ucb1x00_handler().
 */

static int assabet_switches_ucb1x00_init(void)
{
	int i;

	/* Note that ucb1x00_init() must complete before this point: */

	if ((ucb1x00 = ucb1x00_get()) == NULL) {
		printk(KERN_ERR "%s: UCB1300 driver not ready; switches "
		       "3 -- 8 will not be available\n",
		       SWITCHES_NAME);
		return 0;
	}

	ucb1x00_enable(ucb1x00);

	ucb1x00_io_set_dir(ucb1x00,
			   UCB_IO_0 | UCB_IO_1 | UCB_IO_2 |
			   UCB_IO_3 | UCB_IO_4 | UCB_IO_5, 0);

	for (i = 0; i < 6; ++i) {

		ucb1x00_enable_irq(ucb1x00, i, UCB_RISING | UCB_FALLING);

		if (ucb1x00_hook_irq(ucb1x00, i,
				     switches_ucb1x00_handler, NULL) < 0) {
			printk(KERN_ERR "%s: unable to hook IRQ for "
			       "UCB1300 IO_%d\n", SWITCHES_NAME, i);
			return -EBUSY;
		}

	}

	return 0;

}

static void assabet_switches_ucb1x00_shutdown(void)
{
	int i;

	for (i = 5; i >= 0; --i) {

		ucb1x00_disable_irq(ucb1x00, i, UCB_RISING | UCB_FALLING);

		/* Only error conditions are ENOENT and EINVAL; silently
		 * ignore:
		 */
		ucb1x00_free_irq(ucb1x00, i, NULL);

	}

}

static void assabet_switches_ucb1x00_handler(int irq, switches_mask_t *mask)
{
	unsigned int last, this;
	static unsigned int states = 0;

	last = ((states & (1 << irq)) != 0);
	this = ((ucb1x00_io_read(ucb1x00) & (1 << irq)) != 0);

	if (last == this) /* debounce */
		return;

	/* Intel StrongARM SA-1110 Development Board
	 * Schematics Figure 5, Sheet 5 of 12
	 *
	 * See switches S8 and S7. Notice their
	 * relationship to signals SW7 and SW8. Hmmm.
	 */

	switch (irq) {

	case 4:

		SWITCHES_SET(mask, 8, this);
		break;

	case 5:

		SWITCHES_SET(mask, 7, this);
		break;

	default:

		SWITCHES_SET(mask, irq + 3, this);

	}

	states = this ? (states | (1 << irq)) : (states & ~(1 << irq));

}
#endif  /* CONFIG_SA1100_ASSABET */

#ifdef CONFIG_SA1100_SIMPAD

/* SIMpad
 * ^^^^^^
 * Six switches are routed to GPIO pins on the UCB1300: S3 -- S8.
 * This code sets bits in the range [3, 8] in the mask that we
 * return to userland. 
 */

static int simpad_switches_ucb1x00_init(void)
{
	int i;

	/* Note that ucb1x00_init() must complete before this point: */

	if ((ucb1x00 = ucb1x00_get()) == NULL) {
		printk(KERN_ERR "%s: UCB1300 driver not ready; switches "
		       "3 -- 8 will not be available\n",
		       SWITCHES_NAME);
		return 0;
	}

	ucb1x00_enable(ucb1x00);

	ucb1x00_io_set_dir(ucb1x00,
			   UCB_IO_0 | UCB_IO_1 | UCB_IO_2 |
			   UCB_IO_3 | UCB_IO_4 | UCB_IO_5, 
			   UCB_IO_8 | UCB_IO_9);

	ucb1x00_disable(ucb1x00);

	for (i = 0; i < 6; ++i) {

	        if (ucb1x00_hook_irq(ucb1x00, i,
				     switches_ucb1x00_handler, NULL) < 0) {
			printk(KERN_ERR "%s: unable to hook IRQ for "
			       "UCB1300 IO_%d\n", SWITCHES_NAME, i);
			return -EBUSY;
		}
		
		ucb1x00_enable_irq(ucb1x00, i, UCB_RISING | UCB_FALLING);
	}

	return 0;

}

int simpad_switches_ucb1x00_reinit(void)
{
	int i;
	ucb1x00_enable(ucb1x00);

	ucb1x00_io_set_dir(ucb1x00,
			UCB_IO_0 | UCB_IO_1 | UCB_IO_2 |
			UCB_IO_3 | UCB_IO_4 | UCB_IO_5,
			UCB_IO_8 | UCB_IO_9);

	ucb1x00_disable(ucb1x00);

	for (i = 0; i < 6; ++i)
		ucb1x00_enable_irq(ucb1x00, i, UCB_RISING | UCB_FALLING);

	return 0;
}

static void simpad_switches_ucb1x00_shutdown(void)
{
	int i;

	for (i = 5; i >= 0; --i) {

		ucb1x00_disable_irq(ucb1x00, i, UCB_RISING | UCB_FALLING);

		/* Only error conditions are ENOENT and EINVAL; silently
		 * ignore:
		 */
		ucb1x00_free_irq(ucb1x00, i, NULL);

	}

}

static void simpad_switches_ucb1x00_handler(int irq, switches_mask_t *mask)
{
	unsigned int last, this;
	static unsigned int states = 0;

	last = ((states & (1 << irq)) != 0);
	this = ((~ucb1x00_io_read(ucb1x00) & (1 << irq)) != 0);

	if (last == this) /* debounce */
		return;

	switch (irq) {

	case 4:

		

	case 5:

		

	default:

		SWITCHES_SET(mask, irq + 3, this);

	}

	states = this ? (states | (1 << irq)) : (states & ~(1 << irq));

}
#endif  /* CONFIG_SA1100_SIMPAD */


/* switches_ucb1x00_handler()
 * ^^^^^^^^^^^^^^^^^^^^^^^^^^
 * This routine is a generalized handler for UCB1x00 GPIO switches
 * which calls a board-specific service routine and passes an event
 * mask to the core event handler. This routine is appropriate for
 * systems which use the ucb1x00 framework, and can be registered
 * using ucb1x00_hook_irq().
 */
static void switches_ucb1x00_handler(int irq, void *devid)
{
	switches_mask_t mask;

	SWITCHES_ZERO(&mask);

	/* Porting note: call a board-specific UCB1x00 switch handler here.
	 * The handler can assume that sufficient storage for `mask' has
	 * been allocated, and that the corresponding switches_mask_t
	 * structure has been zeroed.
	 */

	if (machine_is_assabet()) {
#ifdef CONFIG_SA1100_ASSABET
		assabet_switches_ucb1x00_handler(irq, &mask);
#endif
	}
	if (machine_is_simpad()) {
#ifdef CONFIG_SA1100_SIMPAD
		simpad_switches_ucb1x00_handler(irq, &mask);
#endif
	}

	switches_event(&mask);

}

int __init switches_ucb1x00_init(void)
{

	/* Porting note: call a board-specific init routine here. */

	if (machine_is_assabet()) {
#ifdef CONFIG_SA1100_ASSABET
		if (assabet_switches_ucb1x00_init() < 0)
			return -EIO;
#endif
	}
	if (machine_is_simpad()) {
#ifdef CONFIG_SA1100_SIMPAD
		if (simpad_switches_ucb1x00_init() < 0)
			return -EIO;
#endif
	}

	return 0;

}

void __exit switches_ucb1x00_exit(void)
{

	/* Porting note: call a board-specific shutdown routine here. */

	if (machine_is_assabet()) {
#ifdef CONFIG_SA1100_ASSABET
		assabet_switches_ucb1x00_shutdown();
#endif
	}
	if (machine_is_simpad()) {
#ifdef CONFIG_SA1100_SIMPAD
		simpad_switches_ucb1x00_shutdown();
#endif
	}

}
