#ifndef _ASM_IA64_OFFSETS_H
#define _ASM_IA64_OFFSETS_H
/*
 * DO NOT MODIFY
 *
 * This file was generated by arch/ia64/tools/print_offsets.awk.
 *
 */
#define PT_PTRACED_BIT		0
#define PT_TRACESYS_BIT		1
#define IA64_TASK_SIZE			3936	/* 0xf60 */
#define IA64_PT_REGS_SIZE		400	/* 0x190 */
#define IA64_SWITCH_STACK_SIZE		560	/* 0x230 */
#define IA64_SIGINFO_SIZE		128	/* 0x80 */
#define IA64_CPU_SIZE			16384	/* 0x4000 */
#define SIGFRAME_SIZE			2816	/* 0xb00 */
#define UNW_FRAME_INFO_SIZE		448	/* 0x1c0 */

#define IA64_TASK_PTRACE_OFFSET		48	/* 0x30 */
#define IA64_TASK_SIGPENDING_OFFSET	16	/* 0x10 */
#define IA64_TASK_NEED_RESCHED_OFFSET	40	/* 0x28 */
#define IA64_TASK_PROCESSOR_OFFSET	96	/* 0x60 */
#define IA64_TASK_THREAD_OFFSET		1488	/* 0x5d0 */
#define IA64_TASK_THREAD_KSP_OFFSET	1488	/* 0x5d0 */
#define IA64_TASK_PFM_OVFL_BLOCK_RESET_OFFSET 2112	/* 0x840 */
#define IA64_TASK_PID_OFFSET		228	/* 0xe4 */
#define IA64_TASK_MM_OFFSET		88	/* 0x58 */
#define IA64_PT_REGS_CR_IPSR_OFFSET	0	/* 0x0 */
#define IA64_PT_REGS_CR_IIP_OFFSET	8	/* 0x8 */
#define IA64_PT_REGS_CR_IFS_OFFSET	16	/* 0x10 */
#define IA64_PT_REGS_AR_UNAT_OFFSET	24	/* 0x18 */
#define IA64_PT_REGS_AR_PFS_OFFSET	32	/* 0x20 */
#define IA64_PT_REGS_AR_RSC_OFFSET	40	/* 0x28 */
#define IA64_PT_REGS_AR_RNAT_OFFSET	48	/* 0x30 */
#define IA64_PT_REGS_AR_BSPSTORE_OFFSET	56	/* 0x38 */
#define IA64_PT_REGS_PR_OFFSET		64	/* 0x40 */
#define IA64_PT_REGS_B6_OFFSET		72	/* 0x48 */
#define IA64_PT_REGS_LOADRS_OFFSET	80	/* 0x50 */
#define IA64_PT_REGS_R1_OFFSET		88	/* 0x58 */
#define IA64_PT_REGS_R2_OFFSET		96	/* 0x60 */
#define IA64_PT_REGS_R3_OFFSET		104	/* 0x68 */
#define IA64_PT_REGS_R12_OFFSET		112	/* 0x70 */
#define IA64_PT_REGS_R13_OFFSET		120	/* 0x78 */
#define IA64_PT_REGS_R14_OFFSET		128	/* 0x80 */
#define IA64_PT_REGS_R15_OFFSET		136	/* 0x88 */
#define IA64_PT_REGS_R8_OFFSET		144	/* 0x90 */
#define IA64_PT_REGS_R9_OFFSET		152	/* 0x98 */
#define IA64_PT_REGS_R10_OFFSET		160	/* 0xa0 */
#define IA64_PT_REGS_R11_OFFSET		168	/* 0xa8 */
#define IA64_PT_REGS_R16_OFFSET		176	/* 0xb0 */
#define IA64_PT_REGS_R17_OFFSET		184	/* 0xb8 */
#define IA64_PT_REGS_R18_OFFSET		192	/* 0xc0 */
#define IA64_PT_REGS_R19_OFFSET		200	/* 0xc8 */
#define IA64_PT_REGS_R20_OFFSET		208	/* 0xd0 */
#define IA64_PT_REGS_R21_OFFSET		216	/* 0xd8 */
#define IA64_PT_REGS_R22_OFFSET		224	/* 0xe0 */
#define IA64_PT_REGS_R23_OFFSET		232	/* 0xe8 */
#define IA64_PT_REGS_R24_OFFSET		240	/* 0xf0 */
#define IA64_PT_REGS_R25_OFFSET		248	/* 0xf8 */
#define IA64_PT_REGS_R26_OFFSET		256	/* 0x100 */
#define IA64_PT_REGS_R27_OFFSET		264	/* 0x108 */
#define IA64_PT_REGS_R28_OFFSET		272	/* 0x110 */
#define IA64_PT_REGS_R29_OFFSET		280	/* 0x118 */
#define IA64_PT_REGS_R30_OFFSET		288	/* 0x120 */
#define IA64_PT_REGS_R31_OFFSET		296	/* 0x128 */
#define IA64_PT_REGS_AR_CCV_OFFSET	304	/* 0x130 */
#define IA64_PT_REGS_AR_FPSR_OFFSET	312	/* 0x138 */
#define IA64_PT_REGS_B0_OFFSET		320	/* 0x140 */
#define IA64_PT_REGS_B7_OFFSET		328	/* 0x148 */
#define IA64_PT_REGS_F6_OFFSET		336	/* 0x150 */
#define IA64_PT_REGS_F7_OFFSET		352	/* 0x160 */
#define IA64_PT_REGS_F8_OFFSET		368	/* 0x170 */
#define IA64_PT_REGS_F9_OFFSET		384	/* 0x180 */
#define IA64_SWITCH_STACK_CALLER_UNAT_OFFSET 0	/* 0x0 */
#define IA64_SWITCH_STACK_AR_FPSR_OFFSET 8	/* 0x8 */
#define IA64_SWITCH_STACK_F2_OFFSET	16	/* 0x10 */
#define IA64_SWITCH_STACK_F3_OFFSET	32	/* 0x20 */
#define IA64_SWITCH_STACK_F4_OFFSET	48	/* 0x30 */
#define IA64_SWITCH_STACK_F5_OFFSET	64	/* 0x40 */
#define IA64_SWITCH_STACK_F10_OFFSET	80	/* 0x50 */
#define IA64_SWITCH_STACK_F11_OFFSET	96	/* 0x60 */
#define IA64_SWITCH_STACK_F12_OFFSET	112	/* 0x70 */
#define IA64_SWITCH_STACK_F13_OFFSET	128	/* 0x80 */
#define IA64_SWITCH_STACK_F14_OFFSET	144	/* 0x90 */
#define IA64_SWITCH_STACK_F15_OFFSET	160	/* 0xa0 */
#define IA64_SWITCH_STACK_F16_OFFSET	176	/* 0xb0 */
#define IA64_SWITCH_STACK_F17_OFFSET	192	/* 0xc0 */
#define IA64_SWITCH_STACK_F18_OFFSET	208	/* 0xd0 */
#define IA64_SWITCH_STACK_F19_OFFSET	224	/* 0xe0 */
#define IA64_SWITCH_STACK_F20_OFFSET	240	/* 0xf0 */
#define IA64_SWITCH_STACK_F21_OFFSET	256	/* 0x100 */
#define IA64_SWITCH_STACK_F22_OFFSET	272	/* 0x110 */
#define IA64_SWITCH_STACK_F23_OFFSET	288	/* 0x120 */
#define IA64_SWITCH_STACK_F24_OFFSET	304	/* 0x130 */
#define IA64_SWITCH_STACK_F25_OFFSET	320	/* 0x140 */
#define IA64_SWITCH_STACK_F26_OFFSET	336	/* 0x150 */
#define IA64_SWITCH_STACK_F27_OFFSET	352	/* 0x160 */
#define IA64_SWITCH_STACK_F28_OFFSET	368	/* 0x170 */
#define IA64_SWITCH_STACK_F29_OFFSET	384	/* 0x180 */
#define IA64_SWITCH_STACK_F30_OFFSET	400	/* 0x190 */
#define IA64_SWITCH_STACK_F31_OFFSET	416	/* 0x1a0 */
#define IA64_SWITCH_STACK_R4_OFFSET	432	/* 0x1b0 */
#define IA64_SWITCH_STACK_R5_OFFSET	440	/* 0x1b8 */
#define IA64_SWITCH_STACK_R6_OFFSET	448	/* 0x1c0 */
#define IA64_SWITCH_STACK_R7_OFFSET	456	/* 0x1c8 */
#define IA64_SWITCH_STACK_B0_OFFSET	464	/* 0x1d0 */
#define IA64_SWITCH_STACK_B1_OFFSET	472	/* 0x1d8 */
#define IA64_SWITCH_STACK_B2_OFFSET	480	/* 0x1e0 */
#define IA64_SWITCH_STACK_B3_OFFSET	488	/* 0x1e8 */
#define IA64_SWITCH_STACK_B4_OFFSET	496	/* 0x1f0 */
#define IA64_SWITCH_STACK_B5_OFFSET	504	/* 0x1f8 */
#define IA64_SWITCH_STACK_AR_PFS_OFFSET	512	/* 0x200 */
#define IA64_SWITCH_STACK_AR_LC_OFFSET	520	/* 0x208 */
#define IA64_SWITCH_STACK_AR_UNAT_OFFSET 528	/* 0x210 */
#define IA64_SWITCH_STACK_AR_RNAT_OFFSET 536	/* 0x218 */
#define IA64_SWITCH_STACK_AR_BSPSTORE_OFFSET 544	/* 0x220 */
#define IA64_SWITCH_STACK_PR_OFFSET	552	/* 0x228 */
#define IA64_SIGCONTEXT_AR_BSP_OFFSET	72	/* 0x48 */
#define IA64_SIGCONTEXT_AR_FPSR_OFFSET	104	/* 0x68 */
#define IA64_SIGCONTEXT_AR_RNAT_OFFSET	80	/* 0x50 */
#define IA64_SIGCONTEXT_AR_UNAT_OFFSET	96	/* 0x60 */
#define IA64_SIGCONTEXT_B0_OFFSET	136	/* 0x88 */
#define IA64_SIGCONTEXT_CFM_OFFSET	48	/* 0x30 */
#define IA64_SIGCONTEXT_FLAGS_OFFSET	0	/* 0x0 */
#define IA64_SIGCONTEXT_FR6_OFFSET	560	/* 0x230 */
#define IA64_SIGCONTEXT_PR_OFFSET	128	/* 0x80 */
#define IA64_SIGCONTEXT_R12_OFFSET	296	/* 0x128 */
#define IA64_SIGCONTEXT_RBS_BASE_OFFSET	2512	/* 0x9d0 */
#define IA64_SIGCONTEXT_LOADRS_OFFSET	2520	/* 0x9d8 */
#define IA64_SIGFRAME_ARG0_OFFSET	0	/* 0x0 */
#define IA64_SIGFRAME_ARG1_OFFSET	8	/* 0x8 */
#define IA64_SIGFRAME_ARG2_OFFSET	16	/* 0x10 */
#define IA64_SIGFRAME_HANDLER_OFFSET	24	/* 0x18 */
#define IA64_SIGFRAME_SIGCONTEXT_OFFSET	160	/* 0xa0 */
#define IA64_CLONE_VFORK		16384	/* 0x4000 */
#define IA64_CLONE_VM			256	/* 0x100 */
#define IA64_CPU_IRQ_COUNT_OFFSET	0	/* 0x0 */
#define IA64_CPU_BH_COUNT_OFFSET	4	/* 0x4 */
#define IA64_CPU_PHYS_STACKED_SIZE_P8_OFFSET 12	/* 0xc */

#endif /* _ASM_IA64_OFFSETS_H */
