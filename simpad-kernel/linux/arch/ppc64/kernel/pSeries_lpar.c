/*
 * pSeries_lpar.c
 * Copyright (C) 2001 Todd Inglett, IBM Corporation
 *
 * pSeries LPAR support.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#include <linux/config.h>
#include <linux/kernel.h>
#include <asm/processor.h>
#include <asm/semaphore.h>
#include <asm/mmu.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/machdep.h>
#include <asm/abs_addr.h>
#include <asm/mmu_context.h>
#include <asm/ppcdebug.h>
#include <asm/pci_dma.h>
#include <linux/pci.h>
#include <asm/naca.h>

/* Status return values */
#define H_Success	0
#define H_Busy		1	/* Hardware busy -- retry later */
#define H_Hardware	-1	/* Hardware error */
#define H_Function	-2	/* Function not supported */
#define H_Privilege	-3	/* Caller not privileged */
#define H_Parameter	-4	/* Parameter invalid, out-of-range or conflicting */
#define H_Bad_Mode	-5	/* Illegal msr value */
#define H_PTEG_Full	-6	/* PTEG is full */
#define H_Not_Found	-7	/* PTE was not found" */
#define H_Reserved_DABR	-8	/* DABR address is reserved by the hypervisor on this processor" */

/* Flags */
#define H_LARGE_PAGE		(1UL<<(63-16))
#define H_EXACT		    (1UL<<(63-24))	/* Use exact PTE or return H_PTEG_FULL */
#define H_R_XLATE		(1UL<<(63-25))	/* include a valid logical page num in the pte if the valid bit is set */
#define H_READ_4		(1UL<<(63-26))	/* Return 4 PTEs */
#define H_AVPN			(1UL<<(63-32))	/* An avpn is provided as a sanity test */
#define H_ICACHE_INVALIDATE	(1UL<<(63-40))	/* icbi, etc.  (ignored for IO pages) */
#define H_ICACHE_SYNCHRONIZE	(1UL<<(63-41))	/* dcbst, icbi, etc (ignored for IO pages */
#define H_ZERO_PAGE		(1UL<<(63-48))	/* zero the page before mapping (ignored for IO pages) */
#define H_COPY_PAGE		(1UL<<(63-49))
#define H_N			(1UL<<(63-61))
#define H_PP1			(1UL<<(63-62))
#define H_PP2			(1UL<<(63-63))



/* pSeries hypervisor opcodes */
#define H_REMOVE		0x04
#define H_ENTER			0x08
#define H_READ			0x0c
#define H_CLEAR_MOD		0x10
#define H_CLEAR_REF		0x14
#define H_PROTECT		0x18
#define H_GET_TCE		0x1c
#define H_PUT_TCE		0x20
#define H_SET_SPRG0		0x24
#define H_SET_DABR		0x28
#define H_PAGE_INIT		0x2c
#define H_SET_ASR		0x30
#define H_ASR_ON		0x34
#define H_ASR_OFF		0x38
#define H_LOGICAL_CI_LOAD	0x3c
#define H_LOGICAL_CI_STORE	0x40
#define H_LOGICAL_CACHE_LOAD	0x44
#define H_LOGICAL_CACHE_STORE	0x48
#define H_LOGICAL_ICBI		0x4c
#define H_LOGICAL_DCBF		0x50
#define H_GET_TERM_CHAR		0x54
#define H_PUT_TERM_CHAR		0x58
#define H_REAL_TO_LOGICAL	0x5c
#define H_HYPERVISOR_DATA	0x60
#define H_EOI			0x64
#define H_CPPR			0x68
#define H_IPI			0x6c
#define H_IPOLL			0x70
#define H_XIRR			0x74

#define HSC			".long 0x44000022\n"
#define H_ENTER_r3		"li	3, 0x08\n"

/* plpar_hcall() -- Generic call interface using above opcodes
 *
 * The actual call interface is a hypervisor call instruction with
 * the opcode in R3 and input args in R4-R7.
 * Status is returned in R3 with variable output values in R4-R11.
 * Only H_PTE_READ with H_READ_4 uses R6-R11 so we ignore it for now
 * and return only two out args which MUST ALWAYS BE PROVIDED.
 */
long plpar_hcall(unsigned long opcode,
		 unsigned long arg1,
		 unsigned long arg2,
		 unsigned long arg3,
		 unsigned long arg4,
		 unsigned long *out1,
		 unsigned long *out2,
		 unsigned long *out3);

/* Same as plpar_hcall but for those opcodes that return no values
 * other than status.  Slightly more efficient.
 */
long plpar_hcall_norets(unsigned long opcode, ...);


long plpar_pte_enter(unsigned long flags,
		     unsigned long ptex,
		     unsigned long new_pteh, unsigned long new_ptel,
		     unsigned long *old_pteh_ret, unsigned long *old_ptel_ret)
{
	unsigned long dummy, ret;
	ret = plpar_hcall(H_ENTER, flags, ptex, new_pteh, new_ptel,
			   old_pteh_ret, old_ptel_ret, &dummy);
	return(ret);
}

long plpar_pte_remove(unsigned long flags,
		      unsigned long ptex,
		      unsigned long avpn,
		      unsigned long *old_pteh_ret, unsigned long *old_ptel_ret)
{
	unsigned long dummy;
	return plpar_hcall(H_REMOVE, flags, ptex, avpn, 0,
			   old_pteh_ret, old_ptel_ret, &dummy);
}

long plpar_pte_read(unsigned long flags,
		    unsigned long ptex,
		    unsigned long *old_pteh_ret, unsigned long *old_ptel_ret)
{
	unsigned long dummy;
	return plpar_hcall(H_READ, flags, ptex, 0, 0,
			   old_pteh_ret, old_ptel_ret, &dummy);
}

long plpar_pte_protect(unsigned long flags,
		       unsigned long ptex,
		       unsigned long avpn)
{
	return plpar_hcall_norets(H_PROTECT, flags, ptex);
}

long plpar_tce_get(unsigned long liobn,
		   unsigned long ioba,
		   unsigned long *tce_ret)
{
	unsigned long dummy;
	return plpar_hcall(H_GET_TCE, liobn, ioba, 0, 0,
			   tce_ret, &dummy, &dummy);
}


long plpar_tce_put(unsigned long liobn,
		   unsigned long ioba,
		   unsigned long tceval)
{
	return plpar_hcall_norets(H_PUT_TCE, liobn, ioba, tceval);
}

long plpar_get_term_char(unsigned long termno,
			 unsigned long *len_ret,
			 char *buf_ret)
{
	unsigned long *lbuf = (unsigned long *)buf_ret;  /* ToDo: alignment? */
	return plpar_hcall(H_GET_TERM_CHAR, termno, 0, 0, 0,
			   len_ret, lbuf+0, lbuf+1);
}

long plpar_put_term_char(unsigned long termno,
			 unsigned long len,
			 const char *buffer)
{
	unsigned long dummy;
	unsigned long *lbuf = (unsigned long *)buffer;  /* ToDo: alignment? */
	return plpar_hcall(H_PUT_TERM_CHAR, termno, len,
			   lbuf[0], lbuf[1], &dummy, &dummy, &dummy);
}

long plpar_eoi(unsigned long xirr)
{
	return plpar_hcall_norets(H_EOI, xirr);
}

long plpar_cppr(unsigned long cppr)
{
	return plpar_hcall_norets(H_CPPR, cppr);
}

long plpar_ipi(unsigned long servernum,
	       unsigned long mfrr)
{
	return plpar_hcall_norets(H_IPI, servernum, mfrr);
}

long plpar_xirr(unsigned long *xirr_ret)
{
	unsigned long dummy;
	return plpar_hcall(H_XIRR, 0, 0, 0, 0,
			   xirr_ret, &dummy, &dummy);
}

long plpar_ipoll(unsigned long servernum, unsigned long* xirr_ret, unsigned long* mfrr_ret)
{
	unsigned long dummy;
	return plpar_hcall(H_IPOLL, servernum, 0, 0, 0,
			   xirr_ret, mfrr_ret, &dummy);
}

/*
 * The following section contains code that ultimately should
 * be put in the relavent file (htab.c, xics.c, etc).  It has
 * been put here for the time being in order to ease maintainence
 * of the pSeries LPAR code until it can all be put into CVS.
 */
static void hpte_invalidate_pSeriesLP(unsigned long slot)
{
	HPTE old_pte;
	unsigned long lpar_rc;
	unsigned long flags = 0;
			
	lpar_rc = plpar_pte_remove(flags,
				   slot,
				   0,
				   &old_pte.dw0.dword0, 
				   &old_pte.dw1.dword1);
	if (lpar_rc != H_Success) BUG();
}

/* NOTE: for updatepp ops we are fortunate that the linux "newpp" bits and
 * the low 3 bits of flags happen to line up.  So no transform is needed.
 * We can probably optimize here and assume the high bits of newpp are
 * already zero.  For now I am paranoid.
 */
static void hpte_updatepp_pSeriesLP(long slot, unsigned long newpp, unsigned long va)
{
	unsigned long lpar_rc;
	unsigned long flags;
	flags =   newpp & 3;
	lpar_rc = plpar_pte_protect( flags,
				     slot,
				     0);
	if (lpar_rc != H_Success) {
		udbg_printf( " bad return code from pte protect rc = %lx \n", lpar_rc); 
		for (;;);
	}
}

static void hpte_updateboltedpp_pSeriesLP(unsigned long newpp, unsigned long ea)
{
	unsigned long lpar_rc;
	unsigned long vsid,va,vpn,flags;
	long slot;

	vsid = get_kernel_vsid( ea );
	va = ( vsid << 28 ) | ( ea & 0x0fffffff );
	vpn = va >> PAGE_SHIFT;

	slot = ppc_md.hpte_find( vpn );
	flags =   newpp & 3;
	lpar_rc = plpar_pte_protect( flags,
				     slot,
				     0);
	if (lpar_rc != H_Success) {
		udbg_printf( " bad return code from pte bolted protect rc = %lx \n", lpar_rc); 
		for (;;);
	}
}


static unsigned long hpte_getword0_pSeriesLP(unsigned long slot)
{
	unsigned long dword0;
	unsigned long lpar_rc;
	unsigned long dummy_word1;
	unsigned long flags;
	/* Read 1 pte at a time                        */
	/* Do not need RPN to logical page translation */
	/* No cross CEC PFT access                     */
	flags = 0;
	
	lpar_rc = plpar_pte_read(flags,
				 slot,
				 &dword0, &dummy_word1);
	if (lpar_rc != H_Success) {
		udbg_printf(" error on pte read in get_hpte0 rc = %lx \n", lpar_rc);
		for (;;);
	}

	return(dword0);
}

static long hpte_selectslot_pSeriesLP(unsigned long vpn)
{
	unsigned long primary_hash;
	unsigned long hpteg_slot;
	unsigned i, k;
	unsigned long flags;
	HPTE  pte_read;
	unsigned long lpar_rc;

	/* Search the primary group for an available slot */
	primary_hash = hpt_hash(vpn, 0);

	hpteg_slot = ( primary_hash & htab_data.htab_hash_mask ) * HPTES_PER_GROUP;

	/* Read 1 pte at a time                        */
	/* Do not need RPN to logical page translation */
	/* No cross CEC PFT access                     */
	flags = 0;
	for (i=0; i<HPTES_PER_GROUP; ++i) {
		/* read the hpte entry from the slot */
		lpar_rc = plpar_pte_read(flags,
					 hpteg_slot + i,
					 &pte_read.dw0.dword0, &pte_read.dw1.dword1);
		if (lpar_rc != H_Success) {
			udbg_printf(" read of hardware page table failed rc = %lx \n", lpar_rc); 
			for (;;);
		}
		if ( pte_read.dw0.dw0.v == 0 ) {
			/* If an available slot found, return it */
			return hpteg_slot + i;
		}

	}


	/* Search the secondary group for an available slot */
	hpteg_slot = ( ~primary_hash & htab_data.htab_hash_mask ) * HPTES_PER_GROUP;


	for (i=0; i<HPTES_PER_GROUP; ++i) {
		/* read the hpte entry from the slot */
		lpar_rc = plpar_pte_read(flags,
					 hpteg_slot + i,
					 &pte_read.dw0.dword0, &pte_read.dw1.dword1);
		if (lpar_rc != H_Success) {
			udbg_printf(" read of hardware page table failed2 rc = %lx  \n", lpar_rc); 
			for (;;);
		}
		if ( pte_read.dw0.dw0.v == 0 ) {
			/* If an available slot found, return it */
			return hpteg_slot + i;
		}

	}

	/* No available entry found in secondary group */


	/* Select an entry in the primary group to replace */

	hpteg_slot = ( primary_hash & htab_data.htab_hash_mask ) * HPTES_PER_GROUP;

	k = htab_data.next_round_robin++ & 0x7;

	for (i=0; i<HPTES_PER_GROUP; ++i) {
		if (k == HPTES_PER_GROUP)
			k = 0;

		lpar_rc = plpar_pte_read(flags,
					 hpteg_slot + k,
					 &pte_read.dw0.dword0, &pte_read.dw1.dword1);
		if (lpar_rc != H_Success) {
			udbg_printf( " pte read failed - rc = %lx", lpar_rc); 
			for (;;);
		}
		if (  ! pte_read.dw0.dw0.bolted)
		{
			hpteg_slot += k;
			/* Invalidate the current entry */
			ppc_md.hpte_invalidate(hpteg_slot); 
			return hpteg_slot;
		}
		++k;
	}

	/* No non-bolted entry found in primary group - time to panic */
	udbg_printf("select_hpte_slot - No non-bolted HPTE in group 0x%lx! \n", hpteg_slot/HPTES_PER_GROUP);
	udbg_printf("No non-bolted HPTE in group %lx", (unsigned long)hpteg_slot/HPTES_PER_GROUP);
	for (;;);

	/* never executes - avoid compiler errors */
	return 0;
}


static void hpte_create_valid_pSeriesLP(unsigned long slot, unsigned long vpn,
					unsigned long prpn, unsigned hash, 
					void *ptep, unsigned hpteflags, 
					unsigned bolted)
{
	/* Local copy of HPTE */
	struct {
		/* Local copy of first doubleword of HPTE */
		union {
			unsigned long d;
			Hpte_dword0   h;
		} dw0;
		/* Local copy of second doubleword of HPTE */
		union {
			unsigned long     d;
			Hpte_dword1       h;
			Hpte_dword1_flags f;
		} dw1;
	} lhpte;
	
	unsigned long avpn = vpn >> 11;
	unsigned long arpn = physRpn_to_absRpn( prpn );

	unsigned long lpar_rc;
	unsigned long flags;
	HPTE ret_hpte;

	/* Fill in the local HPTE with absolute rpn, avpn and flags */
	lhpte.dw1.d        = 0;
	lhpte.dw1.h.rpn    = arpn;
	lhpte.dw1.f.flags  = hpteflags;

	lhpte.dw0.d        = 0;
	lhpte.dw0.h.avpn   = avpn;
	lhpte.dw0.h.h      = hash;
	lhpte.dw0.h.bolted = bolted;
	lhpte.dw0.h.v      = 1;

	/* Now fill in the actual HPTE */
	/* Set CEC cookie to 0                  */
	/* Large page = 0                       */
	/* Zero page = 0                        */
	/* I-cache Invalidate = 0               */
	/* I-cache synchronize = 0              */
	/* Exact = 1 - only modify exact entry  */
	flags = H_EXACT;

	if (hpteflags & (_PAGE_GUARDED|_PAGE_NO_CACHE))
		lhpte.dw1.f.flags &= ~_PAGE_COHERENT;
#if 1
	__asm__ __volatile__ (
		 H_ENTER_r3
		 "mr	4, %1\n"
		 "mr	5, %2\n"
		 "mr	6, %3\n"
		 "mr	7, %4\n"
		 HSC
		 "mr	%0, 3\n"
		 : "=r" (lpar_rc)
		 : "r" (flags), "r" (slot), "r" (lhpte.dw0.d), "r" (lhpte.dw1.d)
		 : "r3", "r4", "r5", "r6", "r7", "cc");
#else
	lpar_rc =  plpar_pte_enter(flags,
				   slot,
				   lhpte.dw0.d,
				   lhpte.dw1.d,
				   &ret_hpte.dw0.dword0,
				   &ret_hpte.dw1.dword1);
#endif
	if (lpar_rc != H_Success) {
		udbg_printf("error on pte enter lapar rc = %ld\n",lpar_rc);
		udbg_printf("ent: s=%lx, dw0=%lx, dw1=%lx\n", slot, lhpte.dw0.d, lhpte.dw1.d);
		/* xmon_backtrace("backtrace"); */
		for (;;);
	}
}

static long hpte_find_pSeriesLP(unsigned long vpn)
{
	union {
		unsigned long d;
		Hpte_dword0   h;
	} hpte_dw0;
	long slot;
	unsigned long hash;
	unsigned long i,j;

	hash = hpt_hash(vpn, 0);
	for ( j=0; j<2; ++j ) {
		slot = (hash & htab_data.htab_hash_mask) * HPTES_PER_GROUP;
		for ( i=0; i<HPTES_PER_GROUP; ++i ) {
			hpte_dw0.d = hpte_getword0_pSeriesLP( slot );
			if ( ( hpte_dw0.h.avpn == ( vpn >> 11 ) ) &&
			     ( hpte_dw0.h.v ) &&
			     ( hpte_dw0.h.h == j ) ) {
				/* HPTE matches */
				if ( j )
					slot = -slot;
				return slot;
			}
			++slot;
		}
		hash = ~hash;
	}
	return -1;
} 

/*
 * Create a pte - LPAR .  Used during initialization only.
 * We assume the PTE will fit in the primary PTEG.
 */
void make_pte_LPAR(HPTE *htab,
		   unsigned long va, unsigned long pa, int mode,
		   unsigned long hash_mask, int large)
{
	HPTE  local_hpte, ret_hpte;
	unsigned long hash, slot, flags,lpar_rc, vpn;

	if (large)
		vpn = va >> 24;
	else
		vpn = va >> 12;

	hash = hpt_hash(vpn, large);

	slot = ((hash & hash_mask)*HPTES_PER_GROUP);

	local_hpte.dw1.dword1 = pa | mode;
	local_hpte.dw0.dword0 = 0;
	local_hpte.dw0.dw0.avpn = va >> 23;
	local_hpte.dw0.dw0.bolted = 1;				/* bolted */
	if (large)
		local_hpte.dw0.dw0.l = 1;  /* large page */
	local_hpte.dw0.dw0.v = 1;

	/* Set CEC cookie to 0                   */
	/* Zero page = 0                         */
	/* I-cache Invalidate = 0                */
	/* I-cache synchronize = 0               */
	/* Exact = 0 - modify any entry in group */
	flags = 0;
#if 1
	__asm__ __volatile__ (
		 H_ENTER_r3
		 "mr	4, %1\n"
		 "mr	5, %2\n"
		 "mr	6, %3\n"
		 "mr	7, %4\n"
		 HSC
		 "mr	%0, 3\n"
		 : "=r" (lpar_rc)
		 : "r" (flags), "r" (slot), "r" (local_hpte.dw0.dword0), "r" (local_hpte.dw1.dword1)
		 : "r3", "r4", "r5", "r6", "r7", "cc");
#else
	lpar_rc =  plpar_pte_enter(flags,
				   slot,
				   local_hpte.dw0.dword0,
				   local_hpte.dw1.dword1,
				   &ret_hpte.dw0.dword0,
				   &ret_hpte.dw1.dword1);
#endif
#if 0 /* NOTE: we explicitly do not check return status here because it is
       * "normal" for early boot code to map io regions for which a partition
       * has no access.  However, we will die if we actually fault on these
       * "permission denied" pages.
       */
	if (lpar_rc != H_Success) {
		/* pSeriesLP_init_early(); */
		udbg_printf("flags=%lx, slot=%lx, dword0=%lx, dword1=%lx, rc=%d\n", flags, slot, local_hpte.dw0.dword0,local_hpte.dw1.dword1, lpar_rc);
		BUG();
	}
#endif
}

static void tce_build_pSeriesLP(struct TceTable *tbl, long tcenum, 
				unsigned long uaddr, int direction )
{
	u64 set_tce_rc;
	union Tce tce;
	
	PPCDBG(PPCDBG_TCE, "build_tce: uaddr = 0x%lx\n", uaddr);
	PPCDBG(PPCDBG_TCE, "\ttcenum = 0x%lx, tbl = 0x%lx, index=%lx\n", 
	       tcenum, tbl, tbl->index);

	tce.wholeTce = 0;
	tce.tceBits.rpn = (virt_to_absolute(uaddr)) >> PAGE_SHIFT;

	tce.tceBits.readWrite = 1;
	if ( direction != PCI_DMA_TODEVICE ) tce.tceBits.pciWrite = 1;

	set_tce_rc = plpar_tce_put((u64)tbl->index, 
				 (u64)tcenum << 12, 
				 tce.wholeTce );

	if(set_tce_rc) {
		printk("tce_build_pSeriesLP: plpar_tce_put failed. rc=%ld\n", set_tce_rc);
		printk("\tindex   = 0x%lx\n", (u64)tbl->index);
		printk("\ttcenum  = 0x%lx\n", (u64)tcenum);
		printk("\ttce val = 0x%lx\n", tce.wholeTce );
	}
}

static void tce_free_one_pSeriesLP(struct TceTable *tbl, long tcenum)
{
	u64 set_tce_rc;
	union Tce tce;

	tce.wholeTce = 0;
	set_tce_rc = plpar_tce_put((u64)tbl->index, 
				 (u64)tcenum << 12,
				 tce.wholeTce );
	if ( set_tce_rc ) {
		printk("tce_free_one_pSeriesLP: plpar_tce_put failed\n");
		printk("\trc      = %ld\n", set_tce_rc);
		printk("\tindex   = 0x%lx\n", (u64)tbl->index);
		printk("\ttcenum  = 0x%lx\n", (u64)tcenum);
		printk("\ttce val = 0x%lx\n", tce.wholeTce );
	}

}

/* PowerPC Interrupts for lpar. */
/* NOTE: this typedef is duplicated (for now) from xics.c! */
typedef struct {
	int (*xirr_info_get)(int cpu);
	void (*xirr_info_set)(int cpu, int val);
	void (*cppr_info)(int cpu, u8 val);
	void (*qirr_info)(int cpu, u8 val);
} xics_ops;
static int pSeriesLP_xirr_info_get(int n_cpu)
{
	unsigned long lpar_rc;
	unsigned long return_value; 

	lpar_rc = plpar_xirr(&return_value);
	if (lpar_rc != H_Success) {
		panic(" bad return code xirr - rc = %lx \n", lpar_rc); 
	}
	return ((int)(return_value));
}

static void pSeriesLP_xirr_info_set(int n_cpu, int value)
{
	unsigned long lpar_rc;
	unsigned long val64 = value & 0xffffffff;

	lpar_rc = plpar_eoi(val64);
	if (lpar_rc != H_Success) {
		panic(" bad return code EOI - rc = %ld, value=%lx \n", lpar_rc, val64); 
	}
}

static void pSeriesLP_cppr_info(int n_cpu, u8 value)
{
	unsigned long lpar_rc;

	lpar_rc = plpar_cppr(value);
	if (lpar_rc != H_Success) {
		panic(" bad return code cppr - rc = %lx \n", lpar_rc); 
	}
}

static void pSeriesLP_qirr_info(int n_cpu , u8 value)
{
	unsigned long lpar_rc;

	lpar_rc = plpar_ipi(get_hard_smp_processor_id(n_cpu),value);
	if (lpar_rc != H_Success) {
    udbg_printf("pSeriesLP_qirr_info - plpar_ipi failed!!!!!!!! \n");
		panic(" bad return code qirr -ipi  - rc = %lx \n", lpar_rc); 
	}
}

xics_ops pSeriesLP_ops = {
	pSeriesLP_xirr_info_get,
	pSeriesLP_xirr_info_set,
	pSeriesLP_cppr_info,
	pSeriesLP_qirr_info
};
/* end TAI-LPAR */


int vtermno;	/* virtual terminal# for udbg  */

static void udbg_putcLP(unsigned char c)
{
	char buf[16];
	unsigned long rc;

	if (c == '\n')
		udbg_putcLP('\r');

	buf[0] = c;
	do {
		rc = plpar_put_term_char(vtermno, 1, buf);
	} while(rc == H_Busy);
}

/* Buffered chars getc */
static long inbuflen;
static long inbuf[2];	/* must be 2 longs */

static int udbg_getc_pollLP(void)
{
	/* The interface is tricky because it may return up to 16 chars.
	 * We save them statically for future calls to udbg_getc().
	 */
	char ch, *buf = (char *)inbuf;
	int i;
	long rc;
	if (inbuflen == 0) {
		/* get some more chars. */
		inbuflen = 0;
		rc = plpar_get_term_char(vtermno, &inbuflen, buf);
		if (rc != H_Success)
			inbuflen = 0;	/* otherwise inbuflen is garbage */
	}
	if (inbuflen <= 0 || inbuflen > 16) {
		/* Catch error case as well as other oddities (corruption) */
		inbuflen = 0;
		return -1;
	}
	ch = buf[0];
	for (i = 1; i < inbuflen; i++)	/* shuffle them down. */
		buf[i-1] = buf[i];
	inbuflen--;
	return ch;
}

static unsigned char udbg_getcLP(void)
{
	int ch;
	for (;;) {
		ch = udbg_getc_pollLP();
		if (ch == -1) {
			/* This shouldn't be needed...but... */
			volatile unsigned long delay;
			for (delay=0; delay < 2000000; delay++)
				;
		} else {
			return ch;
		}
	}
}


/* This is called early in setup.c.
 * Use it to setup page table ppc_md stuff as well as udbg.
 */
void pSeriesLP_init_early(void)
{
	ppc_md.hpte_invalidate   = hpte_invalidate_pSeriesLP;
	ppc_md.hpte_updatepp     = hpte_updatepp_pSeriesLP;
	ppc_md.hpte_updateboltedpp  = hpte_updateboltedpp_pSeriesLP;
	ppc_md.hpte_getword0     = hpte_getword0_pSeriesLP;
	ppc_md.hpte_selectslot   = hpte_selectslot_pSeriesLP;
	ppc_md.hpte_create_valid = hpte_create_valid_pSeriesLP;
	ppc_md.hpte_find	 = hpte_find_pSeriesLP;

	ppc_md.tce_build	 = tce_build_pSeriesLP;
	ppc_md.tce_free_one	 = tce_free_one_pSeriesLP;

#ifdef CONFIG_SMP
	smp_init_pSeries();
#endif
	pSeries_pcibios_init_early();

	/* The keyboard is not useful in the LPAR environment.
	 * Leave all the interfaces NULL.
	 */

	if (naca->serialPortAddr) {
		void *comport = (void *)__ioremap(naca->serialPortAddr, 16, _PAGE_NO_CACHE);
		udbg_init_uart(comport);
		ppc_md.udbg_putc = udbg_putc;
		ppc_md.udbg_getc = udbg_getc;
		ppc_md.udbg_getc_poll = udbg_getc_poll;
	} else {
		/* lookup the first virtual terminal number in case we don't have a com port.
		 * Zero is probably correct in case someone calls udbg before the init.
		 * The property is a pair of numbers.  The first is the starting termno (the
		 * one we use) and the second is the number of terminals.
		 */
		u32 *termno;
		struct device_node *np = find_path_device("/rtas");
		if (np) {
			termno = (u32 *)get_property(np, "ibm,termno", 0);
			if (termno)
				vtermno = termno[0];
		}
		ppc_md.udbg_putc = udbg_putcLP;
		ppc_md.udbg_getc = udbg_getcLP;
		ppc_md.udbg_getc_poll = udbg_getc_pollLP;
	}
}

/* Code for hvc_console.  Should move it back eventually. */

int hvc_get_chars(int index, char *buf, int count)
{
	unsigned long got;

	if (plpar_hcall(H_GET_TERM_CHAR, index, 0, 0, 0, &got,
		(unsigned long *)buf, (unsigned long *)buf+1) == H_Success) {
		/*
		 * Work around a HV bug where it gives us a null
		 * after every \r.  -- paulus
		 */
		if (got > 0) {
			int i;
			for (i = 1; i < got; ++i) {
				if (buf[i] == 0 && buf[i-1] == '\r') {
					--got;
					if (i < got)
						memmove(&buf[i], &buf[i+1],
							got - i);
				}
			}
		}
		return got;
	}
	return 0;
}

int hvc_put_chars(int index, const char *buf, int count)
{
	unsigned long dummy;
	unsigned long *lbuf = (unsigned long *) buf;
	long ret;

	ret = plpar_hcall(H_PUT_TERM_CHAR, index, count, lbuf[0], lbuf[1],
			  &dummy, &dummy, &dummy);
	if (ret == H_Success)
		return count;
	if (ret == H_Busy)
		return 0;
	return -1;
}

int hvc_count(int *start_termno)
{
	u32 *termno;
	struct device_node *dn;

	if ((dn = find_path_device("/rtas")) != NULL) {
		if ((termno = (u32 *)get_property(dn, "ibm,termno", 0)) != NULL) {
			if (start_termno)
				*start_termno = termno[0];
			return termno[1];
		}
	}
	return 0;
}
