/*
 *  linux/drivers/misc/switches-core.c
 *
 *  Copyright (C) 2000-2001 John Dorsey
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  5 October 2000 - created.
 *
 *  25 October 2000 - userland file interface added.
 *
 *  13 January 2001 - added support for Spot.
 *
 *  11 September 2001 - UCB1200 driver framework support added.
 *
 *  19 December 2001 - separated out SA-1100 and UCB1x00 code.
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/wait.h>

#include <asm/uaccess.h>

#include "switches.h"


MODULE_AUTHOR("John Dorsey");
MODULE_DESCRIPTION("Console switch support");
MODULE_LICENSE("GPL");


struct switches_action {
	struct list_head list;
	switches_mask_t  mask;
};


static int switches_users = 0;

static spinlock_t switches_lock = SPIN_LOCK_UNLOCKED;

DECLARE_WAIT_QUEUE_HEAD(switches_wait);
LIST_HEAD(switches_event_queue);


static ssize_t switches_read(struct file *file, char *buffer,
			     size_t count, loff_t *pos)
{
	unsigned long flags;
	struct list_head *event;
	struct switches_action *action;

	if (count < sizeof(struct switches_mask_t))
		return -EINVAL;

	while (list_empty(&switches_event_queue)) {

		if (file->f_flags & O_NDELAY)
			return -EAGAIN;

		interruptible_sleep_on(&switches_wait);

		if (signal_pending(current))
			return -ERESTARTSYS;

	}

	if (verify_area(VERIFY_WRITE, buffer, sizeof(struct switches_mask_t)))
		return -EFAULT;

	spin_lock_irqsave(&switches_lock, flags);

	event = switches_event_queue.next;
	action = list_entry(event, struct switches_action, list);
	copy_to_user(buffer, &(action->mask), sizeof(struct switches_mask_t));
	list_del(event);
	kfree(action);

	spin_unlock_irqrestore(&switches_lock, flags);

	return 0;

}

static ssize_t switches_write(struct file *file, const char *buffer,
			      size_t count, loff_t *ppos)
{
	return -EINVAL;
}

static unsigned int switches_poll(struct file *file, poll_table *wait)
{

	poll_wait(file, &switches_wait, wait);

	if (!list_empty(&switches_event_queue))
		return POLLIN | POLLRDNORM;

	return 0;

}

static int switches_open(struct inode *inode, struct file *file)
{

	if (switches_users > 0)
		return -EBUSY;

	MOD_INC_USE_COUNT;
	++switches_users;
	return 0;

}

static int switches_release(struct inode *inode, struct file *file)
{

	--switches_users;
	MOD_DEC_USE_COUNT;
	return 0;

}

static struct file_operations switches_ops = {
	read:    switches_read,
	write:   switches_write,
	poll:    switches_poll,
	open:    switches_open,
	release: switches_release
};

static struct miscdevice switches_misc = {
	MISC_DYNAMIC_MINOR, SWITCHES_NAME, &switches_ops
};

int switches_event(switches_mask_t *mask)
{
	struct switches_action *action;

	if ((switches_users > 0) && (SWITCHES_COUNT(mask) > 0)) {

		if ((action = (struct switches_action *)
		     kmalloc(sizeof(struct switches_action),
			     GFP_KERNEL)) == NULL) {
			printk(KERN_ERR "%s: unable to allocate action "
			       "descriptor\n", SWITCHES_NAME);
			return -1;
		}

		action->mask = *mask;

		spin_lock(&switches_lock);
		list_add_tail(&action->list, &switches_event_queue);
		spin_unlock(&switches_lock);

		wake_up_interruptible(&switches_wait);

	}

	return 0;

}

static int __init switches_init(void)
{

#ifdef CONFIG_SWITCHES_SA1100
	if (switches_sa1100_init() < 0) {
		printk(KERN_ERR "%s: unable to initialize SA-1100 switches\n",
		       SWITCHES_NAME);
		return -EIO;
	}
#endif

#ifdef CONFIG_SWITCHES_UCB1X00
	if (switches_ucb1x00_init() < 0) {
		printk(KERN_ERR "%s: unable to initialize UCB1x00 switches\n",
		       SWITCHES_NAME);
		return -EIO;
	}
#endif

	if (misc_register(&switches_misc) < 0) {
		printk(KERN_ERR "%s: unable to register misc device\n",
		       SWITCHES_NAME);
		return -EIO;
	}

	printk("Console switches initialized\n");

	return 0;

}

static void __exit switches_exit(void)
{

#ifdef CONFIG_SWITCHES_SA1100
	switches_sa1100_exit();
#endif

#ifdef CONFIG_SWITCHES_UCB1X00
	switches_ucb1x00_exit();
#endif

	if (misc_deregister(&switches_misc) < 0)
		printk(KERN_ERR "%s: unable to deregister misc device\n",
		       SWITCHES_NAME);

}

module_init(switches_init);
module_exit(switches_exit);

EXPORT_NO_SYMBOLS;
