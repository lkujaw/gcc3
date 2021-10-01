/* Definitions of target machine for GNU compiler.  Vxworks PowerPC version.
   Copyright (C) 1996, 2000, 2002, 2003 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the
   Free Software Foundation, 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA.  */

#undef  TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()		\
  do						\
    {						\
      builtin_define ("__vxworks");		\
      builtin_define ("__vxworks__");		\
    }						\
  while (0)

/* We have to kill off the entire specs set created by rs6000/sysv4.h
   and substitute our own set.  The top level vxworks.h has done some
   of this for us.  */

#undef SUBTARGET_EXTRA_SPECS
#undef CPP_SPEC
#undef CC1_SPEC
#undef ASM_SPEC

#define SUBTARGET_EXTRA_SPECS /* none needed */

#define CPP_SPEC \
"-DCPU_FAMILY=PPC -D__ppc -D__EABI__  \
 %{t403: -DCPU=PPC403 -D_SOFT_FLOAT ; \
   t405: -DCPU=PPC405 -D_SOFT_FLOAT ; \
   t440: -DCPU=PPC440 -D_SOFT_FLOAT ; \
   t505: -DCPU=PPC505 -D_NO_ICACHE -D_NO_DCACHE ; \
   t603: -DCPU=PPC603               ; \
   t604: -DCPU=PPC604               ; \
   t860: -DCPU=PPC860 -D_SOFT_FLOAT ; \
       : -DCPU=PPC604}  \
 %{!msoft-float:-D__hardfp}	   \
 %{fpic|fpie: -D__PIC__=1 -D__pic__=1 ; \
   fPIC|fPIE: -D__PIC__=2 -D__pic__=2 } \
 %(cpp_cpu)" \
ADDITIONAL_CPP_SPEC

#define CC1_SPEC \
"%{t403: -mcpu=403 -mstrict-align ;				\
   t405: -mcpu=405 -mstrict-align ;				\
   t440: -mcpu=440 -mstrict-align ;				\
   t505: -mcpu=505 -mstrict-align ;				\
   t603: -mcpu=603 -mstrict-align ;				\
   t604: -mcpu=604 -mstrict-align ;				\
   t860: -mcpu=860                ;                             \
       : -mcpu=604 -mstrict-align }				\
 %{G*} %{mno-sdata:-msdata=none} %{msdata:-msdata=default}	\
 %{mlittle|mlittle-endian:-mstrict-align}			\
 %{profile: -p}							\
 %{fvec:-maltivec} %{fvec-eabi:-maltivec -mabi=altivec}"
   
#define ASM_SPEC "%(asm_cpu) \
%{.s: %{mregnames} %{mno-regnames}} %{.S: %{mregnames} %{mno-regnames}} \
%{v:-V} %{Qy:} %{!Qn:-Qy} %{n} %{T} %{Ym,*} %{Yd,*} %{Wa,*:%*} \
%{mrelocatable} %{mrelocatable-lib} %{fpic:-K PIC} %{fPIC:-K PIC} -mbig"

#undef  MULTILIB_DEFAULTS
#define MULTILIB_DEFAULTS { "t604" }

/* We can't use .ctors/.dtors sections.  */
#undef TARGET_ASM_OUTPUT_CONSTRUCTOR
#undef TARGET_ASM_OUTPUT_DESTRUCTOR

/* Nor sdata.  */
#undef  SDATA_DEFAULT_SIZE
#define SDATA_DEFAULT_SIZE 0

/* VxWorks interrupts may clobber any part of unallocated stack, so ... */
#undef INTERRUPTS_CLOBBER_STACK
#define INTERRUPTS_CLOBBER_STACK 1

/* To allow table based unwinding and exception propagation on this target,
   we currently:

   o let the compiler generate the DWARF info into the .eh_frame section,

   o resort to the crtstuff machinery to...

     - provide the __EH_FRAME_BEGIN__ symbol at the beginning of this section
       in the "final" binary (we have EH_FRAME_SECTION_NAME defined),

     - provide a __do_global_ctors function to trigger the static constructors
       and register the frame info (for which we define HAS_INIT_SECTION and
       USE_EH_FRAME_REGISTRY here, and undefine INIT/FINI_SECTION_ASM_OP to
       avoid relying on the .init/.fini regular elf behavior which does not
       apply to VxWorks),

     - define the _ctors/_dtors arrays for application modules, that VxWorks
       recognizes as constructors and destructors tables, to trigger an
       automatic call to __do_global_ctors/__do_global_dtors at dynamic
       load/unload time.  Doing that is not always appropriate (e.g. when
       linking against the VxWorks kernel), so we actually use one set of crt
       objects with those definitions and one without.  The latter is dragged
       when "-static" is on the link command line, and the former is dragged
       when "-dynamic" is on the link command line.  See the SPECs below.
       When -static is used, and so is the set of crt objects without the
       definitions, another circuitry should be used to trigger the calls.

   o rely on the VxWorks default linker script to drag the .eh_frame sections
     of the individual objects into the .data section of each module.
*/

/* State we're using the basic crtstuff objects. Add crt0.o when using
   RTPs. */

#undef  STARTFILE_SPEC
#define STARTFILE_SPEC "%{mrtp:%{!shared:crt0.o%s}} \
	%{static:crtbeginS.o%s} %{dynamic:crtbegin.o%s}"

#undef  ENDFILE_SPEC
#define ENDFILE_SPEC "%{static:crtendS.o%s} %{dynamic:crtend.o%s}"

/* Set things up for them to include just what we want. Merely undefining
   INIT/FINI_SECTION_ASM_OP triggers undefined references to symbols of those
   names indirectly from EXTRA_SECTION_FUNCTIONS, which we drag from svr4.h
   but actually don't care about, so we work-around that. */

#define HAS_INIT_SECTION
#define USE_EH_FRAME_REGISTRY

#undef  INIT_SECTION_ASM_OP
#undef  INIT_SECTION_FUNCTION
#define INIT_SECTION_FUNCTION

#undef  FINI_SECTION_ASM_OP
#undef  FINI_SECTION_FUNCTION
#define FINI_SECTION_FUNCTION

#undef  DWARF2_UNWIND_INFO
#define DWARF2_UNWIND_INFO 1

/* We don't deal with Java for the time beeing, and the crtstuff bits we drag
   reference some _Jv registration routine if we let JCR_DECTION_NAME defined
   as it is by default.  We undefine that here to avoid unresolved references
   complaints from the VxWorks loader.  */
#include <defaults.h>
#undef JCR_SECTION_NAME

