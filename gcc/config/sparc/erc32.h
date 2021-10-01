/* Definitions for an ERC32 machine running in a bare board
   configuration using the ELF object format.
   Copyright (C) 2004-2005 Free Software Foundation, Inc.

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

/* Use the required linker script file */

#undef LINK_SPEC
#define LINK_SPEC "-T linker_script.ld%s"

/* Make the C library available */

#undef LIB_SPEC
#define LIB_SPEC "-lerc32 %{!g:-lc} %{g:-lg}"

/* This target does not need support for either table based unwinding or
   C++ constructors/destructors, so that there is no need for the
   crtbegin/crtend and crti/crtn machinery.
 */

#undef STARTFILE_SPEC
#define STARTFILE_SPEC "crt0.o%s"

#undef ENDFILE_SPEC
#define ENDFILE_SPEC ""

/* This platform supports software stack checking, and we will reserve
   75 words of space for either propagating the exception (sjlj) or
   executing a possible last chance handler. */

#undef STACK_CHECK_PROTECT
#define STACK_CHECK_PROTECT 300

/* This target does not have MMU so that nothing needs to be done in
   order to enable the execution of code on the stack. */

#undef ENABLE_EXECUTE_STACK 

/* Force the use of dwarf-2 by default */

#undef PREFERRED_DEBUGGING_TYPE
#define PREFERRED_DEBUGGING_TYPE DWARF2_DEBUG
