/* Copyright (C) 2004 Free Software Foundation, Inc.
   Contributed by Douglas B Rupp <rupp@gnat.com>

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

/* As a special exception, if you link this library with other files,
   some of which are compiled with GCC, to produce an executable,
   this library does not by itself cause the resulting executable
   to be covered by the GNU General Public License.
   This exception does not however invalidate any other reasons why
   the executable file might be covered by the GNU General Public License.  */

/* Locate the FDE entry for a given address, using VMS Starlet routines
   to avoid register/deregister calls at DSO load/unload.  */

#include "config.h"
#include <stddef.h>
#include <stdlib.h>
#include "unwind-ia64.h"

#define UNW_IVMS_MODE(x)	(((x) >> 44) & 0x3L)

/* Return a pointer to the unwind table entry for the function
   containing PC.  */

struct vms_unw_table_entry
{
  unsigned long start_offset;
  unsigned long end_offset;
  unsigned long info_offset;
  unsigned long gp_value;
};

struct unw_table_entry *
_Unwind_FindTableEntry (void *pc, unsigned long *segment_base,
                        unsigned long *gp, struct unw_table_entry *ent)
{
  struct vms_unw_table_entry vueblock;
  unsigned long *unw, header, length;

  SYS$GET_UNWIND_ENTRY_INFO (pc, &vueblock, 0);
  *segment_base = 0; /* ??? Fixme. ??? */
  *gp = vueblock.gp_value;
  ent->start_offset = vueblock.start_offset;
  ent->end_offset = vueblock.end_offset;
  ent->info_offset = vueblock.info_offset;

  unw = (unsigned long *) (ent->info_offset + *segment_base);
  header = *unw;

  if (UNW_IVMS_MODE (header))
    return 0;

  return ent;
}
