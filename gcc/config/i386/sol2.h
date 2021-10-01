/* Target definitions for GCC for Intel 80386 running Solaris 2
   Copyright (C) 1993, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003
   Free Software Foundation, Inc.
   Contributed by Fred Fish (fnf@cygnus.com).

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

/* The Solaris 2.0 x86 linker botches alignment of code sections.
   It tries to align to a 16 byte boundary by padding with 0x00000090
   ints, rather than 0x90 bytes (nop).  This generates trash in the
   ".init" section since the contribution from crtbegin.o is only 7
   bytes.  The linker pads it to 16 bytes with a single 0x90 byte, and
   two 0x00000090 ints, which generates a segmentation violation when
   executed.  This macro forces the assembler to do the padding, since
   it knows what it is doing.  */
#define FORCE_CODE_SECTION_ALIGN  asm(ALIGN_ASM_OP "16");

/* Select a format to encode pointers in exception handling data.  CODE
   is 0 for data, 1 for code labels, 2 for function pointers.  GLOBAL is
   true if the symbol may be affected by dynamic relocations.  */
#undef ASM_PREFERRED_EH_DATA_FORMAT
#define ASM_PREFERRED_EH_DATA_FORMAT(CODE,GLOBAL)			\
  (flag_pic ? (GLOBAL ? DW_EH_PE_indirect : 0) | DW_EH_PE_datarel	\
   : DW_EH_PE_absptr)

/* Solaris 2/Intel as chokes on #line directives.  */
#undef CPP_SPEC
#define CPP_SPEC "%{.S:-P} %(cpp_subtarget)"

/* FIXME: Removed -K PIC from generic Solaris 2 ASM_SPEC: the native assembler
   gives many warnings: R_386_32 relocation is used for symbol ".text".  */
#undef ASM_SPEC
#define ASM_SPEC "\
%{v:-V} %{Qy:} %{!Qn:-Qy} %{n} %{T} %{Ym,*} %{Wa,*:%*} -s \
%(asm_cpu) \
"

#define ASM_CPU_SPEC ""
 
#undef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS \
  { "cpp_subtarget",	CPP_SUBTARGET_SPEC },	\
  { "asm_cpu",		ASM_CPU_SPEC },		\
  { "startfile_arch",	STARTFILE_ARCH_SPEC },	\
  { "link_arch",	LINK_ARCH_SPEC }

#undef LOCAL_LABEL_PREFIX
#define LOCAL_LABEL_PREFIX "."

/* The Solaris assembler does not support .quad.  Do not use it.  */
#undef ASM_QUAD

/* The Solaris assembler wants a .local for non-exported aliases.  */
#define ASM_OUTPUT_DEF_FROM_DECLS(FILE, DECL, TARGET)	\
  do {							\
    const char *declname =				\
      IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (DECL));	\
    ASM_OUTPUT_DEF ((FILE), declname,			\
		    IDENTIFIER_POINTER (TARGET));	\
    if (! TREE_PUBLIC (DECL))				\
      {							\
	fprintf ((FILE), "%s", LOCAL_ASM_OP);		\
	assemble_name ((FILE), declname);		\
	fprintf ((FILE), "\n");				\
      }							\
  } while (0)

#ifdef IN_LIBGCC2
#include <ucontext.h>

#define MD_FALLBACK_FRAME_STATE_FOR(CONTEXT, FS, SUCCESS)		\
  do {									\
  unsigned char *pc_ = (CONTEXT)->ra;                                   \
  mcontext_t *mctx_;                                                    \
  long new_cfa_;                                                        \
                                                                        \
  if (/* Solaris 8 - single-threaded                                    \
	----------------------------                                    \
	<sigacthandler+17>:  mov    0x10(%ebp),%esi                     \
	<sigacthandler+20>:  push   %esi                                \
	<sigacthandler+21>:  pushl  0xc(%ebp)                           \
	<sigacthandler+24>:  mov    0x8(%ebp),%ecx 2                    \
	<sigacthandler+27>:  push   %ecx                                \
	<sigacthandler+28>:  mov    offset(%ebx),%eax                   \
	<sigacthandler+34>:  call   *(%eax,%ecx,4)                      \
	<sigacthandler+37>:  add    $0xc,%esp        <--- PC            \
	<sigacthandler+40>:  push   %esi ... */                         \
      (*(unsigned long *)(pc_ - 20) == 0x5610758b                       \
       && *(unsigned long *)(pc_ - 16) == 0x8b0c75ff                    \
       && *(unsigned long *)(pc_ - 12) == 0x8b51084d                    \
       && *(unsigned char *)(pc_ - 8)  == 0x83                          \
       && *(unsigned long *)(pc_ - 4)  == 0x8814ff00                    \
       && *(unsigned long *)(pc_ - 0)  == 0x560cc483)                   \
      || /* Solaris 8 - multi-threaded                                  \
	   ---------------------------                                  \
	   <__sighndlr+0>:      push   %ebp                             \
	   <__sighndlr+1>:      mov    %esp,%ebp                        \
	   <__sighndlr+3>:      pushl  0x10(%ebp)                       \
	   <__sighndlr+6>:      pushl  0xc(%ebp)                        \
	   <__sighndlr+9>:      pushl  0x8(%ebp)                        \
	   <__sighndlr+12>:     call   *0x14(%ebp)                      \
	   <__sighndlr+15>:     leave               <--- PC  */         \
	 (*(unsigned long *)(pc_ - 15) == 0xffec8b55                    \
	  && *(unsigned long *)(pc_ - 11) == 0x75ff1075                 \
	  && *(unsigned long *)(pc_ - 7)  == 0x0875ff0c                 \
	  && *(unsigned long *)(pc_ - 3)  == 0xc91455ff)                \
      || /* Solaris 9 - single-threaded                                 \
	   ----------------------------                                 \
	   <sigacthandler+16>:    mov    0x244(%ebx),%ecx               \
	   <sigacthandler+22>:    mov    0x8(%ebp),%eax                 \
	   <sigacthandler+25>:    mov    (%ecx,%eax,4),%ecx             \
	   <sigacthandler+28>:    pushl  0x10(%ebp)                     \
	   <sigacthandler+31>:    pushl  0xc(%ebp)                      \
	   <sigacthandler+34>:    push   %eax                           \
	   <sigacthandler+35>:    call   *%ecx                          \
	   <sigacthandler+37>:    add    $0xc,%esp	<--- PC         \
	   <sigacthandler+40>:    pushl  0x10(%ebp) */                  \
	 (*(unsigned long *)(pc_ - 21) == 0x2448b8b                     \
	  && *(unsigned long *)(pc_ - 17) == 0x458b0000                 \
	  && *(unsigned long *)(pc_ - 13) == 0x810c8b08                 \
	  && *(unsigned long *)(pc_ - 9)  == 0xff1075ff                 \
	  && *(unsigned long *)(pc_ - 5)  == 0xff500c75                 \
	  && *(unsigned long *)(pc_ - 1)  == 0xcc483d1)                 \
      || /* Solaris 9 - multi-threaded, Solaris 10                      \
	   ---------------------------------------                      \
	   <__sighndlr+0>:      push   %ebp                             \
	   <__sighndlr+1>:      mov    %esp,%ebp                        \
	   <__sighndlr+3>:      pushl  0x10(%ebp)                       \
	   <__sighndlr+6>:      pushl  0xc(%ebp)                        \
	   <__sighndlr+9>:      pushl  0x8(%ebp)                        \
	   <__sighndlr+12>:     call   *0x14(%ebp)                      \
	   <__sighndlr+15>:     add    $0xc,%esp     <--- PC            \
	   <__sighndlr+18>:     leave                                   \
	   <__sighndlr+19>:     ret  */                                 \
	 (*(unsigned long *)(pc_ - 15) == 0xffec8b55                    \
	  && *(unsigned long *)(pc_ - 11) == 0x75ff1075                 \
	  && *(unsigned long *)(pc_ - 7)  == 0x0875ff0c                 \
	  && *(unsigned long *)(pc_ - 3)  == 0x831455ff                 \
	  && *(unsigned long *)(pc_ + 1)  == 0xc3c90cc4)                \
      || /* Solaris 11 before snv_125                                   \
	  --------------------------                                    \
	  <__sighndlr+0>        push   %ebp                             \
	  <__sighndlr+1>        mov    %esp,%ebp                        \
	  <__sighndlr+4>        pushl  0x10(%ebp)                       \
	  <__sighndlr+6>        pushl  0xc(%ebp)                        \
	  <__sighndlr+9>        pushl  0x8(%ebp)                        \
	  <__sighndlr+12>       call   *0x14(%ebp)                      \
	  <__sighndlr+15>	add    $0xc,%esp                        \
	  <__sighndlr+18>       leave                <--- PC            \
	  <__sighndlr+19>       ret  */                                 \
	 (*(unsigned long *)(pc_ - 18) == 0xffec8b55                    \
	  && *(unsigned long *)(pc_ - 14) == 0x7fff107f                 \
	  && *(unsigned long *)(pc_ - 10)  == 0x0875ff0c                \
	  && *(unsigned long *)(pc_ - 6)  == 0x83145fff                 \
	  && *(unsigned long *)(pc_ - 1)  == 0xc3c90cc4)                \
      || /* Solaris 11 since snv_125                                    \
	  -------------------------                                     \
	  <__sighndlr+0>        push   %ebp                             \
	  <__sighndlr+1>        mov    %esp,%ebp                        \
	  <__sighndlr+3>        and    $0xfffffff0,%esp                 \
	  <__sighndlr+6>        sub    $0x4,%esp                        \
	  <__sighndlr+9>        pushl  0x10(%ebp)                       \
	  <__sighndlr+12>       pushl  0xc(%ebp)                        \
	  <__sighndlr+15>       pushl  0x8(%ebp)                        \
	  <__sighndlr+18>       call   *0x14(%ebp)                      \
	  <__sighndlr+21>       leave                <--- PC            \
	  <__sighndlr+22>       ret  */                                 \
	 (*(unsigned long *)(pc_ - 21) == 0x83ec8b55                    \
	  && *(unsigned long *)(pc_ - 17) == 0xec83f0e4                 \
	  && *(unsigned long *)(pc_ - 13)  == 0x1075ff04                \
	  && *(unsigned long *)(pc_ - 9)  == 0xff0c75ff                 \
	  && *(unsigned long *)(pc_ - 5)  == 0x55ff0875                 \
	  && (*(unsigned long *)(pc_ - 1) & 0x00ffffff) == 0x00c3c914)) \
  {                                                                     \
      struct handler_args {                                             \
          int signo;                                                    \
          siginfo_t *sip;                                               \
          ucontext_t *ucontext;                                         \
      } *handler_args_ = (CONTEXT)->cfa;                                \
      mctx_ = &handler_args_->ucontext->uc_mcontext;                    \
  }                                                                     \
  else                                                                  \
      break;                                                            \
                                                                        \
  new_cfa_ = mctx_->gregs[UESP];                                        \
  (FS)->cfa_how = CFA_REG_OFFSET;                                       \
  (FS)->cfa_reg = 4;                                                    \
  (FS)->cfa_offset = new_cfa_ - (long) (CONTEXT)->cfa;                  \
                                                                        \
  /* The SVR4 register numbering macros aren't usable in libgcc.  */    \
  (FS)->regs.reg[0].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[0].loc.offset = (long)&mctx_->gregs[EAX] - new_cfa_;   \
  (FS)->regs.reg[3].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[3].loc.offset = (long)&mctx_->gregs[EBX] - new_cfa_;   \
  (FS)->regs.reg[1].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[1].loc.offset = (long)&mctx_->gregs[ECX] - new_cfa_;   \
  (FS)->regs.reg[2].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[2].loc.offset = (long)&mctx_->gregs[EDX] - new_cfa_;   \
  (FS)->regs.reg[6].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[6].loc.offset = (long)&mctx_->gregs[ESI] - new_cfa_;   \
  (FS)->regs.reg[7].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[7].loc.offset = (long)&mctx_->gregs[EDI] - new_cfa_;   \
  (FS)->regs.reg[5].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[5].loc.offset = (long)&mctx_->gregs[EBP] - new_cfa_;   \
  (FS)->regs.reg[8].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[8].loc.offset = (long)&mctx_->gregs[EIP] - new_cfa_;   \
  (FS)->retaddr_column = 8;                                             \
  goto SUCCESS;                                                         \
} while (0)
#endif /* IN_LIBGCC2 */
