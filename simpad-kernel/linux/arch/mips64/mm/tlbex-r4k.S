/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2000 Silicon Graphics, Inc.
 * Written by Ulf Carlsson (ulfc@engr.sgi.com)
 */
#include <linux/config.h>
#include <linux/init.h>
#include <linux/threads.h>
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/pgtable.h>
#include <asm/stackframe.h>

	.data
	.comm	pgd_current, NR_CPUS * 8, 8

	/*
	 * After this macro runs we have a pointer to the pte of the address
	 * that caused the fault in in PTR.
	 */

	.macro	LOAD_PTE2, ptr, tmp
#ifdef CONFIG_SMP
	dmfc0	\ptr, CP0_CONTEXT
	dmfc0	\tmp, CP0_BADVADDR
	dsra	\ptr, 23			# get pgd_current[cpu]
#else
	dmfc0	\tmp, CP0_BADVADDR
	dla	\ptr, pgd_current
#endif
	bltz	\tmp, kaddr
	ld	\ptr, (\ptr)
	dsrl	\tmp, (PGDIR_SHIFT-3)		# get pgd offset in bytes
	andi	\tmp, ((PTRS_PER_PGD - 1)<<3)
	daddu	\ptr, \tmp			# add in pgd offset
	dmfc0	\tmp, CP0_BADVADDR
	ld	\ptr, (\ptr)			# get pmd pointer
	dsrl	\tmp, (PMD_SHIFT-3)		# get pmd offset in bytes
	andi	\tmp, ((PTRS_PER_PMD - 1)<<3)
	daddu	\ptr, \tmp			# add in pmd offset
	dmfc0	\tmp, CP0_XCONTEXT
	ld	\ptr, (\ptr)			# get pte pointer
	andi	\tmp, 0xff0			# get pte offset
	daddu	\ptr, \tmp
	.endm

	/*
	 * This places the even/odd pte pair in the page table at the pte
	 * entry pointed to by PTE into ENTRYLO0 and ENTRYLO1.
	 */
	.macro	PTE_RELOAD, pte0, pte1
	dsrl	\pte0, 6			# convert to entrylo0
	dmtc0	\pte0, CP0_ENTRYLO0		# load it
	dsrl	\pte1, 6			# convert to entrylo1
	dmtc0	\pte1, CP0_ENTRYLO1		# load it
	.endm

	.text
	.set	noreorder
	.set	mips3

	__INIT

	.align	5
FEXPORT(except_vec0)
	.set	noat
	PANIC("Unused vector called")
1:	b	1b
	 nop

	/*
	 * TLB refill handler for the R10000.
	 * Attention:  We may only use 32 instructions / 128 bytes.
	 */

	.align	5
LEAF(except_vec1_r10k)
	.set	noat
	LOAD_PTE2 k1 k0
	ld	k0, 0(k1)			# get even pte
	ld	k1, 8(k1)			# get odd pte
	PTE_RELOAD k0 k1
	nop
	tlbwr
	eret
kaddr:
	dla	k0, handle_vmalloc_address	# MAPPED kernel needs this
	jr	k0
	 nop
	END(except_vec1_r10k)

	__FINIT

	.align	5
LEAF(handle_vmalloc_address)
	.set	noat
	/*
	 * First, determine that the address is in/above vmalloc range.
	 */
	dmfc0	k0, CP0_BADVADDR
	dli	k1, VMALLOC_START

	/*
	 * Now find offset into kptbl.
	 */
	dsubu	k0, k0, k1
	dla	k1, kptbl
	dsrl	k0, (PAGE_SHIFT+1)		# get vpn2
	dsll	k0, 4				# byte offset of pte
	daddu	k1, k1, k0

	/*
	 * Determine that fault address is within vmalloc range.
	 */
	dla	k0, ekptbl
	sltu	k0, k1, k0
	beqz	k0, not_vmalloc

	/*
	 * Load cp0 registers.
	 */
	ld	k0, 0(k1)			# get even pte
	ld	k1, 8(k1)			# get odd pte

not_vmalloc:
	PTE_RELOAD k0 k1
	nop
	tlbwr
	eret
	END(handle_vmalloc_address)
