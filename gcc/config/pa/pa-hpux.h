/* Definitions of target machine for GNU compiler, for HP-UX.
   Copyright (C) 1991, 1995, 1996, 2002, 2003 Free Software Foundation, Inc.

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

#undef TARGET_DEFAULT
#define TARGET_DEFAULT MASK_DISABLE_INDEXING
/* We disable indexing by default because it very strongly relies on
   REG_POINTER beeing properly set, which turns out not to be case out of a
   number of significant passes (cse, reload, ...).  In various places the
   back-end code attempts to alleviate that, not always in a very successful
   way, leading to a range of symptoms from unrecognizable instruction ICEs
   to wrong code generation.  */

/* Make GCC agree with types.h.  */
#undef SIZE_TYPE
#undef PTRDIFF_TYPE

#define SIZE_TYPE "unsigned int"
#define PTRDIFF_TYPE "int"

#define LONG_DOUBLE_TYPE_SIZE 128
#define HPUX_LONG_DOUBLE_LIBRARY
#define FLOAT_LIB_COMPARE_RETURNS_BOOL(MODE, COMPARISON) ((MODE) == TFmode)

/* GCC always defines __STDC__.  HP C++ compilers don't define it.  This
   causes trouble when sys/stdsyms.h is included.  As a work around,
   we define __STDC_EXT__.  A similar situation exists with respect to
   the definition of __cplusplus.  We define _INCLUDE_LONGLONG
   to prevent nlist.h from defining __STDC_32_MODE__ (no longlong
   support).  */
#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()				\
  do								\
    {								\
	builtin_assert ("system=hpux");				\
	builtin_assert ("system=unix");				\
	builtin_define ("__hp9000s800");			\
	builtin_define ("__hp9000s800__");			\
	builtin_define ("__hp9k8");				\
	builtin_define ("__hp9k8__");				\
	builtin_define ("__hpux");				\
	builtin_define ("__hpux__");				\
	builtin_define ("__unix");				\
	builtin_define ("__unix__");				\
	if (c_dialect_cxx ())					\
	  {							\
	    builtin_define ("_HPUX_SOURCE");			\
	    builtin_define ("_INCLUDE_LONGLONG");		\
	    builtin_define ("__STDC_EXT__");			\
	  }							\
	else if (!flag_iso)					\
	  {							\
	    builtin_define ("_HPUX_SOURCE");			\
	    if (preprocessing_trad_p ())			\
	      {							\
		builtin_define ("hp9000s800");			\
		builtin_define ("hp9k8");			\
		builtin_define ("hppa");			\
		builtin_define ("hpux");			\
		builtin_define ("unix");			\
		builtin_define ("__CLASSIC_C__");		\
		builtin_define ("_PWB");			\
		builtin_define ("PWB");				\
	      }							\
	    else						\
	      builtin_define ("__STDC_EXT__");			\
	  }							\
	if (TARGET_SIO)						\
	  builtin_define ("_SIO");				\
	else							\
	  {							\
	    builtin_define ("__hp9000s700");			\
	    builtin_define ("__hp9000s700__");			\
	    builtin_define ("_WSIO");				\
	  }							\
    }								\
  while (0)

#undef SUBTARGET_SWITCHES
#define SUBTARGET_SWITCHES \
  { "sio",	 MASK_SIO,	N_("Generate cpp defines for server IO") }, \
  { "wsio",	-MASK_SIO,	N_("Generate cpp defines for workstation IO") },

/* Like the default, except no -lg.  */
#undef LIB_SPEC
#define LIB_SPEC "%{!shared:%{!p:%{!pg:-lc}}%{p: -L/lib/libp/ -lc}%{pg: -L/lib/libp/ -lc}}"

#undef LINK_SPEC
#if ((TARGET_DEFAULT | TARGET_CPU_DEFAULT) & MASK_PA_11)
#define LINK_SPEC \
  "%{!mpa-risc-1-0:%{!shared:-L/lib/pa1.1 -L/usr/lib/pa1.1 }}%{mlinker-opt:-O} %{!shared:-u main} %{static:-a archive} %{g*:-a archive} %{shared:-b}"
#else
#define LINK_SPEC \
  "%{mlinker-opt:-O} %{!shared:-u main} %{static:-a archive} %{g*:-a archive} %{shared:-b}"
#endif

/* hpux8 and later have C++ compatible include files, so do not
   pretend they are `extern "C"'.  */
#define NO_IMPLICIT_EXTERN_C

/* hpux11 and earlier don't have fputc_unlocked, so we must inhibit the
   transformation of fputs_unlocked and fprintf_unlocked to fputc_unlocked.  */
#define DONT_HAVE_FPUTC_UNLOCKED

/* We want the entry value of SP saved in the frame marker for
   compatibility with the HP-UX unwind library.  */
#undef TARGET_HPUX_UNWIND_LIBRARY
#define TARGET_HPUX_UNWIND_LIBRARY 1

/* Define the necessary stuff for the DWARF2 CFI support.  */

#define NO_PROFILE_COUNTERS 1
#define DWARF2_UNWIND_INFO 1

/* This macro chooses the encoding of pointers embedded in the exception
   handling sections.  If at all possible, this should be defined such
   that the exception handling section will not require dynamic relocations,
   and so may be read-only.

   FIXME:  This should use an indirect data relative encoding for code
   labels and function pointers.  We used DW_EH_PE_aligned to output
   a PLABEL constructor.  */
#undef ASM_PREFERRED_EH_DATA_FORMAT
#define ASM_PREFERRED_EH_DATA_FORMAT(CODE,GLOBAL)                     \
  (CODE == 2 && GLOBAL ? DW_EH_PE_aligned : DW_EH_PE_absptr)

/* Handle special EH pointer encodings.  Absolute, pc-relative, and
   indirect are handled automatically.  Since pc-relative encodining is
   not possible on the PA and we don't have the infrastructure for
   data relative encoding, we use aligned plabels for code labels
   and function pointers.  */
#define ASM_MAYBE_OUTPUT_ENCODED_ADDR_RTX(FILE, ENCODING, SIZE, ADDR, DONE) \
  do {                                                                       \
    if (((ENCODING) & 0x0F) == DW_EH_PE_aligned)                      \
      {                                                                      \
      fputs (integer_asm_op ((SIZE), FALSE), FILE);                   \
      fputs ("P%", FILE);                                             \
      assemble_name (FILE, XSTR ((ADDR), 0));                         \
      goto DONE;                                                      \
      }                                                                      \
    } while (0)


/* A C expression whose value is RTL representing the location of the incoming
   return address at the beginning of any function, before the prologue. This
   RTL is either a REG, indicating that the return value is saved in `REG', or
   a MEM representing a location in the stack.

   You only need to define this macro if you want to support call frame
   debugging information like that provided by DWARF 2.

   If this RTL is a REG, you should also define DWARF_FRAME_RETURN_COLUMN to
   DWARF_FRAME_REGNUM (REGNO).  */
#define INCOMING_RETURN_ADDR_RTX  (gen_rtx_REG (word_mode, 2))
#define DWARF_FRAME_RETURN_COLUMN (DWARF_FRAME_REGNUM (2))

/* A C expression whose value is an integer giving the offset, in bytes, from
   the value of the stack pointer register to the top of the stack frame at
   the beginning of any function, before the prologue. The top of the frame is
   defined to be the value of the stack pointer in the previous frame, just
   before the call instruction.

   You only need to define this macro if you want to support call frame
   debugging information like that provided by DWARF 2.  */
#define INCOMING_FRAME_SP_OFFSET 0

#define MD_FALLBACK_FRAME_STATE_FOR_SOURCE "config/pa/hpux-ehfb.h"

/* Define this to be nonzero if static stack checking is supported.  */
#define STACK_CHECK_STATIC_BUILTIN 1
