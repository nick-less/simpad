/*
 *  linux/drivers/misc/switches-sa1100.c
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

#include <asm/hardware.h>
#include <asm/irq.h>

#ifdef CONFIG_SA1100_ASSABET
#include <asm/arch/assabet.h>
#endif

#ifdef CONFIG_SA1100_SIMPAD
#include <asm/arch/simpad.h>
#endif

#include "switches.h"


static void switches_sa1100_handler(int irq, void *dev_id,
				    struct pt_regs *regs);


#ifdef CONFIG_SA1100_ASSABET

/* Assabet
 * ^^^^^^^
 * We have two general-purpose switches, S1 and S2, available via GPIO
 * on Assabet. This code sets bits in the range [1, 2] in the mask that
 * we return to userland.
 */

static int assabet_switches_sa1100_init(void)
{

	if (machine_has_neponset())
		NCR_0 |= NCR_GP01_OFF;

	set_irq_type(IRQ_GPIO0, IRQT_BOTHEDGE);
	set_irq_type(IRQ_GPIO1, IRQT_BOTHEDGE);

	if (request_irq(IRQ_GPIO0, switches_sa1100_handler, SA_INTERRUPT,
			SWITCHES_NAME, NULL) < 0) {
		printk(KERN_ERR "%s: unable to register IRQ for GPIO 0\n",
		       SWITCHES_NAME);
		return -EIO;
	}

	if (request_irq(IRQ_GPIO1, switches_sa1100_handler, SA_INTERRUPT,
			SWITCHES_NAME, NULL) < 0) {
		printk(KERN_ERR "%s: unable to register IRQ for GPIO 1\n",
		       SWITCHES_NAME);
		return -EIO;
	}

	return 0;

}

static void assabet_switches_sa1100_shutdown(void)
{

	free_irq(IRQ_GPIO1, NULL);
	free_irq(IRQ_GPIO0, NULL);

}

static void assabet_switches_sa1100_handler(int irq, switches_mask_t *mask)
{
	unsigned int s, last, this;
	static unsigned int states = 0;

	switch (irq) {

	case IRQ_GPIO0:	s = 0; break;

	case IRQ_GPIO1:	s = 1; break;

	default:	return;

	}

	last = ((states & (1 << s)) != 0);
	this = ((GPLR & GPIO_GPIO(s)) != 0);

	if (last == this) /* debounce */
		return;

	SWITCHES_SET(mask, s + 1, this);

	states = this ? (states | (1 << s)) : (states & ~(1 << s));

}
#endif  /* CONFIG_SA1100_ASSABET */


#ifdef CONFIG_SA1100_SPOT

/* Spot
 * ^^^^
 * Spot (R2, R3) has a single general-purpose switch (S1), which is
 * also the power-on switch. We set bit [1] in the mask we return to
 * userland.
 */

static int spot_switches_sa1100_init(void)
{

	set_GPIO_IRQ_edge(GPIO_SW1, GPIO_BOTH_EDGES);

	if (request_irq(IRQ_GPIO_SW1, switches_sa1100_handler, SA_INTERRUPT,
			SWITCHES_NAME, NULL) < 0) {
		printk(KERN_ERR "%s: unable to register IRQ for SW1\n",
		       SWITCHES_NAME);
		return -EIO;
	}

	return 0;

}

static void spot_switches_sa1100_shutdown(void)
{

	free_irq(IRQ_GPIO_SW1, NULL);

}

static void spot_switches_sa1100_handler(int irq, switches_mask_t *mask)
{
	unsigned int s, last, this;
	static unsigned int states = 0;

	switch (irq) {

	case IRQ_GPIO_SW1:	s = 0; break;

	default:		return;

	}

	last = ((states & (1 << s)) != 0);
	this = ((GPLR & GPIO_GPIO(s)) != 0);

	if (last == this) /* debounce */
		return;

	SWITCHES_SET(mask, s + 1, this);

	states = this ? (states | (1 << s)) : (states & ~(1 << s));

}
#endif  /* CONFIG_SA1100_SPOT */

#ifdef CONFIG_SA1100_SIMPAD

/* SIMpad
 * ^^^^
 * SIMpad has a single general-purpose switch (S0), which is
 * also the power-on switch. We set bit [1] in the mask we return to
 * userland.
 */

static int simpad_switches_sa1100_init(void)
{

	set_GPIO_IRQ_edge(GPIO_GPIO0, GPIO_BOTH_EDGES);
	
	if (request_irq(IRQ_GPIO0, switches_sa1100_handler, SA_INTERRUPT,
			SWITCHES_NAME, NULL) < 0) {
		printk(KERN_ERR "%s: unable to register IRQ for SW0\n",
		SWITCHES_NAME);
		return -EIO;
	}

	return 0;

}

static void simpad_switches_sa1100_shutdown(void)
{

	free_irq(IRQ_GPIO0, NULL);

}

static void simpad_switches_sa1100_handler(int irq, switches_mask_t *mask)
{
	unsigned int s, last, this;
	static unsigned int states = 0;

	switch (irq) {

	case IRQ_GPIO0:	s = 0; break;

	default:		return;

	}

	last = ((states & (1 << s)) != 0);
	this = ((GPLR & GPIO_GPIO(s)) != 0);

	if (last == this) /* debounce */
		return;

	SWITCHES_SET(mask, s + 1, this);

	states = this ? (states | (1 << s)) : (states & ~(1 << s));

}
#endif  /* CONFIG_SA1100_SIMPAD */



/* switches_sa1100_handler()
 * ^^^^^^^^^^^^^^^^^^^^^^^^^
 * This routine is a generalized handler for SA-1100 switches
 * which manages action descriptors and calls a board-specific
 * service routine. This routine is appropriate for GPIO switches
 * or other primary interrupt sources, and can be registered as a
 * first-class IRQ handler using request_irq().
 */
static void switches_sa1100_handler(int irq, void *dev_id,
				    struct pt_regs *regs)
{
	switches_mask_t mask;

	SWITCHES_ZERO(&mask);

	/* Porting note: call a board-specific switch interrupt handler
	 * here. The handler can assume that sufficient storage for
	 * `mask' has been allocated, and that the corresponding
	 * switches_mask_t structure has been zeroed.
	 */

	if (machine_is_assabet()) {
#ifdef CONFIG_SA1100_ASSABET
		assabet_switches_sa1100_handler(irq, &mask);
#endif
	} else if (machine_is_spot()) {
#ifdef CONFIG_SA1100_SPOT
		spot_switches_sa1100_handler(irq, &mask);
#endif
	} else if (machine_is_simpad()) {
#ifdef CONFIG_SA1100_SIMPAD
		simpad_switches_sa1100_handler(irq, &mask);
#endif
	}

	switches_event(&mask);

}

int __init switches_sa1100_init(void)
{

	/* Porting note: call a board-specific init routine here. */

	if (machine_is_assabet()) {
#ifdef CONFIG_SA1100_ASSABET
		if (assabet_switches_sa1100_init() < 0)
			return -EIO;
#endif
	} else if (machine_is_spot()) {
#ifdef CONFIG_SA1100_SPOT
		if (spot_switches_sa1100_init() < 0)
			return -EIO;
#endif
	} else if (machine_is_simpad()) {
#ifdef CONFIG_SA1100_SIMPAD
		if (simpad_switches_sa1100_init() < 0)
			return -EIO;
#endif
	}

	return 0;

}

void __exit switches_sa1100_exit(void)
{

	/* Porting note: call a board-specific shutdown routine here. */

	if (machine_is_assabet()) {
#ifdef CONFIG_SA1100_ASSABET
		assabet_switches_sa1100_shutdown();
#endif
	} else if (machine_is_spot()) {
#ifdef CONFIG_SA1100_SPOT
		spot_switches_sa1100_shutdown();
#endif
	} else if (machine_is_simpad()) {
#ifdef CONFIG_SA1100_SIMPAD
		simpad_switches_sa1100_shutdown();
#endif
	}

}
