/*  Special support for trampolines
 *
 *   Copyright (C) 1996, 1997, 2000 Free Software Foundation, Inc.
 *   Written By Michael Meissner
 * 
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 * 
 * In addition to the permissions in the GNU General Public License, the
 * Free Software Foundation gives you unlimited permission to link the
 * compiled version of this file with other programs, and to distribute
 * those programs without any restriction coming from the use of this
 * file.  (The General Public License restrictions do apply in other
 * respects; for example, they cover modification of the file, and
 * distribution when not linked into another program.)
 * 
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * 
 *    As a special exception, if you link this library with files
 *    compiled with GCC to produce an executable, this does not cause
 *    the resulting executable to be covered by the GNU General Public License.
 *    This exception does not however invalidate any other reasons why
 *    the executable file might be covered by the GNU General Public License.
 */ 

/* Set up trampolines.  */

	.file	"tramp.asm"
	.section ".text"
	#include "ppc-asm.h"

#ifndef __powerpc64__
	.type	trampoline_initial,@object
	.align	2
trampoline_initial:
	mflr	r0
	bl	1f
.Lfunc = .-trampoline_initial
	.long	0			/* will be replaced with function address */
.Lchain = .-trampoline_initial
	.long	0			/* will be replaced with static chain */
1:	mflr	r11
	mtlr	r0
	lwz	r0,0(r11)		/* function address */
	lwz	r11,4(r11)		/* static chain */
	mtctr	r0
	bctr

trampoline_size = .-trampoline_initial
	.size	trampoline_initial,trampoline_size


/* R3 = stack address to store trampoline */
/* R4 = length of trampoline area */
/* R5 = function address */
/* R6 = static chain */

FUNC_START(__trampoline_setup)
	mflr	r0		/* save return address */
        bl	.LCF0		/* load up __trampoline_initial into r7 */
.LCF0:
        mflr	r11
        addi	r7,r11,trampoline_initial-4-.LCF0 /* trampoline address -4 */

	li	r8,trampoline_size	/* verify that the trampoline is big enough */
	cmpw	cr1,r8,r4
	srwi	r4,r4,2		/* # words to move */
	addi	r9,r3,-4	/* adjust pointer for lwzu */
	mtctr	r4
	blt	cr1,.Labort

	mtlr	r0

	/* Copy the instructions to the stack */
.Lmove:
	lwzu	r10,4(r7)
	stwu	r10,4(r9)
	bdnz	.Lmove

	/* Store correct function and static chain */
	stw	r5,.Lfunc(r3)
	stw	r6,.Lchain(r3)

#if defined (_NO_ICACHE) && defined (_NO_DCACHE)
	/* No caches, so no need to flush anything */
#else
	/* Now flush the cache(s) */
	mtctr	r4
.Lcache:
#ifndef _NO_ICACHE
	icbi	0,r3		/* invalidate instruction cache */
#endif
#ifndef _NO_DCACHE
	dcbf	0,r3		/* flush data cache */
#endif
	addi	r3,r3,4
	bdnz	.Lcache
#endif
	
	/* Finally synchronize things & return */
	sync			/* ensure update is in main memory */
	isync			/* discard prefetched instructions */
	blr

.Labort:
/* Use a longcall sequence in the simple non PIC case.  This allows the
   non-pic code to be mlongcall compliant.  The PIC+longcall case would
   be much harder to handle, and there is no real need as of today.  */
#if defined __PIC__ || defined __pic__
        bl JUMP_TARGET(abort)
#else
        addis 11, 0,JUMP_TARGET(abort)@ha
        addi  12,11,JUMP_TARGET(abort)@l
        mtlr  12
        blrl
#endif
FUNC_END(__trampoline_setup)

#endif
