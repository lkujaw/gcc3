/* Output variables, constants and external declarations, for GNU compiler.
   Copyright (C) 2005
   Free Software Foundation, Inc.

This file is part of GCC.

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

#include <vms/libicb.h>
#include <vms/chfdef.h>
#include <vms/chfctxdef.h>

#define __int64 long long
#include <vms/intstkdef.h>

#include <stdio.h>

#define UNW_SUCCESS 1
#define UNW_FAILURE 0

#define DYN$C_SSENTRY 66
/* ??? would rather get the proper header file.  */

extern INVO_CONTEXT_BLK * LIB$I64_CREATE_INVO_CONTEXT (void);

extern int LIB$I64_IS_EXC_DISPATCH_FRAME (void *);
extern int LIB$I64_IS_AST_DISPATCH_FRAME (void *);

extern int LIB$I64_INIT_INVO_CONTEXT (INVO_CONTEXT_BLK *, int, int);
extern int LIB$I64_GET_CURR_INVO_CONTEXT (INVO_CONTEXT_BLK *);
extern int LIB$I64_GET_PREV_INVO_CONTEXT (INVO_CONTEXT_BLK *);

typedef unsigned long uw_reg;
typedef uw_reg * uw_loc;

typedef char fp_reg[16];

#define DENOTES_VMS_DISPATCHER_FRAME(icb) \
(LIB$I64_IS_EXC_DISPATCH_FRAME (&(icb)->libicb$ih_pc)      \
 || LIB$I64_IS_AST_DISPATCH_FRAME (&(icb)->libicb$ih_pc))

#define DENOTES_BOTTOM_OF_STACK(icb) ((icb)->libicb$v_bottom_of_stack)

#define FAIL_IF(COND) do { if (COND) return UNW_FAILURE; } while (0);

int
md_fallback_frame_state_for
(struct _Unwind_Context *context, _Unwind_FrameState *fs)
{
  int i, status;

  INVO_CONTEXT_BLK local_icb;
  INVO_CONTEXT_BLK *icb = &local_icb;
    
  CHFCTX * chfctx;
  CHF$MECH_ARRAY * chfmech;
  CHF64$SIGNAL_ARRAY *chfsig64;
  INTSTK * intstk;

  FAIL_IF (icb == 0);

  /* Step 1 :
     ------------------------------------------------------------------------
     Unwind up to the VMS dispatcher frame, fetch pointers to useful
     datastructures from there, then unwind one step further up to the
     interrupted user context from which some required values will be
     easily accessible.
     ------------------------------------------------------------------------
  */

  status = LIB$I64_INIT_INVO_CONTEXT (icb, LIBICB$K_INVO_CONTEXT_VERSION, 0);
  FAIL_IF (status == 0);

  status = LIB$I64_GET_CURR_INVO_CONTEXT (icb);

  while (! DENOTES_VMS_DISPATCHER_FRAME (icb))
    {
      status = LIB$I64_GET_PREV_INVO_CONTEXT (icb);
      FAIL_IF (status == 0 || DENOTES_BOTTOM_OF_STACK (icb));
    }

  chfctx = icb->libicb$ph_chfctx_addr;
  FAIL_IF (chfctx == 0);
  
  chfmech = (CHF$MECH_ARRAY *)chfctx->chfctx$q_mcharglst;
  FAIL_IF (chfmech == 0);

  chfsig64 = (CHF64$SIGNAL_ARRAY *)chfmech->chf$ph_mch_sig64_addr;
  FAIL_IF (chfsig64 == 0);
 
  intstk = (INTSTK *)chfmech->chf$q_mch_esf_addr;
  FAIL_IF (intstk == 0 || intstk->intstk$b_subtype == DYN$C_SSENTRY);

  status = LIB$I64_GET_PREV_INVO_CONTEXT (icb);
  FAIL_IF (status == 0);

  /* Step 2 :
     ------------------------------------------------------------------------
     Point the GCC context locations/values required for further unwinding at
     their corresponding locations/values in the datastructures at hand.
     ------------------------------------------------------------------------
  */

  /* Static General Register locations, including scratch registers in case
     the unwinder needs to refer to a value stored in one of them.  */
  {
    uw_reg * ctxregs = (uw_reg *)&intstk->intstk$q_regbase;

    for (i = 2; i <= 3; i++)
      context->ireg[i - 2].loc = (uw_loc)&ctxregs[i];
    for (i = 8; i <= 11; i++)
      context->ireg[i - 2].loc = (uw_loc)&ctxregs[i];
    for (i = 14; i <= 31; i++)
      context->ireg[i - 2].loc = (uw_loc)&ctxregs[i];
  }

  /* Static Floating Point Register locations, as available from the
     mechargs array, which happens to include all the to be preserved
     ones + others.  */
  {
    fp_reg * ctxregs;

    ctxregs = (fp_reg *)&chfmech->chf$fh_mch_savf2;
    for (i = 2; i <= 5 ; i++)
      context->fr_loc[i - 2] = (uw_loc)&ctxregs[i - 2];

    ctxregs = (fp_reg *)&chfmech->chf$fh_mch_savf12;
    for (i = 12; i <= 31 ; i++)
      context->fr_loc[i - 2] = (uw_loc)&ctxregs[i - 12];
  }

  /* Relevant application register locations.  */

  context->fpsr_loc = (uw_loc)&intstk->intstk$q_fpsr;
  context->lc_loc   = (uw_loc)&intstk->intstk$q_lc;
  context->unat_loc = (uw_loc)&intstk->intstk$q_unat;

  /* Branch register locations.  */
  
  {
    uw_reg * ctxregs = (uw_reg *)&intstk->intstk$q_b0;

    for (i = 0; i < 8; i++)
      context->br_loc[i] = (uw_loc)&ctxregs[i];
  }

  /* Necessary register values.  */

  /* ??? Still unclear if we need to account for possible flushes to
     an alternate backing store and how this would be handled.  */

  context->bsp = (uw_reg)intstk->intstk$q_bsp;
  fs->no_reg_stack_frame = 1;

  context->pr  = (uw_reg)intstk->intstk$q_preds;
  context->gp  = (uw_reg)intstk->intstk$q_gp;

  context->psp = (uw_reg)icb->libicb$ih_psp;

  /* Previous Frame State location.  What eventually ends up in pfs_loc is
     installed with ar.pfs = pfs_loc; br.ret; so setup to target intstk->q_ifs
     to have the interrupted context restored and not that of its caller if
     we happen to have a handler in the interrupted context itself.  */
  fs->curr.reg[UNW_REG_PFS].where = UNW_WHERE_SPREL;
  fs->curr.reg[UNW_REG_PFS].val
    = (uw_loc)((uw_reg)&intstk->intstk$q_ifs - (uw_reg)context->psp);
  fs->curr.reg[UNW_REG_PFS].when = -1;

  /* If we need to unwind further up, past the interrupted context, we need to
     hand out the interrupted context's pfs, still.  */
  context->signal_pfs_loc = (uw_loc) &intstk->intstk$q_pfs;

  /* Finally, rules for RP .  */
  {
    uw_reg * post_sigarray
      = (uw_reg *)chfsig64 + 1 + chfsig64->chf64$l_sig_args;

    uw_reg * ih_pc_loc = post_sigarray - 2;

    fs->curr.reg[UNW_REG_RP].where = UNW_WHERE_SPREL;
    fs->curr.reg[UNW_REG_RP].val
      = (uw_loc) ((uw_reg)ih_pc_loc - (uw_reg)context->psp);
    fs->curr.reg[UNW_REG_RP].when = -1;
  }

  return UNW_SUCCESS;
}
     
