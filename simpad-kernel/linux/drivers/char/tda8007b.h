/*
 *  Double multiprotocol IC car interface (Philips SmartCard reader)
 *
 *  linux/drivers/char/tda8007b.h
 *
 *  Copyright (C) 2002 juergen.messerer@freesurf.ch, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 */
#ifndef TDA8007B_H
#define TDA8007B_H

#define CS4BUSTYPE unsigned volatile long
#define CS4_BASE  0xf2000000

#define CARD_PORT1 1
#define CARD_PORT2 2
#define CARD_PORT3 3

#define TDA_REG_READ(reg)       *(CS4BUSTYPE *)(CS4_BASE+reg)
#define TDA_REG_WRITE(reg,val)  *(CS4BUSTYPE *)(CS4_BASE+reg)=val

#define TDA_MULTIPLEXED_MODE 0

#define TDA_UCR2_DIV              0
#define TDA_PDR_VAL               12

#define pascii(i) ((i>=' ' && i < 0x7f) ? (i):'.')

#define BUFFSIZE 128

#define TDA_READ  1
#define TDA_WRITE 2

#define TDA_INFO         1
#define TDA_INIT         2
#define TDA_SET          3
#define TDA_CARD_PRESENT 4
#define TDA_CARD_VOLT    5

int hsr[BUFFSIZE];
int msr[BUFFSIZE];
int csr[BUFFSIZE];
int urr[BUFFSIZE];
int usr[BUFFSIZE];

/*************************** Control Register ********************************/

/* 
 * Card select register (read/write) 
 * all significant bits are cleared execept SC1 which is set (xxxx'0001)
 */
#define TDA_CSR        0x00
#define TDA_CSR_SC1    (1 << 0)
#define TDA_CSR_SC2    (1 << 1)
#define TDA_CSR_SC3    (1 << 2)
#define TDA_CSR_nRIU   (1 << 3)

/* 
 * Clock configuration register (read/write) 
 * all bits are cleared (0000'0000) 
 */
#define TDA_CCR            0x01
#define TDA_CCR_AC0        (1 << 0)
#define TDA_CCR_AC1        (1 << 1)
#define TDA_CCR_AC2        (1 << 2)
#define TDA_CCR_SC         (1 << 3)
#define TDA_CCR_CST        (1 << 4)
#define TDA_CCR_SHL        (1 << 5)

/* 
 * Programmable divider register (read/write) 
 * all bits are cleared (0000'0000)
 */
#define TDA_PDR        0x02
#define TDA_PDR_PD0    (1 << 0)
#define TDA_PDR_PD1    (1 << 1)
#define TDA_PDR_PD2    (1 << 2)
#define TDA_PDR_PD3    (1 << 3)
#define TDA_PDR_PD4    (1 << 4)
#define TDA_PDR_PD5    (1 << 5)
#define TDA_PDR_PD6    (1 << 6)
#define TDA_PDR_PD7    (1 << 7)

/* 
 * UART configuration register 2(read/write) 
 * all relevant bits are cleared after reset (x000'0000)
 */
#define TDA_UCR2             0x03
#define TDA_UCR_PSC          (1 << 0)
#define TDA_UCR_CKU          (1 << 1)
#define TDA_UCR_nAUTOCONV    (1 << 2)
#define TDA_UCR_SAN          (1 << 3)
#define TDA_UCR_PDWN         (1 << 4)
#define TDA_UCR_DISAUX       (1 << 5)
#define TDA_UCR_DISTBE_RBF   (1 << 6)

/* 
 * Guard time register (read/write) 
 * all bits are cleared (0000'0000) 
 */
#define TDA_GTR        0x05
#define TDA_GTR_GT0    (1 << 0)
#define TDA_GTR_GT1    (1 << 1)
#define TDA_GTR_GT2    (1 << 2)
#define TDA_GTR_GT3    (1 << 3)
#define TDA_GTR_GT4    (1 << 4)
#define TDA_GTR_GT5    (1 << 5)
#define TDA_GTR_GT6    (1 << 6)
#define TDA_GTR_GT7    (1 << 7)

/* 
 * UART configuration register 1(read/write) 
 * all relevant bits are cleared after reset (x000'0000)
 */
#define TDA_UCR1             0x06
#define TDA_UCR_CONV         (1 << 0)
#define TDA_UCR_SS           (1 << 1)
#define TDA_UCR_LCT          (1 << 2)
#define TDA_UCR_T_R          (1 << 3)
#define TDA_UCR_PROT         (1 << 4)
#define TDA_UCR_FC           (1 << 5)
#define TDA_UCR_FIP          (1 << 6)

/* 
 * Power control register (read/write) 
 * all relevant bits are cleared after reset (xx11'0000)
 */
#define TDA_PCR              0x07
#define TDA_PCR_START        (1 << 0)
#define TDA_PCR_3V_5V        (1 << 1)
#define TDA_PCR_RSTIN        (1 << 2)
#define TDA_PCR_1V8          (1 << 3)
#define TDA_PCR_C4           (1 << 4)
#define TDA_PCR_C8           (1 << 5)

/* 
 * Time-out configuration register (read/write) 
 * all bits are cleared (0000'0000)
 */
#define TDA_TOC            0x08
#define TDA_TOC_STOP_ALL   0x00
#define TDA_TOC_MODE1      0x61
#define TDA_TOC_MODE2      0x65
#define TDA_TOC_MODE3      0x68
#define TDA_TOC_MODE4      0x7c
#define TDA_TOC_MODE5      0xe5

/* 
 * Time-out register 1(write only) 
 * all bits are cleared (0000'0000)
 */
#define TDA_TOR1         0x09
#define TDA_TOR1_TOL0    (1 << 0)
#define TDA_TOR1_TOL1    (1 << 1)
#define TDA_TOR1_TOL2    (1 << 2)
#define TDA_TOR1_TOL3    (1 << 3)
#define TDA_TOR1_TOL4    (1 << 4)
#define TDA_TOR1_TOL5    (1 << 5)
#define TDA_TOR1_TOL6    (1 << 6)
#define TDA_TOR1_TOL7    (1 << 7)

/* 
 * Time-out register 2(write only) 
 * all bits are cleared (0000'0000)
 */
#define TDA_TOR2          0x0a
#define TDA_TOR2_TOL10    (1 << 0)
#define TDA_TOR2_TOL11    (1 << 1)
#define TDA_TOR2_TOL12    (1 << 2)
#define TDA_TOR2_TOL13    (1 << 3)
#define TDA_TOR2_TOL14    (1 << 4)
#define TDA_TOR2_TOL15    (1 << 5)
#define TDA_TOR2_TOL16    (1 << 6)
#define TDA_TOR2_TOL17    (1 << 7)

/* 
 * Time-out register 3(write only) 
 * all bits are cleared (0000'0000)
 */
#define TDA_TOR3          0x0b
#define TDA_TOR3_TOL16    (1 << 0)
#define TDA_TOR3_TOL17    (1 << 1)
#define TDA_TOR3_TOL18    (1 << 2)
#define TDA_TOR3_TOL19    (1 << 3)
#define TDA_TOR3_TOL20    (1 << 4)
#define TDA_TOR3_TOL21    (1 << 5)
#define TDA_TOR3_TOL22    (1 << 6)
#define TDA_TOR3_TOL23    (1 << 7)

/* 
 * Mixed status register (read only)
 * bits TBE, RBF and BGT are cleared, bit FE is set after reset (x10x'xxx0)
 */
#define TDA_MSR            0x0c
#define TDA_MSR_TBE_RBF    (1 << 0)
#define TDA_MSR_INTAUX     (1 << 1)
#define TDA_MSR_PR1        (1 << 2)
#define TDA_MSR_PR2        (1 << 3)
#define TDA_MSR_BGT        (1 << 5)
#define TDA_MSR_FE         (1 << 6)

/* 
 * FIFO control register (write only)
 * all relevant bits are cleared after reset (x000'x000)
 */
#define TDA_FCR        0x0c
#define TDA_FCR_FL0    (1 << 0)
#define TDA_FCR_FL1    (1 << 1)
#define TDA_FCR_FL2    (1 << 2)
#define TDA_FCR_PEC0   (1 << 4)
#define TDA_FCR_PEC1   (1 << 5)
#define TDA_FCR_PEC2   (1 << 6)

/* 
 * UART transmit register (write only)
 * all bits are cleared (0000'0000)
 */
#define TDA_UTR        0x0d
#define TDA_UTR_UT0    (1 << 0)
#define TDA_UTR_UT1    (1 << 1)
#define TDA_UTR_UT2    (1 << 2)
#define TDA_UTR_UT3    (1 << 3)
#define TDA_UTR_UT4    (1 << 4)
#define TDA_UTR_UT5    (1 << 5)
#define TDA_UTR_UT6    (1 << 6)
#define TDA_UTR_UT7    (1 << 7)

/* 
 * UART receive register (read only)
 * all bits are cleared (0000'0000)
 */
#define TDA_URR        0x0d
#define TDA_URR_UR0    (1 << 0)
#define TDA_URR_UR1    (1 << 1)
#define TDA_URR_UR2    (1 << 2)
#define TDA_URR_UR3    (1 << 3)
#define TDA_URR_UR4    (1 << 4)
#define TDA_URR_UR5    (1 << 5)
#define TDA_URR_UR6    (1 << 6)
#define TDA_URR_UR7    (1 << 7)

/* 
 * UART status register (read only)
 * all bits are cleared (0x00'0000)  
 */
#define TDA_USR            0x0e
#define TDA_USR_TBE_RBF    (1 << 0)
#define TDA_USR_FER        (1 << 1)
#define TDA_USR_OVR        (1 << 2)
#define TDA_USR_PE         (1 << 3)
#define TDA_USR_EA         (1 << 4)
#define TDA_USR_TO1        (1 << 5)
#define TDA_USR_TO3        (1 << 7)

/* 
 * Hardware status register (read only) 
 * all significant bits are cleared, except SUPL (x001'0000)
 */
#define TDA_HSR            0x0f
#define TDA_HSR_PTL        (1 << 0)
#define TDA_HSR_INTAUXL    (1 << 1)
#define TDA_HSR_PRL1       (1 << 2)
#define TDA_HSR_PRL2       (1 << 3)
#define TDA_HSR_SUPL       (1 << 4)
#define TDA_HSR_PRTL1      (1 << 5)
#define TDA_HSR_PRTL2      (1 << 6)

typedef struct tda8007_reg_entry {
        u32 addr;
        char* name;
        char* description;
        u8  mode;
        unsigned short low_ino;
} tda8007_reg_entry_t;


/*
 * Read       : 1
 * Write      : 2
 * Read/Write : 3
 */

static tda8007_reg_entry_t tda8007_regs[] =
{
  {TDA_CSR,  "TDA_CSR",  "Card select register (read/write)",            3},
  {TDA_CCR,  "TDA_CCR",  "Clock configuration register (read/write)",    3},
  {TDA_PDR,  "TDA_PDR",  "Programmable divider register (read/write)",   3},
  {TDA_UCR2, "TDA_UCR2", "UART configuration register 2(read/write)",    3},
  {TDA_GTR,  "TDA_GTR",  "Guard time register (read/write)",             3},
  {TDA_UCR1, "TDA_UCR1", "UART configuration register 1(read/write)",    3},
  {TDA_PCR,  "TDA_PCR",  "Power control register (read/write)",          3},
  {TDA_TOC,  "TDA_TOC",  "Time-out configuration register (read/write)", 3},
  {TDA_MSR,  "TDA_MSR",  "Mixed status register (read only)",       1},
  {TDA_URR,  "TDA_URR",  "UART receive register (read only)",       1},
  {TDA_USR,  "TDA_USR",  "UART status register (read only)",        1},
  {TDA_HSR,  "TDA_HSR",  "Hardware status register (read only)",    1},
  {TDA_TOR1, "TDA_TOR1", "Time-out register 1(write only)",     2},
  {TDA_TOR2, "TDA_TOR2", "Time-out register 2(write only)",     2},
  {TDA_TOR3, "TDA_TOR3", "Time-out register 3(write only)",     2},
  {TDA_FCR,  "TDA_FCR",  "FIFO control register (write only)",  2},
  {TDA_UTR,  "TDA_UTR",  "UART transmit register (write only)", 2}
};
#define NUM_OF_TDA8007_REG_ENTRY  (sizeof(tda8007_regs)/sizeof(tda8007_reg_entry_t))
/*
struct tda8007 {

};
*/
#endif /* TDA8007B_H */
