/***************************************************************************
 MQPLAT.H

 MQ200 platform, system or OS specific header file

 Copyright (c) 2000 by MediaQ, Incorporated.
 All Rights Reserved.

***************************************************************************/
#ifndef _VIDEO_MQ200_MQPLAT_H
#define _VIDEO_MQ200_MQPLAT_H

/* OS variation - ONLY define one */
#undef  MQ_VXWORKS
#undef	MQ_EPOC32 
#undef 	MQ_WINCE
#undef 	MQ_OS9
#define MQ_LINUX

/* CPU variation - ONLY define one */
#undef  MQ_X86
#define	MQ_SA1110
#undef	MQ_MIPS_NEC
#undef	MQ_MIPS_TOSHIBA
#undef	MQ_SH4

/* PCI support - undef accordingly */
#undef MQ_PCI
#ifdef MQ_PCI
 #warning "MQ200 driver compiled for PCI !"
#endif

/* Derived equates from CPU type */
#ifdef	MQ_SHx
 #warning "MQ200 driver compiled for SHx !"
 #define FB_BASE	0x93800000L		/* MQ200 frame buffer adddr */
#endif

#ifdef	MQ_MIPS_NEC
 #warning "MQ200 driver compiled for NEC MIPS !"
 #define CHECK_FIFO_REQUIRED	/* GE fifo checking required */
 #define FB_BASE	0xAA000000L
 
 #ifdef  MQ_PCI
 #define IOREGS_BASE	0xAF000000L		/* for VR4122 */
 #define IOREGS_SIZE	0x00002000L		
 #else
 #define IOREGS_BASE	0xAB000000L		/* for VR 4111/21 */
 #define IOREGS_SIZE	0x00001000L
 #define CHECK_NOTBUSY                          /* Needed for NEC MIPS */
 #define CHECK_CMDFIFO                          /* Needed for NEC MIPS */
 #endif /* MQ_PCI */
#endif

#ifdef	MQ_MIPS_TOSHIBA
 #warning "MQ200 driver compiled for TOSHIBA MIPS !"
 #define FB_BASE	0x6D800000L
#endif

#ifdef	MQ_SA1110
 #warning "MQ200 driver compiled for Intel SA1110 !"
 #define FB_BASE_CS3	0x1b800000L		/* configured as CS3 */
 #define FB_BASE_CS4	0x43800000L		/* configured as CS4 */
#define FB_BASE_CS5	0x4b800000L		/* configured as CS5 */
#define FB_BASE	        FB_BASE_CS5		/* change accordingly! */
#define REGISTER_BASE   0x4be00000L 
#endif

/* OS-derived misc defines */
#ifdef MQ_VXWORKS
 #warning "MQ200 driver compiled for VxWorks !"
 #define MQ_DELAY(cnt) taskDelay(cnt*30);
 #define MQ_COLOR_RGB				/* Color (32bit):ARGB */
#endif

#ifdef MQ_WINCE
 #warning "MQ200 driver compiled for Window CE !"
 #define MQ_DELAY(cnt) Sleep(cnt)
#endif

#ifdef MQ_LINUX
 #warning "MQ200 driver compiled for Linux !"
/* #define MQ_DELAY(cnt) {volatile int delay; for (delay=0; delay<10000*cnt; delay++); } */

#define MQ_DELAY(cnt) mdelay(cnt * 10)
#endif

/* Further derivation
#ifdef MQ_COLOR_RGB
 #define GETR(color)		(unsigned char)(color >> 16)
 #define GETG(color)		(unsigned char)((unsigned short)(color) >> 8)
 #define GETB(color)		(unsigned char)(color)
 #define MAKERGB(r,g,b)		((unsigned long)(((unsigned char)(r)|\
				((unsigned short)((unsigned char)(g))<<8))|\
				(((unsigned long)(unsigned char)(b))<<16)))
#endif
*/

/* Cursor color */
#ifndef CURSOR_FGCLR
 #define CURSOR_FGCLR		0x0000FFFF 		/* Yellow */
#endif
#ifndef CURSOR_BGCLR
 #define CURSOR_BGCLR		0x00000000 		/* Black */
#endif
#endif /* _VIDEO_MQ200_MQPLAT_H */
