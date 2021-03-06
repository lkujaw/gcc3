------------------------------------------------------------------------------
--                                                                          --
--                         GNAT RUN-TIME COMPONENTS                         --
--                                                                          --
--                 A D A . T A S K _ T E R M I N A T I O N                  --
--                                                                          --
--                                 B o d y                                  --
--                                                                          --
--         Copyright (C) 2005-2006, Free Software Foundation, Inc.          --
--                                                                          --
-- This specification is derived from the Ada Reference Manual for use with --
-- GNAT. The copyright notice above, and the license provisions that follow --
-- apply solely to the  contents of the part following the private keyword. --
--                                                                          --
-- GNAT is free software;  you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 2,  or (at your option) any later ver- --
-- sion.  GNAT is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License --
-- for  more details.  You should have  received  a copy of the GNU General --
-- Public License  distributed with GNAT;  see file COPYING.  If not, write --
-- to  the  Free Software Foundation,  51  Franklin  Street,  Fifth  Floor, --
-- Boston, MA 02110-1301, USA.                                              --
--                                                                          --
--
--
--
--
--
--
--
-- GNAT was originally developed  by the GNAT team at  New York University. --
-- Extensive contributions were provided by Ada Core Technologies Inc.      --
--                                                                          --
------------------------------------------------------------------------------

--  This is a simplified version of this package to be used in when the
--  Ravenscar profile and the No_Exception_Handlers restrictions are in
--  effect. It means that the only task termination cause that need to be
--  taken into account is normal task termination (abort is not allowed by
--  the Ravenscar profile and the restricted exception support does not
--  include Exception_Occurrence).

with System.Tasking;
--  used for Task_Id
--           Self
--           Fall_Back_Handler

with System.Task_Primitives.Operations;
--  Used for Self
--           Set_Priority
--           Get_Priority

with Unchecked_Conversion;

package body Ada.Task_Termination is

   use System.Task_Primitives.Operations;

   use type Ada.Task_Identification.Task_Id;

   -----------------------
   -- Local subprograms --
   -----------------------

   function To_TT is new Unchecked_Conversion
     (System.Tasking.Termination_Handler, Termination_Handler);

   function To_ST is new Unchecked_Conversion
     (Termination_Handler, System.Tasking.Termination_Handler);

   -----------------------------------
   -- Current_Task_Fallback_Handler --
   -----------------------------------

   function Current_Task_Fallback_Handler return Termination_Handler is
   begin
      --  There is no need for explicit protection against race conditions
      --  for this function because this function can only be executed by
      --  Self, and the Fall_Back_Handler can only be modified by Self.

      return To_TT (System.Tasking.Fall_Back_Handler);
   end Current_Task_Fallback_Handler;

   -------------------------------------
   -- Set_Dependents_Fallback_Handler --
   -------------------------------------

   procedure Set_Dependents_Fallback_Handler
     (Handler : Termination_Handler)
   is
      Self_Id         : constant System.Tasking.Task_Id := System.Tasking.Self;
      Caller_Priority : constant System.Any_Priority := Get_Priority (Self_Id);

   begin
      --  Raise the priority to prevent race conditions when modifying
      --  System.Tasking.Fall_Back_Handler.

      Set_Priority (Self_Id, System.Any_Priority'Last);

      System.Tasking.Fall_Back_Handler := To_ST (Handler);

      --  Restore the original priority

      Set_Priority (Self_Id, Caller_Priority);
   end Set_Dependents_Fallback_Handler;

end Ada.Task_Termination;
