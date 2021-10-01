/* This has been put in a separate file to avoid having to recompile the
   world after each modification.  */

/* Do code reading to identify a signal frame, and set the frame
   state data appropriately.  See unwind-dw2.c for the structs.  */

#include <ucontext.h>

/* Handle multilib correctly.  */
#if defined(__arch64__)

/* 64-bit Sparc version */
#define MD_FALLBACK_FRAME_STATE_FOR(context, fs, success);		\
do {									\
  void * pc_ = (context)->ra;						\
  void * this_cfa_ = (context)->cfa;					\
  void * new_cfa_;							\
  int regs_off_;							\
  int fpu_save_off_;							\
  void ** ra_location;							\
  unsigned int i_;							\
  uint8_t fpu_save_;							\
  									\
  /* This is the observed pattern for the sigacthandler in Solaris 8 */	\
  unsigned int sigacthandler_sol8_pattern []				\
    = {0x9401400f, 0xca5aafa0, 0x913e2000, 0x892a3003,			\
       0xe0590005, 0x9fc40000, 0x9410001a, 0x80a6e008};			\
									\
  /* This is the observed pattern for the sigacthandler in Solaris 9 */ \
  unsigned int sigacthandler_sol9_pattern []				\
    = {0xa33e2000, 0x00000000, 0x892c7003, 0x90100011,			\
       0xe0590005, 0x9fc40000, 0x9410001a, 0x80a46008};			\
									\
  /* This is the observed pattern for the __sighndlr */			\
  unsigned int sighndlr_pattern []					\
    = {0x9de3bf50, 0x90100018, 0x92100019, 0x9fc6c000,			\
       0x9410001a, 0x81c7e008, 0x81e80000};				\
									\
  /* Look for the sigacthandler pattern. The pattern changes slightly	\
     in different versions of the operating system, so we skip the	\
     comparison against pc_-(4*6) for Solaris 9 */			\
  if ((    *(unsigned int *)(pc_-(4*7)) == sigacthandler_sol8_pattern[0] \
        && *(unsigned int *)(pc_-(4*6)) == sigacthandler_sol8_pattern[1] \
        && *(unsigned int *)(pc_-(4*5)) == sigacthandler_sol8_pattern[2] \
        && *(unsigned int *)(pc_-(4*4)) == sigacthandler_sol8_pattern[3] \
        && *(unsigned int *)(pc_-(4*3)) == sigacthandler_sol8_pattern[4] \
        && *(unsigned int *)(pc_-(4*2)) == sigacthandler_sol8_pattern[5] \
        && *(unsigned int *)(pc_-(4*1)) == sigacthandler_sol8_pattern[6] \
        && *(unsigned int *)(pc_)       == sigacthandler_sol8_pattern[7] ) || \
      (    *(unsigned int *)(pc_-(4*7)) == sigacthandler_sol9_pattern[0] \
        /* skip pc_-(4*6) */						\
        && *(unsigned int *)(pc_-(4*5)) == sigacthandler_sol9_pattern[2] \
        && *(unsigned int *)(pc_-(4*4)) == sigacthandler_sol9_pattern[3] \
        && *(unsigned int *)(pc_-(4*3)) == sigacthandler_sol9_pattern[4] \
        && *(unsigned int *)(pc_-(4*2)) == sigacthandler_sol9_pattern[5] \
        && *(unsigned int *)(pc_-(4*1)) == sigacthandler_sol9_pattern[6] \
        && *(unsigned int *)(pc_)       == sigacthandler_sol9_pattern[7] ) ) \
    /* We need to move up two frames (the kernel frame and the handler	\
       frame). Minimum stack frame size is 176 bytes (128 + 48). 128	\
       bytes for spilling register window (16 extended words for in	\
       and local registers), and 6 extended words to store at least	\
       6 arguments to callees, The kernel frame and the sigacthandler	\
       both have this minimal stack. The ucontext_t structure is after	\
       this offset */							\
    regs_off_ = 176 + 176;						\
  /* Look for the __sighndlr pattern */					\
  else if (    *(unsigned int *)(pc_-(4*5)) == sighndlr_pattern[0]	\
            && *(unsigned int *)(pc_-(4*4)) == sighndlr_pattern[1]	\
            && *(unsigned int *)(pc_-(4*3)) == sighndlr_pattern[2]	\
            && *(unsigned int *)(pc_-(4*2)) == sighndlr_pattern[3]	\
            && *(unsigned int *)(pc_-(4*1)) == sighndlr_pattern[4]	\
            && *(unsigned int *)(pc_)       == sighndlr_pattern[5]	\
            && *(unsigned int *)(pc_+(4*1)) == sighndlr_pattern[6] )	\
    /* We have observed different calling frames among different	\
       versions of the operating system, so that we need to		\
       discriminate using the upper frame. We look for the return	\
       address of the caller frame (there is an offset of 15 double	\
       words between the frame address and the place where this return	\
       address is stored) in order to do some more pattern matching */	\
    if (*(unsigned int *)(*(unsigned long *)(this_cfa_ + 15*8) - 4) ==	\
        0x9410001a)							\
      /* In Solaris 9 we need to move up four frames (the kernel frame,	\
         the sigacthandler frame, the call_user_handler, and the	\
         __sighndlr frame).  The kernel frame has a stack frame size of	\
         176, the __sighndlr frames of 304 bytes, 176 bytes for the	\
         call_user_handler frame, and another 176 bytes for the		\
         sigacthandler frame. The ucontext_t structure is after this	\
         offset */							\
      regs_off_ = 176 + 304 + 176 + 176;				\
    else								\
      /* We need to move up three frames (the kernel frame, the		\
         sigacthandler frame, and the __sighndlr frame). The kernel	\
         frame has a stack frame size of 176, the __sighndlr frames of	\
         304 bytes, and there is a stack frame of 176 bytes for the	\
         sigacthandler frame. The ucontext_t structure is after this	\
         offset */							\
      regs_off_ = 176 + 304 + 176;					\
  else									\
    /* Exit if the pattern at the return address does not match the	\
       previous three patterns. */					\
    break;								\
									\
  /* FPU information can be extracted from the ucontext_t structure 	\
     that is the third argument for the signal handler, that is saved	\
     in the stack. There are 64 bytes between the beginning of the	\
     ucontext_t argument of the signal handler and the uc_mcontext	\
     field. There are 176 bytes between the beginning of uc_mcontext	\
     and the beginning of the fpregs field. */				\
  fpu_save_off_ = regs_off_ + (8 * 10) + 176;				\
									\
  /* The fpregs field contains 32 extended words at the beginning that	\
     contain the fpu state. Then there are 2 extended words and two	\
     bytes*/								\
  fpu_save_ =								\
    *(uint8_t *)(this_cfa_ + fpu_save_off_ + (8 * 32) + (2 * 8) + 2);	\
									\
  /* We need to get the frame pointer for the kernel frame that		\
     executes when the signal is raised. This frame is just the		\
     following to the application code that generated the signal, so	\
     that the later's stack pointer is the former's frame pointer. The	\
     stack pointer for the interrupted application code can be		\
     calculated from the ucontext_t structure (third argument for the	\
     signal handler) that is saved in the stack. There are 10 words	\
     between the beginning of the  ucontext_t argument  of the signal	\
     handler and the uc_mcontext.gregs field that contains the		\
     registers saved by the signal handler. */				\
  new_cfa_ = *(void **)(this_cfa_ + (regs_off_ + (8*10) + (REG_SP*8)));	\
  /* The frame address is %sp + STACK_BIAS in 64-bit mode. */		\
  new_cfa_ += 2047;							\
  (fs)->cfa_how = CFA_REG_OFFSET;					\
  (fs)->cfa_reg = __builtin_dwarf_sp_column ();				\
  (fs)->cfa_offset = new_cfa_ - this_cfa_;				\
									\
  /* Restore global and out registers (in this order) from the		\
     ucontext_t structure, uc_mcontext.gregs field. */			\
  for (i_ = 1; i_ < 16; ++i_)						\
    {									\
      /* We never restore %sp as everything is purely CFA-based. */	\
      if (i_ == __builtin_dwarf_sp_column ())				\
        continue;							\
      /* First the global registers and then the out registers */	\
      (fs)->regs.reg[i_].how = REG_SAVED_OFFSET;			\
      (fs)->regs.reg[i_].loc.offset =					\
        this_cfa_ + (regs_off_+ (8*10) + ((REG_Y+i_)*8)) - new_cfa_;	\
    }									\
									\
  /* Just above the stack pointer there are 16 extended words in which	\
     the register window (in and local registers) was saved. */		\
  for (i_ = 0; i_ < 16; ++i_)						\
    {									\
      (fs)->regs.reg[i_ + 16].how = REG_SAVED_OFFSET;			\
      (fs)->regs.reg[i_ + 16].loc.offset = i_ * 8;			\
    }									\
									\
  /* Check whether we need to restore fpu registers */			\
  if (fpu_save_)							\
    {									\
      for (i_ = 0; i_ < 64; ++i_)					\
        {								\
          if (i_ > 32 && (i_ & 0x1))					\
            continue;							\
          (fs)->regs.reg[i_ + 32].how = REG_SAVED_OFFSET;		\
          (fs)->regs.reg[i_ + 32].loc.offset =				\
            this_cfa_ + fpu_save_off_ + (i_ * 4) - new_cfa_;		\
        }								\
    }									\
									\
  ra_location = this_cfa_ + (regs_off_ + (8 * 10) + (REG_PC * 8));	\
  /* The dwarf2 unwind machinery is going to add 8 to the PC it uses on	\
     Sparc, so that we should adjust the return address by substracting	\
     8 to the stored PC. Moreover, some parts of the personality	\
     routine will substract 1 to that return address, so that is the	\
     reason why we substract 7 instead of 8. This function can be	\
     executed several times, so that we need to avoid adjusting the PC	\
     more than once (done by the if clause, taking into account that	\
     instructions are always 32-bits wide). */				\
  if (! ((int)(*ra_location) & 0x3))					\
    *ra_location -=7;							\
									\
  (fs)->regs.reg[0].how = REG_SAVED_OFFSET;				\
  (fs)->regs.reg[0].loc.offset =					\
    this_cfa_ + (regs_off_ + (8 * 10) + (REG_PC * 8)) - new_cfa_;	\
  (fs)->retaddr_column = 0;						\
  goto success;								\
} while (0)

#define MD_FROB_UPDATE_CONTEXT(context, fs)				\
do {									\
  /* The column of %sp contains the old CFA, not the old value of %sp.	\
     The CFA offset already comprises the stack bias so, when %sp is the\
     CFA register, we must avoid counting the stack bias twice. Do not	\
     do that for signal frames as the offset is artificial for them. */	\
  if ((fs)->cfa_reg == __builtin_dwarf_sp_column ()			\
      && (fs)->cfa_how == CFA_REG_OFFSET				\
      && (fs)->cfa_offset != 0						\
      && (fs)->retaddr_column != 0)					\
    (context)->cfa -= 2047;						\
} while (0)

#else

/* 32-bit Sparc version */
#define MD_FALLBACK_FRAME_STATE_FOR(context, fs, success);		\
do {									\
  void * pc_ = (context)->ra;						\
  void * this_cfa_ = (context)->cfa;					\
  void * new_cfa_;							\
  int regs_off_;							\
  int fpu_save_off_;							\
  void ** ra_location;							\
  unsigned int i_;							\
  uint8_t fpu_save_;							\
									\
  /* This is the observed pattern for the sigacthandler */		\
  unsigned int sigacthandler_pattern []					\
    = {0x9602400f, 0x92100019, 0x00000000, 0x912e2002,			\
       0xe002000a, 0x90100018, 0x9fc40000, 0x9410001a,			\
       0x80a62008};						\
									\
  /* This is the observed pattern for the __libthread_segvhdlr */	\
  unsigned int segvhdlr_pattern []					\
    = {0x94102000, 0xe007bfe4, 0x9010001c, 0x92100019,			\
       0x9fc40000, 0x9410001a, 0x81c7e008, 0x81e80000,			\
       0x80a26000};							\
									\
  /* This is the observed pattern for the __sighndlr */			\
  unsigned int sighndlr_pattern []					\
    = {0x9de3bfa0, 0x90100018, 0x92100019, 0x9fc6c000,			\
       0x9410001a, 0x81c7e008, 0x81e80000};				\
									\
  /* Look for the sigacthandler pattern. The pattern changes slightly	\
     in different versions of the operating system, so we skip the	\
     comparison against pc_-(4*6) */					\
  if (    *(unsigned int *)(pc_-(4*8)) == sigacthandler_pattern[0]	\
       && *(unsigned int *)(pc_-(4*7)) == sigacthandler_pattern[1]	\
       /* skip pc_-(4*6) */						\
       && *(unsigned int *)(pc_-(4*5)) == sigacthandler_pattern[3]	\
       && *(unsigned int *)(pc_-(4*4)) == sigacthandler_pattern[4]	\
       && *(unsigned int *)(pc_-(4*3)) == sigacthandler_pattern[5]	\
       && *(unsigned int *)(pc_-(4*2)) == sigacthandler_pattern[6]	\
       && *(unsigned int *)(pc_-(4*1)) == sigacthandler_pattern[7]	\
       && *(unsigned int *)(pc_)       == sigacthandler_pattern[8] )	\
    /* We need to move up two frames (the kernel frame and the handler	\
       frame). Minimum stack frame size is 96 bytes (64 + 4 + 24). 64	\
       bytes for spilling register window (16 words for in and local	\
       registers), 4 bytes for a pointer to space for callees		\
       returning structs, and 24 bytes to store at least six argument	\
       to callees. The ucontext_t structure is after this offset */	\
    regs_off_ = 96 + 96;						\
  /* Look for the __libthread_segvhdlr pattern */			\
  else if (    *(unsigned int *)(pc_-(4*6)) == segvhdlr_pattern[0]	\
            && *(unsigned int *)(pc_-(4*5)) == segvhdlr_pattern[1]	\
            && *(unsigned int *)(pc_-(4*4)) == segvhdlr_pattern[2]	\
            && *(unsigned int *)(pc_-(4*3)) == segvhdlr_pattern[3]	\
            && *(unsigned int *)(pc_-(4*2)) == segvhdlr_pattern[4]	\
            && *(unsigned int *)(pc_-(4*1)) == segvhdlr_pattern[5]	\
            && *(unsigned int *)(pc_)       == segvhdlr_pattern[6]	\
            && *(unsigned int *)(pc_+(4*1)) == segvhdlr_pattern[7]	\
            && *(unsigned int *)(pc_+(4*2)) == segvhdlr_pattern[8] )	\
    /* We need to move up four frames (the kernel frame, the		\
       sigacthandler frame, the __sighndlr frame, and the		\
       __libthread_segvhdlr). Two of them have the minimum		\
       stack frame size (kernel and __sighndlr frames) of 96 bytes,	\
       other has a stack frame of 216 bytes (the sigacthandler frame),	\
       and there is another with a stack frame of 128 bytes (the	\
       __libthread_segvhdlr). The ucontext_t structure is after this	\
       offset */							\
    regs_off_ = 96 + 96 + 128 + 216;					\
    /* Look for the __sighndlr pattern */				\
  else if (    *(unsigned int *)(pc_-(4*5)) == sighndlr_pattern[0]	\
            && *(unsigned int *)(pc_-(4*4)) == sighndlr_pattern[1]	\
            && *(unsigned int *)(pc_-(4*3)) == sighndlr_pattern[2]	\
            && *(unsigned int *)(pc_-(4*2)) == sighndlr_pattern[3]	\
            && *(unsigned int *)(pc_-(4*1)) == sighndlr_pattern[4]	\
            && *(unsigned int *)(pc_)       == sighndlr_pattern[5]	\
            && *(unsigned int *)(pc_+(4*1)) == sighndlr_pattern[6] )	\
    /* We have observed different calling frames among different	\
       versions of the operating system, so that we need to		\
       discriminate using the upper frame. We look for the return	\
       address of the caller frame (there is an offset of 15 words	\
       between the frame address and the place where this return	\
       address is stored) in order to do some more pattern matching */	\
    if (*(unsigned int *)(*(unsigned int *)(this_cfa_ + 15*4) - 4) ==	\
        0xd407a04c)							\
      /* This matches the call_user_handler pattern for Solaris 10. 	\
         We need to move up three frames (the kernel frame, the		\
         call_user_handler frame, the __sighndlr frame). Two of them	\
         have the minimum stack frame size (kernel and __sighndlr	\
         frames) of 96 bytes, and there is another with a stack frame	\
         of 160 bytes (the call_user_handler frame). The ucontext_t	\
         structure is after this offset */				\
      regs_off_ = 96 + 96 + 160;					\
    else if (*(unsigned int *)(*(unsigned int *)(this_cfa_ + 15*4) - 4) == \
             0x9410001a)						\
      /* This matches the call_user_handler pattern for Solaris 9.	\
         We need to move up four frames (the kernel frame, the signal	\
         frame, the call_user_handler frame, the __sighndlr frame).	\
         Three of them have the minimum stack frame size (kernel,	\
         signal, and __sighndlr frames) of 96 bytes, and there is	\
         another with a stack frame of 160 bytes (the call_user_handler	\
         frame). The ucontext_t structure is after this offset */	\
      regs_off_ = 96 + 96 + 96 + 160;					\
    else								\
      /* We need to move up three frames (the kernel frame, the		\
         sigacthandler frame, and the __sighndlr frame). Two of them	\
         have the minimum stack frame size (kernel and __sighndlr	\
         frames) of 96 bytes, and there is another with a stack frame	\
         of 216 bytes (the sigacthandler frame). The ucontext_t 	\
         structure is after this offset. */				\
      regs_off_ = 96 + 96 + 216;					\
  else									\
    /* Exit if the pattern at the return address does not match the	\
       previous three patterns. */					\
    break;								\
									\
  /* FPU information can be extracted from the ucontext_t structure	\
     that is the third argument for the signal handler, that is saved	\
     in the stack. There are 10 words between the beginning of the	\
     ucontext_t argument of the signal handler and the uc_mcontext	\
     field. There are 80 bytes between the beginning of uc_mcontext	\
     and the beginning of the fpregs field. */				\
  fpu_save_off_ = regs_off_ + (4 * 10) + (4 * 20);			\
									\
  /* The fpregs field contains 32 words at the beginning that contain	\
     the fpu state. Then there are 2 words and two bytes */		\
  fpu_save_ =								\
    *(uint8_t *)(this_cfa_ + fpu_save_off_ + (4 * 32) + (2 * 4) + 2);	\
									\
  /* We need to get the frame pointer for the kernel frame that		\
     executes when the signal is raised. This frame is just the		\
     following to the application code that generated the signal, so	\
     that the later's stack pointer is the former's frame pointer. The	\
     stack pointer for the interrupted application code can be		\
     calculated from the ucontext_t structure (third argument for the	\
     signal handler) that is saved in the stack. There are 10 words	\
     between the beginning of the  ucontext_t argument  of the signal	\
     handler and the uc_mcontext.gregs field that contains the		\
     registers saved by the signal handler. */				\
  new_cfa_ = *(void **)(this_cfa_ + (regs_off_ + (4*10) + (REG_SP*4)));	\
  (fs)->cfa_how = CFA_REG_OFFSET;					\
  (fs)->cfa_reg = __builtin_dwarf_sp_column ();				\
  (fs)->cfa_offset = new_cfa_ - this_cfa_;				\
									\
  /* Restore global and out registers (in this order) from the		\
     ucontext_t structure, uc_mcontext.gregs field. */			\
  for (i_ = 1; i_ < 16; ++i_)						\
    {									\
      /* We never restore %sp as everything is purely CFA-based. */	\
      if (i_ == __builtin_dwarf_sp_column ())				\
        continue;							\
      /* First the global registers and then the out registers */	\
      (fs)->regs.reg[i_].how = REG_SAVED_OFFSET;			\
      (fs)->regs.reg[i_].loc.offset =					\
        this_cfa_ + (regs_off_ + (4*10) + ((REG_Y+i_)*4)) - new_cfa_;	\
    }									\
									\
  /* Just above the stack pointer there are 16 words in which the	\
     register window (in and local registers) was saved. */		\
  for (i_ = 0; i_ < 16; ++i_)						\
    {									\
      (fs)->regs.reg[i_ + 16].how = REG_SAVED_OFFSET;			\
      (fs)->regs.reg[i_ + 16].loc.offset = i_*4;			\
    }									\
									\
  /* Check whether we need to restore fpu registers */			\
  if (fpu_save_)							\
    {									\
      for (i_ = 0; i_ < 32; ++i_)					\
        {								\
          (fs)->regs.reg[i_ + 32].how = REG_SAVED_OFFSET;		\
          (fs)->regs.reg[i_ + 32].loc.offset =				\
            this_cfa_ + fpu_save_off_ + (i_ * 4) - new_cfa_;		\
        }								\
    }									\
									\
  ra_location = this_cfa_ + (regs_off_ + (4 * 10) + (REG_PC * 4));	\
  /* The dwarf2 unwind machinery is going to add 8 to the PC it uses on	\
     Sparc, so that we should adjust the return address by substracting	\
     8 to the stored PC. Moreover, some parts of the personality	\
     routine will substract 1 to that return address, so that is the	\
     reason why we substract 7 instead of 8. This function can be	\
     executed several times, so that we need to avoid adjusting the PC	\
     more than once (done by the if clause, taking into account that	\
     instructions are always 32-bits wide). */				\
  if (! ((int)(*ra_location) & 0x3))					\
    *ra_location -=7;							\
									\
  (fs)->regs.reg[0].how = REG_SAVED_OFFSET;				\
  (fs)->regs.reg[0].loc.offset =					\
    this_cfa_ + (regs_off_ + (4 * 10) + (REG_PC * 4)) - new_cfa_;	\
  (fs)->retaddr_column = 0;						\
  goto success;								\
} while (0)

#endif
