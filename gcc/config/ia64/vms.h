/* Output variables, constants and external declarations, for GNU compiler.
   Copyright (C) 2003
   Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#define TARGET_OBJECT_SUFFIX ".obj"
#define TARGET_EXECUTABLE_SUFFIX ".exe"

#define OBJECT_FORMAT_ELF

#define TARGET_OS_CPP_BUILTINS()		\
    do {					\
	builtin_define_std ("vms");		\
	builtin_define_std ("VMS");		\
	builtin_define ("__IA64");		\
	builtin_assert ("system=vms");		\
	builtin_define ("__IEEE_FLOAT");	\
    } while (0)

/* By default, allow $ to be part of an identifier.  */
#define DOLLARS_IN_IDENTIFIERS 2

#undef TARGET_ABI_OPEN_VMS
#define TARGET_ABI_OPEN_VMS 1

#undef TARGET_NAME   
#define TARGET_NAME "OpenVMS/IA64"
#undef TARGET_VERSION
#define TARGET_VERSION fprintf (stderr, " (%s)", TARGET_NAME);           

/* Need .debug_line info generated from gcc and gas */
#undef TARGET_DEFAULT
#define TARGET_DEFAULT MASK_GNU_AS

extern const char *ia64_debug_main;

#undef SUBTARGET_OPTIONS
#define SUBTARGET_OPTIONS				\
  {"debug-main=", &ia64_debug_main,			\
   N_("Set name of main routine for the debugger"), 0}

#define VMS_DEBUG_MAIN_POINTER "TRANSFER$BREAK$GO"

/* "long" is 32 bits, but 64 bits for Ada.  */
#undef LONG_TYPE_SIZE
#define LONG_TYPE_SIZE 32
#define ADA_LONG_TYPE_SIZE 64

/* Pointer is 32 bits but the hardware has 64-bit addresses, sign extended.  */
#undef POINTER_SIZE
#define POINTER_SIZE 32
#define POINTERS_EXTEND_UNSIGNED 0

/* HP OpenVMS Calling Standard dated June, 2004, that describes
   HP OpenVMS I64 Version 8.2EFT
   chapter 4 "OpenVMS I64 Conventions"
   section 4.7 "Procedure Linkage"
   subsection 4.7.5.2, "Normal Register Parameters"

   "Unsigned integral (except unsigned 32-bit), set, and VAX
   floating-point values passed in registers are zero-filled;
   signed integral values as well as unsigned 32-bit integral
   values are sign-extended to 64 bits.  For all other types
   passed in the general registers, unused bits are undefined."  */
#define PROMOTE_FUNCTION_MODE(MODE,UNSIGNEDP,TYPE)	\
  if (GET_MODE_CLASS (MODE) == MODE_INT			\
      && GET_MODE_SIZE (MODE) < UNITS_PER_WORD)		\
    {							\
      if ((MODE) == SImode)				\
	(UNSIGNEDP) = 0;				\
      (MODE) = DImode;					\
    }

#define PROMOTE_FUNCTION_ARGS
#define PROMOTE_FUNCTION_RETURN

/* The structure return address arrives as an "argument" on VMS.  */
#undef PCC_STATIC_STRUCT_RETURN

/* VMS debugger expects a separator */
#define DWARF2_DIR_SHOULD_END_WITH_SEPARATOR 1

#define ASM_OUTPUT_DWARF_DELTA_UNITS(FILE,SIZE,LABEL1,LABEL2,UNITS) \
do {                                \
  fprintf (FILE, "\tdata4.ua\t ("); \
  assemble_name (FILE, LABEL1);     \
  fprintf (FILE, "-");              \
  assemble_name (FILE, LABEL2);     \
  fprintf (FILE, ")/16*3");         \
} while (0)

#undef STARTFILE_SPEC
#define STARTFILE_SPEC \
"%{!shared:%{mvms-return-codes:vcrt0.o%s} %{!mvms-return-codes:pcrt0.o%s} \
    crtbegin.o%s} \
 %{!static:%{shared:crtinitS.o%s crtbeginS.o%s}}"

#undef ENDFILE_SPEC
#define ENDFILE_SPEC \
"%{!shared:crtend.o%s} %{!static:%{shared:crtendS.o%s}}"

#define LINK_GCC_C_SEQUENCE_SPEC "%G"

#undef LINK_SPEC
#define LINK_SPEC "%{g*} %{map} %{save-temps} %{shared} %{v}"

#ifdef LIB_SPEC
#undef LIB_SPEC
#endif
#define LIB_SPEC ""

#ifdef ASM_SPEC
#undef ASM_SPEC
#endif
#define ASM_SPEC                                          \
  "%{mno-gnu-as:-N so -N vms_upcase -W DVLoc_off}         \
   %{mconstant-gp:-M const_gp} %{mauto-pic:-M no_plabel}"

#undef ASM_OUTPUT_EXTERNAL_LIBCALL
#define ASM_OUTPUT_EXTERNAL_LIBCALL(FILE, FUN)			\
do {								\
  (*targetm.asm_out.globalize_label) (FILE, XSTR (FUN, 0));	\
  ASM_OUTPUT_TYPE_DIRECTIVE (FILE, XSTR (FUN, 0), "function");	\
} while (0)

/* Set the function to change the names of the division and modulus
   functions.   */
#undef TARGET_INIT_LIBFUNCS
#define TARGET_INIT_LIBFUNCS ia64_vms_init_libfuncs

#define NAME__MAIN "__gccmain"
#define SYMBOL__MAIN __gccmain

#define CTOR_LIST_BEGIN asm (".global\tLIB$INITIALIZE#\n");                  \
STATIC func_ptr __CTOR_LIST__[1]                                             \
  __attribute__ ((__unused__, section(".ctors"), aligned(sizeof(func_ptr)))) \
  = { (func_ptr) (-1) };

#undef INIT_SECTION_ASM_OP
#define INIT_SECTION_ASM_OP ".section\tLIB$INITIALIZE#,\"a\",@progbits"

#define CRT_CALL_STATIC_FUNCTION(SECTION_OP, FUNC)      \
  asm (SECTION_OP "\n\tdata4 @fptr(" #FUNC"#)\n");      \
  FORCE_CODE_SECTION_ALIGN                            \
  asm (TEXT_SECTION_ASM_OP);

#undef FINI_SECTION_ASM_OP

/* Maybe same as HPUX?  Needs to be checked. */
#define JMP_BUF_SIZE  (8 * 76)

typedef struct crtl_name_spec
{
  const char *const name;
  const char *const deccname;
  int referenced;
} crtl_name_spec;

#include "config/vms/vms-crtl.h"

#define DO_CRTL_NAMES                                      \
  do                                                       \
    {                                                      \
      int i;                                               \
      static crtl_name_spec vms_crtl_names[] = CRTL_NAMES; \
                                                           \
      for (i=0; vms_crtl_names [i].name; i++)              \
	if (!vms_crtl_names [i].referenced &&              \
	    strcmp (name, vms_crtl_names [i].name) == 0)   \
	  {                                                \
	    fprintf (file, "\t.alias %s, \"%s\"\n",        \
		     name, vms_crtl_names [i].deccname);   \
	    vms_crtl_names [i].referenced = 1;             \
	  }                                                \
    } while (0)

#define OPTIMIZATION_OPTIONS(LEVEL,SIZE)                   \
  do {                                                     \
       flag_merge_constants = 0;                           \
  } while (0)

#if 0
/* A C expression for the size in bytes of the trampoline, as an integer.  */

#undef TRAMPOLINE_SIZE
#define TRAMPOLINE_SIZE		48

/* A C statement to initialize the variable parts of a trampoline.  */

#undef INITIALIZE_TRAMPOLINE
#define INITIALIZE_TRAMPOLINE(ADDR, FNADDR, STATIC_CHAIN) \
  ia64_vms_initialize_trampoline((ADDR), (FNADDR), (STATIC_CHAIN))
#endif

/* Define this to be nonzero if static stack checking is supported.  */
#define STACK_CHECK_STATIC_BUILTIN 1

#define MD_FALLBACK_FRAME_STATE_FOR_SOURCE "config/ia64/vms-ehfb.h"

