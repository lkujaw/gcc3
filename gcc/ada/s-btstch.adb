------------------------------------------------------------------------------
--                                                                          --
--                 GNAT RUN-TIME LIBRARY (GNARL) COMPONENTS                 --
--                                                                          --
--     S Y S T E M . B B . T H R E A D S . S T A C K _ C H E C K I N G      --
--                                                                          --
--                                  B o d y                                 --
--                                                                          --
--               Copyright (C) 2004 The European Space Agency               --
--                     Copyright (C) 2004-2005, AdaCore                     --
--                                                                          --
-- GNARL is free software; you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 2,  or (at your option) any later ver- --
-- sion. GNARL is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License --
-- for  more details.  You should have  received  a copy of the GNU General --
-- Public License  distributed with GNARL; see file COPYING.  If not, write --
-- to  the Free Software Foundation,  59 Temple Place - Suite 330,  Boston, --
-- MA 02111-1307, USA.                                                      --
--                                                                          --
--
--
--
--
--
--
--
-- GNARL was developed by the GNARL team at Florida State University.       --
-- Extensive contributions were provided by Ada Core Technologies, Inc.     --
--                                                                          --
-- The porting of GNARL to bare board  targets was initially  developed  by --
-- the Real-Time Systems Group at the Technical University of Madrid.       --
--                                                                          --
------------------------------------------------------------------------------

pragma Restrictions (No_Elaboration_Code);
--  We want to guarantee the absence of elaboration code because the
--  binder does not handle references to this package.

with System.Parameters;
--  Used for Stack_Grows_Down

with System.BB.Interrupts;
--  Used for Within_Interrupt_Stack

package body System.BB.Threads.Stack_Checking is

   procedure Stack_Check (Stack_Address : System.Address) is
      Self : constant Thread_Id := Thread_Self;
   begin
      if ((System.Parameters.Stack_Grows_Down
           and then Stack_Address < Self.Bottom_Of_Stack)
          or else
          (not System.Parameters.Stack_Grows_Down
           and then Stack_Address > Self.Top_Of_Stack))
          and then not Interrupts.Within_Interrupt_Stack (Stack_Address)
      then
         raise Storage_Error;
      end if;
   end Stack_Check;

end System.BB.Threads.Stack_Checking;
