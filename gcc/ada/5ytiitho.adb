------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--       S Y S T E M . T H R E A D S . I N I T I A L I Z A T I O N .        --
--                I N I T I A L I Z E _ T A S K _ H O O K S                 --
--                                                                          --
--                                 B o d y                                  --
--                                                                          --
--          Copyright (C) 1992-2003 Free Software Foundation, Inc.          --
--                                                                          --
-- GNAT is free software;  you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 2,  or (at your option) any later ver- --
-- sion.  GNAT is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License --
-- for  more details.  You should have  received  a copy of the GNU General --
-- Public License  distributed with GNAT;  see file COPYING.  If not, write --
-- to  the Free Software Foundation,  59 Temple Place - Suite 330,  Boston, --
-- MA 02111-1307, USA.                                                      --
--                                                                          --
-- As a special exception,  if other files  instantiate  generics from this --
-- unit, or you link  this unit with other files  to produce an executable, --
-- this  unit  does not  by itself cause  the resulting  executable  to  be --
-- covered  by the  GNU  General  Public  License.  This exception does not --
-- however invalidate  any other reasons why  the executable file  might be --
-- covered by the  GNU Public License.                                      --
--                                                                          --
-- GNAT was originally developed  by the GNAT team at  New York University. --
-- Extensive contributions were provided by Ada Core Technologies Inc.      --
--                                                                          --
------------------------------------------------------------------------------

--  This is the VxWorks AE 653 version of this procedure

separate (System.Threads.Initialization)

procedure Initialize_Task_Hooks is

   --  When defining the following routines for export in an AE 1.1
   --  simulation of AE653, Interfaces.C.int may be used for the
   --  parameters of FUNCPTR.
   type FUNCPTR is access function (T : OSI.Thread_Id) return OSI.STATUS;

   --------------------------------
   -- Imported vThreads Routines --
   --------------------------------

   procedure procCreateHookAdd (createHookFunction : FUNCPTR);
   pragma Import (C, procCreateHookAdd, "procCreateHookAdd");
   --  Registers task registration routine for AE653

   procedure procStartHookAdd (StartHookFunction : FUNCPTR);
   pragma Import (C, procStartHookAdd, "procStartHookAdd");
   --  Registers task restart routine for AE653

   Result : OSI.STATUS;
begin
   --  Register the exported routines with the vThreads ARINC API
   procCreateHookAdd (Register'Access);
   procStartHookAdd (Reset_TSD'Access);
   --  Register the environment task
   Result := Register (OSI.taskIdSelf);
   pragma Assert (Result /= -1);
end Initialize_Task_Hooks;