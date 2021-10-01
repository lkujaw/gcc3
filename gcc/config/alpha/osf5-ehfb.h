/* This has been put in a separate file to avoid having to recompile the
   world after each modification.  */
   
/* Do code reading to identify a signal frame and fill frame state data
   to unwind past it.  See unwind-dw2.c for the structs.  */

#ifdef IN_LIBGCC2

/* This file implements the MD_FALLBACK_FRAME_STATE_FOR macro, triggered when
   the GCC table based unwinding process hits a frame for which no unwind info
   has been registered. This typically occurs when raising an exception from a
   signal handler, because the handler is actually called from the OS kernel.

   The basic idea is to detect that we are indeed trying to unwind past a
   signal handler and to fill out the GCC internal unwinding structures for
   the OS kernel frame as if it had been directly called from the interrupted
   context.

   This is all assuming that the code to set the handler asked the kernel to
   pass a pointer to such context information.  */

/* --------------------------------------------------------------------------
   -- Basic principles of operation:
   --------------------------------------------------------------------------

   1/ We first need a way to detect if we are trying to unwind past a signal
      handler.

   The typical method that is used on most platforms is to look at the code
   around the return address we have and check if it matches the OS code
   calling a handler.  To determine what this code is expected to be, get a
   breakpoint into a real signal handler and look at the code around the
   return address.  Depending on the library versions the pattern of the
   signal handler is different; this is the reason why we check against more
   than one pattern.

   On this target, the return address is right after the call and every
   instruction is 4 bytes long.  For the simple case of a null dereference in
   a single-threaded app, it went like:

   # Check that we indeed have something we expect: the instruction right
   # before the return address is within a __sigtramp function and is a call.

   [... run gdb and break at the signal handler entry ...]

   (gdb) x /i $ra-4
   <__sigtramp+160>: jsr     ra,(a3),0x3ff800d0ed4 <_fpdata+36468>

   # Look at the code around that return address, and eventually observe a
   # significantly large chunk of *constant* code right before the call:

   (gdb) x /10i  $ra-44
   <__sigtramp+120>: lda     gp,-27988(gp)
   <__sigtramp+124>: ldq     at,-18968(gp)
   <__sigtramp+128>: lda     t0,-1
   <__sigtramp+132>: stq     t0,0(at)
   <__sigtramp+136>: ldq     at,-18960(gp)
   <__sigtramp+140>: ldl     t1,8(at)
   <__sigtramp+144>: ldq     at,-18960(gp)
   <__sigtramp+148>: stl     t1,12(at)
   <__sigtramp+152>: ldq     at,-18960(gp)
   <__sigtramp+156>: stl     t0,8(at)
   
   # The hexadecimal equivalent that we will have to match is:

   (gdb) x /10x  $ra-44
   <__sigtramp+120>: 0x23bd92ac    0xa79db5e8    0x203fffff   0xb43c0000
   <__sigtramp+136>: 0xa79db5f0    0xa05c0008    0xa79db5f0   0xb05c000c
   <__sigtramp+152>: 0xa79db5f0    0xb03c0008
   
   The problem observed on this target with this approach is that although
   we found a constant set of instruction patterns there were some
   gp-related offsets that made the machine code to differ from one
   installation to another.  This problem could have been overcome by masking
   these offsets, but we found that it would be simpler and more efficient to
   check whether the return address was part of a signal handler, by comparing
   it against some expected code offset from __sigtramp.

   # Check that we indeed have something we expect: the instruction
   # right before the return address is within a __sigtramp
   # function and is a call. We also need to obtain the offset
   # between the return address and the start address of __sigtramp.

   [... run gdb and break at the signal handler entry ...]

   (gdb) x /2i $ra-4
   <__sigtramp+160>: jsr     ra,(a3),0x3ff800d0ed4 <_fpdata+36468>
   <__sigtramp+164>: ldah    gp,16381(ra)

   (gdb) p (long)$ra - (long)&__sigtramp
   $2 = 164

   --------------------------------------------------------------------------

   2/ Once we know we are going through a signal handler, we need a way to
      retrieve information about the interrupted run-time context.

   On this platform, the third handler's argument is a pointer to a structure
   describing this context (struct sigcontext *). We unfortunately have no
   direct way to transfer this value here, so a couple of tricks are required
   to compute it.

   As documented at least in some header files (e.g. sys/machine/context.h),
   the structure the handler gets a pointer to is located on the stack.  As of
   today, while writing this macro, we have unfortunately not been able to
   find a detailed description of the full stack layout at handler entry time,
   so we'll have to resort to empirism :)

   When unwinding here, we have the handler's CFA at hand, as part of the
   current unwinding context which is one of our arguments.  We presume that
   for each call to a signal handler by the same kernel routine, the context's
   structure location on the stack is always at the same offset from the
   handler's CFA, and we compute that offset from bare observation:
   
   For the simple case of a bare null dereference in a single-threaded app,
   computing the offset was done using GNAT like this:

   # Break on the first handler's instruction, before the prologue to have the
   # CFA in $sp, and get there:

   (gdb) b *&__gnat_error_handler
   Breakpoint 1 at 0x120016090: file init.c, line 378.

   (gdb) r
   Program received signal SIGSEGV, Segmentation fault.

   (gdb) c
   Breakpoint 1, __gnat_error_handler (sig=..., sip=..., context=...)

   # The displayed argument value are meaningless because we stopped before
   # their final "homing". We know they are passed through $a0, $a1 and $a2
   # from the ABI, though, so ...

   # Observe that $sp and the context pointer are in the same (stack) area,
   # and compute the offset:

   (gdb) p /x $sp
   $2 = 0x11fffbc80

   (gdb) p /x $a2
   $3 = 0x11fffbcf8

   (gdb) p /x (long)$a2 - (long)$sp
   $4 = 0x78
   
   --------------------------------------------------------------------------

   3/ Once we know we are unwinding through a signal handler and have the
      address of the structure describing the interrupted context at hand, we
      have to fill the internal frame-state/unwind-context structures properly
      to allow the unwinding process to proceed.

   Roughly, we are provided with an *unwinding* CONTEXT, describing the state
   of some point P in the call chain we are unwinding through.  The macro we
   implement has to fill a "frame state" structure FS that describe the P's
   caller state, by way of *rules* to compute its CFA, return address, and
   **saved** registers *locations*. 

   For the case we are going to deal with, the caller is some kernel code
   calling a signal handler, and:

   o The saved registers are all in the interrupted run-time context,

   o The CFA is the stack pointer value when the kernel code is entered, that
     is, the stack pointer value at the interruption point, also part of the
     interrupted run-time context.

   o We want the return address to appear as the address of the active
     instruction at the interruption point, so that the unwinder proceeds as
     if the interruption had been a regular call.  This address is also part
     of the interrupted run-time context.

   --

   Despite the apparent simplicity of the rules above, a trick is played to
   deal with the kernel frame "return address", because what we typically get
   in a signal context is not really a return address but some of functions in
   the general unwinder think it is.

   The basic issue is that...

   o The generic unwinding engine is expecting to deal with a call return
     address, because that is the nominal case. It is however actually
     interested in what region the call itself pertains to, so it substracts 1
     to the frame_state's "return address" and uses that to search the unwind
     table (see e.g. uw_frame_state_for).
   
   o The address we get for a signal context is not a return address but the
     address of a faulting instruction, which we want to use *untouched* to
     search the tables.

   o There is currently no provision in the generic unwinder to allow
     differentiating the two cases.

   What we do here is we cheat by adjusting the faulting address *value* by 1
   at the place where it is saved in the sigcontext structure to compensate.
   Note that we must account for the fact that we may be called more than once
   for the same context and ensure the adjustment remains constant. We exploit
   the fact that instruction address are normally always multiple of 4 here,
   and only adjust if it (still) the case.

   Something that needs to be taken into account is that floating point traps
   are imprecise in the Alpha architecture.  Hence, software assistance
   is needed for determining the exact location that caused the floating point
   trap, so that the return address that is stored in the sigcontext structure
   is *exactly* the faulting address.  It can be achieved by using the
   "-mtrap-precision=i" GCC command option, so that the trap handler can
   determine the exact instruction that caused the floating point exception,
   and then the unwinding mechanism works appropriately.  Not specifying this
   command option results in the unwinder not using the address of the
   instruction that triggered the trap but the one where the trap was
   delivered, that can be placed an arbitrary number of instructions after the
   trigger instruction, so that we may indeed unwind to the wrong place. Note
   that, in the case of programs that may cause floating point exceptions, it
   could be more efficient to use the "-mieee" GCC command option so that
   the generated code is able to correctly support denormalized numbers and
   exceptional IEEE values without generating traps.

   --

   Also, note that there is an important difference between the return address
   we need to claim for the kernel frame and the value of the return address
   register at the interruption point.

   The latter might be required to be able to unwind past the interrupted
   routine, for instance if it is interrupted before saving the incoming
   register value in its own frame, which may typically happen during stack
   probes for stack-checking purposes.

   It is then essential that the rules stated to locate the kernel frame
   return address don't clobber the rules describing where is saved the return
   address register at the interruption point, so some scratch register state
   entry should be used for the former. We have DWARF_ALT_FRAME_RETURN_COLUMN
   at hand exactly for that purpose.

   --------------------------------------------------------------------------

   4/ Depending on the context (single-threaded or multi-threaded app, ...),
   the code calling the handler and the handler-cfa to interrupted-context
   offset might change, so we use a simple generic data structure to track
   the possible variants.
*/

/* This is the structure to wrap information about each possible sighandler
   caller we may have to identify.  */

typedef struct {
  void * ra_value;
  /* Expected return address when being called from a sighandler */
  
  int cfa_to_context_offset;
  /* Offset to get to the sigcontext structure from the handler's CFA
     when the pattern matches.  */

} sighandler_call_t;

/* Helper macro for MD_FALLBACK_FRAME_STATE_FOR below.

   Look at RA to see if it matches within a sighandler caller.
   Set SIGCTX to the corresponding sigcontext structure (computed from
   CFA) if it does, or to 0 otherwise.  */

#define COMPUTE_SIGCONTEXT_FOR(RA,CFA,SIGCTX)				    \
do {									    \
  /* Define and register the applicable patterns.  */			    \
  extern void __sigtramp (void);					    \
									    \
  sighandler_call_t sighandler_calls [] = {				    \
    {__sigtramp + 164, 0x78}						    \
  };									    \
									    \
  int n_patterns_to_match						    \
    = sizeof (sighandler_calls) / sizeof (sighandler_call_t);		    \
									    \
  int pn;  /* pattern number  */					    \
									    \
  int match = 0;  /* Did last pattern match ?  */			    \
									    \
  /* Try to match each pattern in turn.  */				    \
  for (pn = 0; !match && pn < n_patterns_to_match; pn ++)		    \
    match = ((RA) == sighandler_calls[pn].ra_value);			    \
									    \
  (SIGCTX) = (struct sigcontext *)					    \
    (match ? ((CFA) + sighandler_calls[pn - 1].cfa_to_context_offset) : 0); \
} while (0);

#include <sys/context_t.h>

#define REG_SP  30  /* hard reg for stack pointer */
#define REG_RA  26  /* hard reg for return address */

static int
md_fallback_frame_state_for 
(struct _Unwind_Context *CONTEXT, _Unwind_FrameState *FS)
{
  char * eh_debug_env = getenv ("EH_DEBUG");
  int  eh_debug = eh_debug_env ? atoi (eh_debug_env) : 0;

  void * ctx_ra  = (void *)((CONTEXT)->ra);
  void * ctx_cfa = (void *)((CONTEXT)->cfa);

  struct sigcontext * sigctx;

  if (eh_debug)
    printf ("FALLBACK called for CFA = 0x%p, RA = 0x%p\n", ctx_cfa, ctx_ra);

  COMPUTE_SIGCONTEXT_FOR (ctx_ra, ctx_cfa, sigctx);

  if (!sigctx)
    return 0;
  else
    {
      int i;

      /* The kernel frame's CFA is extactly the stack pointer value at the
	 interruption point.  */
      void * k_cfa
	= (void *) sigctx->sc_regs [REG_SP];

      if (eh_debug)
        printf ("Match for K_CFA = 0x%p, SIGCTX @ 0x%p\n", k_cfa, sigctx);

      /* State the rules to compute the CFA we have the value of: use the
         previous CFA and offset by the difference between the two.  See
	 uw_update_context_1 for the supporting details.  */
      (FS)->cfa_how = CFA_REG_OFFSET;
      (FS)->cfa_reg = __builtin_dwarf_sp_column ();
      (FS)->cfa_offset = k_cfa - ctx_cfa;

      /* Fill the internal frame_state structure with information stating
	 where each register of interest in the saved context can be found
	 from the CFA.  */

      /* The general registers are in sigctx->sc_regs.  Leave out r31, which
         is read-as-zero. It makes no sense restoring it, and we are going to
	 use the state entry for the kernel return address rule below.

         This loop must cover at least all the callee-saved registers, and
	 we just don't bother specializing the set here.  */
      for (i = 0; i <= 30; i ++)
	{
	  (FS)->regs.reg[i].how = REG_SAVED_OFFSET;
	  (FS)->regs.reg[i].loc.offset
	    = (void *) &sigctx->sc_regs[i] - (void *) k_cfa;
	}

      /* Ditto for the floating point registers in sigctx->sc_fpregs.  */
      for (i = 0; i <= 31; i ++)
	{
	  (FS)->regs.reg[32+i].how = REG_SAVED_OFFSET;
	  (FS)->regs.reg[32+i].loc.offset
	    = (void *) &sigctx->sc_fpregs[i] - (void *) k_cfa;
	}

      /* State the rules to find the kernel's code "return address", which
	 is the address of the active instruction when the signal was caught,
	 in sigctx->sc_pc. Use DWARF_ALT_FRAME_RETURN_COLUMN since the return
	 address register is a general register and should be left alone.  */
      (FS)->retaddr_column = DWARF_ALT_FRAME_RETURN_COLUMN;
      (FS)->regs.reg[DWARF_ALT_FRAME_RETURN_COLUMN].how = REG_SAVED_OFFSET;
      (FS)->regs.reg[DWARF_ALT_FRAME_RETURN_COLUMN].loc.offset
	= (void *) &sigctx->sc_pc - (void *) k_cfa;

      /* Trick its value to compensate for later adjustments from the generic
	 unwinding circuitry, which thinks it is a real return address. We
	 may be called more than once for the same context, so take care not
	 to perform the adjustment multiple times. We exploit the fact that
	 instruction addresses are normally always multiple of 4 here. */
      if ((sigctx->sc_pc & 0x3) == 0)
        sigctx->sc_pc +=1;

      return 1;
    }
}

#endif

