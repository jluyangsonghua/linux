/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_CURRENT_H
#define __ASM_CURRENT_H

#include <linux/compiler.h>

#ifndef __ASSEMBLY__

struct task_struct;

/*
 * We don't use read_sysreg() as we want the compiler to cache the value where
 * possible.
 */
static __always_inline struct task_struct *get_current(void)//current的定义
{
	unsigned long sp_el0;

	asm ("mrs %0, sp_el0" : "=r" (sp_el0));//是通过使用el0模式下的sp寄存器(因为此时是运行于内核模式，使用的栈寄存器是sp_el1)

	return (struct task_struct *)sp_el0;
}

#define current get_current()

#endif /* __ASSEMBLY__ */

#endif /* __ASM_CURRENT_H */

