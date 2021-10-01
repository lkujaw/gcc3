/* Fallback frame unwinding for mips-irix6.x.
   Copyright (C) 2004 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   In addition to the permissions in the GNU General Public License, the
   Free Software Foundation gives you unlimited permission to link the
   compiled version of this file into combinations with other programs,
   and to distribute those combinations without any restriction coming
   from the use of this file.  (The General Public License restrictions
   do apply in other respects; for example, they cover modification of
   the file, and distribution when not linked into a combined
   executable.)

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

/* This file is #included into unwind-dw2.c and so has access to type
   definitions only visible there.  It provides a "functionified" version of
   the MD_FALLBACK_FRAME_STATE_FOR macro.  */

#include <signal.h>

static sigcontext_t * sigcontext_for (void * ra, void * cfa);

static int md_fallback_frame_state_for 
(struct _Unwind_Context *CONTEXT, _Unwind_FrameState *FS);

#define UINT_AT(ADDR) (*(unsigned int *)(ADDR))

/* Look at the code around RA to see if it matches a sighandler caller with a
   sigcontext_t * argument (SA_SIGINFO cleared).  Return that pointer argument
   if it does match, or 0 otherwise.  */

static sigcontext_t *
sigcontext_for (void * ra, void * cfa)
{
  /* IRIX 6.5, mono-threaded application.  */
  if (UINT_AT (ra - 40) == 0x00e0c825
      && UINT_AT (ra - 36) == 0x658cd38c
      && UINT_AT (ra - 32) == 0x019f602d
      && UINT_AT (ra - 28) == 0x0300f825
      && UINT_AT (ra - 24) == 0x8d8c9744
      && UINT_AT (ra - 20) == 0x8d8c0000
      && UINT_AT (ra - 16) == 0x8d8d0000
      && UINT_AT (ra - 12) == 0xffac0008
      && UINT_AT (ra - 8)  == 0x0320f809
      && UINT_AT (ra - 4)  == 0xffad0010)
    return (sigcontext_t *)(cfa + 0x30);

  /* IRIX 6.5, multi-threaded application, pthreads.  */
  if (UINT_AT (ra - 40) == 0xffb00000
      && UINT_AT (ra - 36) == 0x0004f880
      && UINT_AT (ra - 32) == 0x27399058
      && UINT_AT (ra - 28) == 0x8c300edc
      && UINT_AT (ra - 24) == 0x033fc821
      && UINT_AT (ra - 20) == 0x8f390000
      && UINT_AT (ra - 16) == 0xdc210e70
      && UINT_AT (ra - 12) == 0xde120058
      && UINT_AT (ra - 8)  == 0x0320f809
      && UINT_AT (ra - 4)  == 0xfe010058)
    return (sigcontext_t *)(cfa + 0x60);

  return 0;
}

#define SIGCTX_GREG_ADDR(REGNO,SIGCTX) \
  ((void *) &(SIGCTX)->sc_regs[REGNO])

#define SIGCTX_FPREG_ADDR(REGNO,SIGCTX) \
  ((void *) &(SIGCTX)->sc_fpregs[REGNO])

/* See if CONTEXT identifies some signal handler frame ; fill out FS
   accordingly and return 1 if so. Return 0 otherwise.  */

static int
md_fallback_frame_state_for 
(struct _Unwind_Context *CONTEXT, _Unwind_FrameState *FS)
{
  char * eh_debug_env = getenv ("EH_DEBUG");
  int  eh_debug = eh_debug_env ? atoi (eh_debug_env) : 0;

  /* Return address and CFA of the frame we're attempting to unwind through,
     possibly a signal handler.  */
  void * ctx_ra  = (void *)((CONTEXT)->ra);
  void * ctx_cfa = (void *)((CONTEXT)->cfa);

  /* CFA of the intermediate abstract kernel frame between the interrupted
     code and the signal handler, if we're indeed unwinding through a signal
     handler.  */
  void * k_cfa;

  /* Pointer to the sigcontext_t structure pushed by the kernel when we're
     unwinding through a signal handler setup with SA_SIGINFO cleared.  */
  sigcontext_t * sigctx;
  int i;

  if (eh_debug)
    printf ("FALLBACK called for CFA = 0x%p, RA = 0x%p\n", ctx_cfa, ctx_ra);

  sigctx = sigcontext_for (ctx_ra, ctx_cfa);

  if (sigctx == 0)
    return 0;

  /* The abstract kernel frame's CFA is extactly the stack pointer
     value at the interruption point.  */
  k_cfa = *(void **)SIGCTX_GREG_ADDR (CTX_SP, sigctx);

  if (eh_debug)
    printf ("Match for K_CFA = 0x%p, SIGCTX @ 0x%p\n", k_cfa, sigctx);

  /* State the rules to compute the CFA we have the value of: use the
     previous CFA and offset by the difference between the two.  See
     uw_update_context_1 for the supporting details.  */

  (FS)->cfa_how = CFA_REG_OFFSET;
  (FS)->cfa_reg = __builtin_dwarf_sp_column ();
  (FS)->cfa_offset = k_cfa - ctx_cfa;

  /* Fill the internal frame_state structure with information stating where
     each register of interest can be found from the CFA.  */
  
  for (i = 0; i <= 31; i ++)
    {
      (FS)->regs.reg[i].how = REG_SAVED_OFFSET;
      (FS)->regs.reg[i].loc.offset = SIGCTX_GREG_ADDR (i, sigctx) - k_cfa;
    }

  for (i = 0; i <= 31; i ++)
    {
      (FS)->regs.reg[32+i].how = REG_SAVED_OFFSET;
      (FS)->regs.reg[32+i].loc.offset = SIGCTX_FPREG_ADDR (i, sigctx) - k_cfa;
    }

  /* State the rules to find the kernel's code "return address", which is the
     address of the active instruction when the signal was caught.  Cheat two
     call clobbered register slots (that we don't care restoring properly) to
     account for the unwinder's adjustments caused by it expecting to have a
     call return address at hand.  */

  sigctx->sc_regs [CTX_T0] = sigctx->sc_pc + 4;

  (FS)->retaddr_column = CTX_T1;
  (FS)->regs.reg[CTX_T1].how = REG_SAVED_OFFSET;
  (FS)->regs.reg[CTX_T1].loc.offset
    = SIGCTX_GREG_ADDR (CTX_T0, sigctx) - k_cfa;
   
  return 1;
}

