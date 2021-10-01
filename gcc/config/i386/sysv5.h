/* Definitions for Intel 386 running System V Release 5 (i.e. UnixWare 7)
   Copyright (C) 2005 Free Software Foundation, Inc.
   Contributed by Kean Johnston (jkj@sco.com)

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

/* The .file command should always begin the output.  */
#define TARGET_ASM_FILE_START_FILE_DIRECTIVE true

#define TARGET_VERSION fprintf (stderr, " (i386 System V Release 5)");

/* Emit '.version\t"01.01"' like the native UnixWare compiler.  */
#undef X86_FILE_START_VERSION_DIRECTIVE
#define X86_FILE_START_VERSION_DIRECTIVE true

/* The svr4 ABI for the i386 says that records and unions are returned
   in memory.  */
#undef DEFAULT_PCC_STRUCT_RETURN
#define DEFAULT_PCC_STRUCT_RETURN 1

#if !defined(USE_GNU_AS)
#undef ASM_QUAD
#undef DBX_DEBUGGING_INFO
#endif

#if !defined(USE_GNU_LD)
/* Unlike GNU ld, the native UnixWare linker does not coalesce named
   .ctors.N/.dtors.N sections. Hence we define CTORS_SECTION_ASM_OP and
   DTORS_SECTION_ASM_OP so that are always emitted in the same section.
   Otherwise, many C++ constructors and destructors will never be run. */
#define CTORS_SECTION_ASM_OP		"\t.section\t.ctors, \"aw\""
#define DTORS_SECTION_ASM_OP		"\t.section\t.dtors, \"aw\""
#endif

/* If defined, a C expression whose value is a string containing the
   assembler operation to identify the following data as
   uninitialized global data.  If not defined, and neither
   `ASM_OUTPUT_BSS' nor `ASM_OUTPUT_ALIGNED_BSS' are defined,
   uninitialized global data will be output in the data section if
   `-fno-common' is passed, otherwise `ASM_OUTPUT_COMMON' will be
   used.  */
#undef BSS_SECTION_ASM_OP
#define BSS_SECTION_ASM_OP		"\t.section\t.bss, \"aw\", @nobits"

/* A C statement (sans semicolon) to output to the stdio stream
   FILE the assembler definition of uninitialized global DECL named
   NAME whose size is SIZE bytes and alignment is ALIGN bytes.
   Try to use asm_output_aligned_bss to implement this macro.  */

#define ASM_OUTPUT_ALIGNED_BSS(FILE, DECL, NAME, SIZE, ALIGN) \
  asm_output_aligned_bss (FILE, DECL, NAME, SIZE, ALIGN)

#undef DBX_REGISTER_NUMBER
#define DBX_REGISTER_NUMBER(n)  svr4_dbx_register_map[n]

#undef DWARF2_UNWIND_INFO
#define DWARF2_UNWIND_INFO		1

#undef NO_IMPLICIT_EXTERN_C
#define NO_IMPLICIT_EXTERN_C		1

#undef SWITCH_TAKES_ARG
#define SWITCH_TAKES_ARG(CHAR)                                          \
  (DEFAULT_SWITCH_TAKES_ARG(CHAR)					\
   || (CHAR) == 'h'                                                     \
   || (CHAR) == 'R'                                                     \
   || (CHAR) == 'Y'                                                     \
   || (CHAR) == 'z')

#undef WORD_SWITCH_TAKES_ARG
#define WORD_SWITCH_TAKES_ARG(STR)					\
 (DEFAULT_WORD_SWITCH_TAKES_ARG (STR)					\
  && strcmp (STR, "Tdata") && strcmp (STR, "Ttext")			\
  && strcmp (STR, "Tbss"))

/*
 * Define sizes and types
 */
#undef SIZE_TYPE
#define SIZE_TYPE "unsigned int"

#undef PTRDIFF_TYPE
#define PTRDIFF_TYPE "int"

#undef WCHAR_TYPE
#define WCHAR_TYPE "long int"

#undef WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE BITS_PER_WORD

#undef WINT_TYPE
#define WINT_TYPE "long int"

#undef MD_STARTFILE_PREFIX
#define MD_STARTFILE_PREFIX	"/usr/ccs/lib/"

/*
 * Use crti.o for shared objects, crt1.o for normal executables. Make sure
 * to recognize both -G and -shared as a valid way of introducing shared
 * library generation. This is important for backwards compatibility.
 *
 * For profiling support, we immitate what the native compiler does, thus:
 * -p (normal prof support)
 *    Use mcrt1.o instead of crt1.o
 *    Add -lprof
 *    Change paths to look in /usr/ccs/lib/libp and /usr/lib/libp first
 *    Add -I /usr/ccs/lib/libp/libc.so.1 (sets interpreter)
 * -pl (line profiling, the equivalent of the native -ql flag for lprof)
 *    Use pcrt1.o instead of crt1.o
 *    Add -lprof
 * -pf (flow profiling, for fprof)
 *    Add -lfprof
 *
 * Support the same -X flags as the native compiler, thus:
 * -Xa - use values-Xa.o
 * -Xb - use values-Xa.o
 * -Xc - use values-Xc.o
 * -Xt - use values-Xt.o
 *
 * Support pthreads as the native compiler, thus:
 * -pthread (POSIX threads semantics)
 *   add -lthread before -lc
 *   add values-pthread.o after values-X[act].o
 * -pthreadT (POSIX threads semantics)
 *   add -lthreadT before -lc
 *   add values-pthread.o after values-X[act].o
 */

#undef STARTFILE_SPEC
#define STARTFILE_SPEC \
 "%{!shared:\
   %{!symbolic:\
    %{!G:\
     %{p:mcrt1.o%s}%{pl:pcrt1.o%s}%{!p:%{!pl:crt1.o%s}}}}}\
  crti.o%s\
  %{ansi:values-Xc.o%s}\
   %{!ansi:%{traditional:values-Xt.o%s}\
    %{!traditional:%{Xa:values-Xa.o%s}\
     %{!Xa:%{Xb:values-Xa.o%s}\
      %{!Xb:%{Xc:values-Xc.o%s}\
       %{!Xc:%{Xt:values-Xt.o%s}\
        %{!Xt:values-Xa.o%s}}}}}}\
  %{pthread:values-pthread.o%s}%{pthreadT:values-pthread.o%s}\
  crtbegin.o%s"

#undef ENDFILE_SPEC
#define ENDFILE_SPEC "crtend.o%s crtn.o%s"

/* Names to predefine in the preprocessor for this target machine.  */
#define TARGET_OS_CPP_BUILTINS()                        \
    do {                                                \
        builtin_define ("__UNIXWARE__");                \
        builtin_define_std ("unix");                    \
        builtin_define_std ("sco");                     \
        builtin_assert ("system=unix");                 \
        if (flag_pic)                                   \
        {                                               \
            builtin_define ("__PIC__");                 \
            builtin_define ("__pic__");                 \
        }                                               \
    } while (0)

#undef CPP_SPEC
#define CPP_SPEC \
 "%{pthread:%{pthreadT:%e-pthread and -pthreadT are mutually exclusive}}\
  %{pthread:-D_REENTRANT -D_THREAD_SAFE}\
  %{pthreadT:-D_REENTRANT -D_THREAD_SAFE}"

#undef LINK_SPEC
#ifdef USE_GNU_LD
#define LINK_SPEC "%{!r:\
  %{Wl,*:%*} %{YP,*} %{G:%{!shared:-shared}}\
  %{shared:-shared %{rpath*} %{h*}\
   %{static:%e-shared and -static are mutually exclusive}\
   %{Bstatic:%e-shared and -Bstatic are mutually exclusive}\
   %{dn:%e-shared and -dn are mutually exclusive}}\
  %{pg:%e-pg not supported on this platform}\
  %{p:%{pf:%e-p and -pf are mutually exclusive}}\
  %{p:%{pl:%e-p and -pl are mutually exclusive}}\
  %{pf:%{pl:%e-pf and -pl are mutually exclusive}}\
  %{!shared:%{!static:%{!Bstatic:\
   %{rdynamic:-export-dynamic}\
   %{symbolic:-Bsymbolic} %{Bsymbolic:-Bsymbolic}}}}\
  %{static:-static} %{Bstatic:-static}\
  %{z*} %{R*}}"
#else
# define LINK_SPEC \
 "%{pg:%e-pg not supported on this platform}\
  %{p:%{pf:%e-p and -pf are mutually exclusive}}\
  %{p:%{pl:%e-p and -pl are mutually exclusive}}\
  %{pf:%{pl:%e-pf and -pl are mutually exclusive}}\
  %{!r:%{Wl,*:%*} %{YP,*} %{YL,*} %{YU,*}\
   %{!YP,*:%{p:-YP,/usr/ccs/lib/libp:/usr/lib/libp:/usr/ccs/lib:/usr/lib}\
  %{!p:-YP,/usr/ccs/lib:/usr/lib}}\
  %{h*} %{static:-dn -Bstatic %{G:%e-G and -static are mutually exclusive}\
   %{shared:%e-shared and -static are mutually exclusive}}\
  %{shared:%{!G:-G -dy}} %{G:%{!shared:-G -dy}} %{shared:%{G:-G -dy}}\
  %{symbolic:-Bsymbolic -G} %{z*} %{R*} %{Y*}\
  %{Qn:} %{!Qy:-Qn} -z alt_resolve}"
#endif

/* Library spec. If we are not building a shared library, provide the
   standard libraries, as per the system compiler.  */

#undef LIB_SPEC
#ifdef USE_GNU_LD
# define LIB_SPEC \
 "%{pthread:%{pthreadT:%e-pthread and -pthreadT are mutually exclusive}}\
  %{shared:%{!G:pic/%{pthread:pthread/}%{pthreadT:pthread/}libgcc.a%s}}\
  %{G:%{!shared:pic/%{pthread:pthread/}%{pthreadT:pthread/}libgcc.a%s}}\
  %{shared:%{G:pic/%{pthread:pthread/}%{pthreadT:pthread/}libgcc.a%s}}\
  --as-needed\
  %{p:-lprof} %{pl:-lprof} %{pf:-lfprof}\
  %{pthread:-lthread} %{pthreadT:-lthreadT}\
  %{!shared:%{!symbolic:%{!G:-lc -lcrt\
   %{p:-I /usr/ccs/lib/libp/libc.so.1}}}}"
#else
# define LIB_SPEC \
 "%{pthread:%{pthreadT:%e-pthread and -pthreadT are mutually exclusive}}\
  %{shared:%{!G:pic/%{pthread:pthread/}%{pthreadT:pthread/}libgcc.a%s}}\
  %{shared:%{!G:pic/%{lpthread:pthread/}%{lpthreadT:pthread/}libgcc.a%s}}\
  %{G:%{!shared:pic/%{pthread:pthread/}%{pthreadT:pthread/}libgcc.a%s}}\
  %{G:%{!shared:pic/%{lpthread:pthread/}%{lpthreadT:pthread/}libgcc.a%s}}\
  %{shared:%{G:pic/%{pthread:pthread/}%{pthreadT:pthread/}libgcc.a%s}}\
  %{shared:%{G:pic/%{lpthread:pthread/}%{lpthreadT:pthread/}libgcc.a%s}}\
  %{p:-lprof} %{pl:-lprof} %{pf:-lfprof}\
  %{pthread:-lthread} %{pthreadT:-lthreadT}\
  %{lpthread:-lthread} %{lpthreadT:-lthreadT}\
  %{!shared:%{!symbolic:%{!G:-lc -lcrt\
   %{p:-I /usr/ccs/lib/libp/libc.so.1}}}}"
#endif

#undef LIBGCC_SPEC
#define LIBGCC_SPEC \
 "%{!shared:%{!G:-lgcc}}"

#undef THREAD_MODEL_SPEC
#define THREAD_MODEL_SPEC \
  "%{!pthread:%{!pthreadT:single}%{pthreadT:posix}}%{pthread:posix}"

/* Handle special EH pointer encodings.  Absolute, pc-relative, and
   indirect are handled automatically.  */
#define ASM_MAYBE_OUTPUT_ENCODED_ADDR_RTX(FILE, ENCODING, SIZE, ADDR, DONE) \
  do {									\
    if ((SIZE) == 4 && ((ENCODING) & 0x70) == DW_EH_PE_datarel)		\
      {									\
        fputs (ASM_LONG, FILE);			\
        assemble_name (FILE, XSTR (ADDR, 0));				\
	fputs (((ENCODING) & DW_EH_PE_indirect ? "@GOT" : "@GOTOFF"), FILE); \
        goto DONE;							\
      }									\
  } while (0)

/* Used by crtstuff.c to initialize the base of data-relative relocations.
   These are GOT relative on x86, so return the pic register.  */
#ifdef __PIC__
#define CRT_GET_RFIB_DATA(BASE)			\
  {						\
    register void *ebx_ __asm__("ebx");		\
    BASE = ebx_;				\
  }
#else
#define CRT_GET_RFIB_DATA(BASE)						\
  __asm__ ("call\t.LPR%=\n"						\
	   ".LPR%=:\n\t"						\
	   "popl\t%0\n\t"						\
	   /* Due to a GAS bug, this cannot use EAX.  That encodes	\
	      smaller than the traditional EBX, which results in the	\
	      offset being off by one.  */				\
	   "addl\t$_GLOBAL_OFFSET_TABLE_+[.-.LPR%=],%0"			\
	   : "=d"(BASE))
#endif

#ifdef IN_LIBGCC2
#include <signal.h>
#include <sys/ucontext.h>

#define MD_FALLBACK_FRAME_STATE_FOR(CONTEXT, FS, SUCCESS)		\
do {									\
  unsigned char *pc_ = (CONTEXT)->ra;                                   \
  mcontext_t *mctx_;                                                    \
  long new_cfa_;                                                        \
                                                                        \
/* UnixWare 7 - single-threaded [libc.so.1, sigacthandler (0x40290)]    \
4029d:       81 c3 d5 4a 05 00       add    $0x54ad5,%ebx               \
402a3:       8b 4c 24 14             mov    0x14(%esp),%ecx             \
402a7:       8b 44 24 10             mov    0x10(%esp),%eax             \
402ab:       51                      push   %ecx                        \
402ac:       50                      push   %eax                        \
402ad:       ff 54 24 24             call   *0x24(%esp)                 \
402b1:       8b 54 24 20             mov    0x20(%esp),%edx  <-- PC */  \
     if ((*(unsigned long *)(pc_ - 20) == 0x4ad5c381                    \
       && *(unsigned long *)(pc_ - 16) == 0x4c8b0005                    \
       && *(unsigned long *)(pc_ - 12) == 0x448b1424                    \
       && *(unsigned long *)(pc_ - 8)  == 0x50511024                    \
       && *(unsigned long *)(pc_ - 4)  == 0x242454ff                    \
       && *(unsigned long *)(pc_)      == 0x2024548b) ||                \
/* UW 7 - multi-threaded [libthread.so.1, _thr_sigacthandler (0x122f0)] \
   Note that this sequence is exactly the same found in the tracing     \
   version of the Threads Library, libthreadT.so.1.                     \
12693:       8b 4c 24(44)    mov    0x44(%esp),%ecx                     \
12697:       75 0a           jne    126a3 <_thr_sigacthandler+0x3b3>    \
12699:       c7 85 4c 02 00 00 00    movl   $0x0,0x24c(%ebp)            \
126a0:       00 00 00                                                   \
126a3:       52              push   %edx                                \
126a4:       51              push   %ecx                                \
126a5:       57              push   %edi                                \
126a6:       ff 54 24 44     call   *0x44(%esp)                         \
126aa:       83 c4 0c        add    $0xc,%esp  <-- PC                   \
126ad:      (eb)5b           jmp    1270a <_thr_sigacthandler+0x41a> */ \
         (*(unsigned long *)(pc_ - 20) == 0xc70a7544                    \
       && *(unsigned long *)(pc_ - 16) == 0x00024c85                    \
       && *(unsigned long *)(pc_ - 12) == 0x00000000                    \
       && *(unsigned long *)(pc_ - 8)  == 0x57515200                    \
       && *(unsigned long *)(pc_ - 4)  == 0x442454ff                    \
       && *(unsigned long *)(pc_)      == 0xeb0cc483))                  \
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
  new_cfa_ = mctx_->gregs[R_ESP];                                       \
  (FS)->cfa_how = CFA_REG_OFFSET;                                       \
  (FS)->cfa_reg = 4;                                                    \
  (FS)->cfa_offset = new_cfa_ - (long) (CONTEXT)->cfa;                  \
                                                                        \
  /* The SVR4 register numbering macros aren't usable in libgcc.  */    \
  (FS)->regs.reg[0].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[0].loc.offset = (long)&mctx_->gregs[R_EAX] - new_cfa_; \
  (FS)->regs.reg[3].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[3].loc.offset = (long)&mctx_->gregs[R_EBX] - new_cfa_; \
  (FS)->regs.reg[1].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[1].loc.offset = (long)&mctx_->gregs[R_ECX] - new_cfa_; \
  (FS)->regs.reg[2].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[2].loc.offset = (long)&mctx_->gregs[R_EDX] - new_cfa_; \
  (FS)->regs.reg[6].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[6].loc.offset = (long)&mctx_->gregs[R_ESI] - new_cfa_; \
  (FS)->regs.reg[7].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[7].loc.offset = (long)&mctx_->gregs[R_EDI] - new_cfa_; \
  (FS)->regs.reg[5].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[5].loc.offset = (long)&mctx_->gregs[R_EBP] - new_cfa_; \
  (FS)->regs.reg[8].how = REG_SAVED_OFFSET;                             \
  (FS)->regs.reg[8].loc.offset = (long)&mctx_->gregs[R_EIP] - new_cfa_; \
  (FS)->retaddr_column = 8;                                             \
  goto SUCCESS;                                                         \
} while (0)
#endif /* IN_LIBGCC2 */
