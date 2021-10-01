/* Definitions of target machine for GCC,
   for ARM with targetting the VXWorks run time environment. 
   Copyright (C) 1999, 2000, 2003 Free Software Foundation, Inc.

   Contributed by: Mike Stump <mrs@wrs.com>
   Brought up to date by CodeSourcery, LLC.
   
This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */


#define TARGET_OS_CPP_BUILTINS()		\
  do {						\
    builtin_define ("__vxworks");		\
    if (TARGET_BIG_END)				\
      builtin_define ("ARMEB");			\
    else					\
      builtin_define ("ARMEL");			\
						\
    if (arm_arch_xscale)				\
      builtin_define ("CPU=XSCALE");		\
    else if (arm_arch5)				\
      builtin_define ("CPU=ARMARCH5");		\
    else if (arm_arch4)				\
      {						\
	if (thumb_code)				\
	  builtin_define ("CPU=ARMARCH4_T");	\
	else					\
	  builtin_define ("CPU=ARMARCH4");	\
      }						\
  } while (0)

#undef  CC1_SPEC
#define CC1_SPEC							\
"%{t4:        -mapcs-32 -mlittle-endian -march=armv4 ;			\
   t4be:      -mapcs-32 -mbig-endian -march=armv4 ;			\
   t4t:       -mthumb -mthumb-interwork -mlittle-endian -march=armv4t ;	\
   t4tbe:     -mthumb -mthumb-interwork -mbig-endian -march=armv4t ;	\
   t5:        -mapcs-32 -mlittle-endian -march=armv5 ;			\
   t5be:      -mapcs-32 -mbig-endian -march=armv5 ;			\
   t5t:       -mthumb -mthumb-interwork -mlittle-endian -march=armv5 ;	\
   t5tbe:     -mthumb -mthumb-interwork -mbig-endian -march=armv5 ;	\
   txscale:   -mapcs-32 -mlittle-endian -mcpu=xscale ;			\
   txscalebe: -mapcs-32 -mbig-endian -mcpu=xscale ;			\
            : -march=armv4}"

/* The -Q options from svr4.h aren't understood and must be removed.  */
#undef  ASM_SPEC
#define ASM_SPEC \
  "%{v:-V} %{n} %{T} %{Ym,*} %{Yd,*} %{Wa,*:%*}"

#undef  TARGET_VERSION
#define TARGET_VERSION	fputs (" (ARM/VxWorks)", stderr);

/* There is no default multilib.  */
#undef MULTILIB_DEFAULTS

/* #undef NO_DOLLAR_IN_LABEL again, eventhough it's already #undef'ed
   from config/vxworks.h.

   The common #undef is overruled by arm/aout.h, and having it #defined
   influences ASM_PN_FORMAT in a way that confuses the debugger with respect
   to nested functions in Ada.  */

#undef NO_DOLLAR_IN_LABEL

/* VxWorks expects the word order for doubles to match the target endianness,
   and the default soft format is FPA which is big-endian unconditionally, so
   we redefine the default flags to the elf.h ones + vfp */

#undef TARGET_DEFAULT
#define TARGET_DEFAULT \
 (ARM_FLAG_SOFT_FLOAT | ARM_FLAG_APCS_32 | ARM_FLAG_APCS_FRAME \
  | ARM_FLAG_MMU_TRAPS | ARM_FLAG_VFP)

/* Set up a trampoline, without forgetting to flush the Icache. */
#undef  INITIALIZE_TRAMPOLINE
#define INITIALIZE_TRAMPOLINE(TRAMP, FNADDR, CXT)                      \
{                                                                      \
  emit_move_insn (gen_rtx (MEM, SImode, plus_constant ((TRAMP), 8)),   \
                 (CXT));                                               \
  emit_move_insn (gen_rtx (MEM, SImode, plus_constant ((TRAMP), 12)),  \
                 (FNADDR));                                            \
  emit_library_call (gen_rtx_SYMBOL_REF (Pmode, "__clear_cache"),      \
                    0, VOIDmode, 2, TRAMP, Pmode,                      \
                    plus_constant (TRAMP, TRAMPOLINE_SIZE), Pmode);    \
}

/* Clear the instruction cache from `BEG' to `END', resorting to the
   appropriate VxWorks service.  */
#define CLEAR_INSN_CACHE(BEG, END)                                     \
{                                                                      \
  extern int cacheTextUpdate (void *, size_t);                         \
  cacheTextUpdate ((void *)(BEG), (size_t)((END) - (BEG)));            \
}


/* VxWorks expects no minimum structure size/alignment.  */
#undef  DEFAULT_STRUCTURE_SIZE_BOUNDARY
#define DEFAULT_STRUCTURE_SIZE_BOUNDARY 8
