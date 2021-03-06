/*
 * Wrap-around code for a console using the
 * ARC io-routines.
 *
 * Copyright (c) 1998 Harald Koerfgen 
 * Copyright (c) 2001 Ralf Baechle
 */
#include <linux/tty.h>
#include <linux/major.h>
#include <linux/ptrace.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/fs.h>

#include <asm/sgialib.h>

extern void prom_printf (char *, ...);

void prom_putchar(char c)
{
	ULONG cnt;
	CHAR it = c;

	ArcWrite(1, &it, 1, &cnt);
}

static void prom_console_write(struct console *co, const char *s,
			       unsigned count)
{
	unsigned i;

	/*
	 *    Now, do each character
	 */
	for (i = 0; i < count; i++) {
		if (*s == 10)
			prom_printf("%c", 13);
		prom_printf("%c", *s++);
	}
}

static kdev_t prom_console_device(struct console *c)
{
	return MKDEV(TTY_MAJOR, 64 + c->index);
}

static struct console arc_cons = {
    name:	"ttyS",
    write:	prom_console_write,
    device:	prom_console_device,
    flags:	CON_PRINTBUFFER,
    index:	-1,
};

/*
 *    Register console.
 */

void __init arc_console_init(void)
{
	register_console(&arc_cons);
}
