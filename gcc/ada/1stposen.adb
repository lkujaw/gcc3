------------------------------------------------------------------------------
--                                                                          --
--                 GNAT RUN-TIME LIBRARY (GNARL) COMPONENTS                 --
--                                                                          --
--     S Y S T E M . T A S K I N G . P R O T E C T E D _ O B J E C T S .    --
--                          S I N G L E _ E N T R Y                         --
--                                                                          --
--                                  B o d y                                 --
--                                                                          --
--                     Copyright (C) 1998-2005, AdaCore                     --
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
------------------------------------------------------------------------------

pragma Style_Checks (All_Checks);
--  Turn off subprogram ordering check, since restricted GNARLI
--  subprograms are gathered together at end.

--  This package provides an optimized version of Protected_Objects.Operations
--  and Protected_Objects.Entries making the following assumptions:
--
--  PO have only one entry
--  There is only one caller at a time (No_Entry_Queue)
--  There is no dynamic priority support (No_Dynamic_Priorities)
--  No Abort Statements
--    (No_Abort_Statements, Max_Asynchronous_Select_Nesting => 0)
--  PO are at library level
--  No Requeue
--  None of the tasks will terminate (no need for finalization)
--
--  This interface is intended to be used in the ravenscar and restricted
--  profiles, the compiler is responsible for ensuring that the conditions
--  mentioned above are respected, except for the No_Entry_Queue restriction
--  that is checked dynamically in this package, since the check cannot be
--  performed at compile time (see Protected_Single_Entry_Call, Service_Entry).

pragma Polling (Off);
--  Turn off polling, we do not want polling to take place during tasking
--  operations. It can cause  infinite loops and other problems.

pragma Suppress (All_Checks);

with System.Task_Primitives.Operations;
--  used for Self
--           Get_Priority
--           Set_Priority

package body System.Tasking.Protected_Objects.Single_Entry is

   package STPO renames System.Task_Primitives.Operations;

   --------------------------------
   -- Complete_Single_Entry_Body --
   --------------------------------

   procedure Complete_Single_Entry_Body (Object : Protection_Entry_Access) is
      pragma Unreferenced (Object);
   begin
      --  Nothing needs to be done.
      null;
   end Complete_Single_Entry_Body;

   ---------------------------------
   -- Initialize_Protection_Entry --
   ---------------------------------

   procedure Initialize_Protection_Entry
     (Object           : Protection_Entry_Access;
      Ceiling_Priority : Integer;
      Compiler_Info    : System.Address;
      Entry_Body       : Entry_Body_Access)
   is
      Init_Priority : Integer := Ceiling_Priority;
   begin
      if Init_Priority = Unspecified_Priority then
         Init_Priority := System.Priority'Last;
      end if;

      Object.Ceiling := System.Any_Priority (Init_Priority);
      Object.Caller_Priority := System.Any_Priority'First;
      Object.Owner := Null_Task;
      Object.Compiler_Info := Compiler_Info;
      Object.Call_In_Progress := null;
      Object.Entry_Body := Entry_Body;
      Object.Entry_Queue := null;
   end Initialize_Protection_Entry;

   ----------------
   -- Lock_Entry --
   ----------------

   procedure Lock_Entry (Object : Protection_Entry_Access) is
      Self_Id         : constant Task_Id := STPO.Self;
      Caller_Priority : constant Any_Priority := STPO.Get_Priority (Self_Id);

   begin
      --  For this run time, pragma Detect_Blocking is always active. As
      --  described in ARM 9.5.1, par. 15, an external call on a protected
      --  subprogram with the same target object as that of the protected
      --  action that is currently in progress (i.e., if the caller is
      --  already the protected object's ownership) is a potentially blocking
      --  operation, and hence Program_Error must be raised.

      if Object.Owner = Self_Id then
         raise Program_Error;
      end if;

      --  Check ceiling locking violation

      if Caller_Priority > Object.Ceiling then
         raise Program_Error;
      end if;

      STPO.Set_Priority (Self_Id, Object.Ceiling);

      --  Update the protected object's owner

      Object.Owner := Self_Id;

      --  Store caller's active priority so that it can be later
      --  restored when finishing the protected action.

      Object.Caller_Priority := Caller_Priority;

      --  We are entering in a protected action, so that we increase the
      --  protected object nesting level.

      Self_Id.Common.Protected_Action_Nesting :=
        Self_Id.Common.Protected_Action_Nesting + 1;
   end Lock_Entry;

   ----------------------------
   -- Protected_Single_Count --
   ----------------------------

   function Protected_Count_Entry (Object : Protection_Entry) return Natural is
   begin
      return Boolean'Pos (Object.Entry_Queue /= null);
   end Protected_Count_Entry;

   ---------------------------------
   -- Protected_Single_Entry_Call --
   ---------------------------------

   procedure Protected_Single_Entry_Call
     (Object             : Protection_Entry_Access;
      Uninterpreted_Data : System.Address;
      Mode               : Call_Modes)
   is
      pragma Unreferenced (Mode);

      Self_Id : constant Task_Id := STPO.Self;

   begin
      --  For this run time, pragma Detect_Blocking is always active, so
      --  we must raise Program_Error if this potentially blocking operation
      --  is called from a protected action.

      if Self_Id.Common.Protected_Action_Nesting > 0 then
         raise Program_Error;
      end if;

      Lock_Entry (Object);
      Self_Id.Entry_Call.Uninterpreted_Data := Uninterpreted_Data;

      if Object.Entry_Body.Barrier (Object.Compiler_Info, 1) then

         --  No other task can be executing an entry within this protected
         --  object. On a single processor implementation (such as this one),
         --  the ceiling priority protocol and the strictly preemptive
         --  priority scheduling policy guarantee that protected objects are
         --  always available when any task tries to use them (otherwise,
         --  either the currently executing task would not have had a high
         --  enough priority to be executing, or a blocking operation would
         --  have been called from within the entry body).

         pragma Assert (Object.Call_In_Progress = null);

         Object.Call_In_Progress := Self_Id.Entry_Call'Access;
         Object.Entry_Body.Action
           (Object.Compiler_Info, Self_Id.Entry_Call.Uninterpreted_Data, 1);
         Object.Call_In_Progress := null;

         --  Entry call is over

         Unlock_Entry (Object);

      else
         if Object.Entry_Queue /= null then

            --  This violates the No_Entry_Queue restriction, raise
            --  Program_Error.

            Unlock_Entry (Object);
            raise Program_Error;
         end if;

         Object.Entry_Queue := Self_Id.Entry_Call'Access;
         Unlock_Entry (Object);

         --  Suspend until entry call has been completed.
         --  On exit, the call will not be queued.

         Self_Id.Common.State := Entry_Caller_Sleep;
         STPO.Sleep (Self_Id, Entry_Caller_Sleep);
         Self_Id.Common.State := Runnable;
      end if;
   end Protected_Single_Entry_Call;

   -----------------------------------
   -- Protected_Single_Entry_Caller --
   -----------------------------------

   function Protected_Single_Entry_Caller
     (Object : Protection_Entry) return Task_Id
   is
   begin
      return Object.Call_In_Progress.Self;
   end Protected_Single_Entry_Caller;

   -------------------
   -- Service_Entry --
   -------------------

   procedure Service_Entry (Object : Protection_Entry_Access) is
      Entry_Call : constant Entry_Call_Link := Object.Entry_Queue;
      Caller     : Task_Id;

   begin
      if Entry_Call /= null
        and then Object.Entry_Body.Barrier (Object.Compiler_Info, 1)
      then
         Object.Entry_Queue := null;

         --  No other task can be executing an entry within this protected
         --  object. On a single processor implementation (such as this one),
         --  the ceiling priority protocol and the strictly preemptive
         --  priority scheduling policy guarantee that protected objects are
         --  always available when any task tries to use them (otherwise,
         --  either the currently executing task would not have had a high
         --  enough priority to be executing, or a blocking operation would
         --  have been called from within the entry body).

         pragma Assert (Object.Call_In_Progress = null);

         Object.Call_In_Progress := Entry_Call;
         Object.Entry_Body.Action
           (Object.Compiler_Info, Entry_Call.Uninterpreted_Data, 1);
         Object.Call_In_Progress := null;
         Caller := Entry_Call.Self;
         Unlock_Entry (Object);

         --  Signal the entry caller that the entry is completed

         STPO.Wakeup (Caller, Entry_Caller_Sleep);

      else
         --  Just unlock the entry

         Unlock_Entry (Object);
      end if;
   end Service_Entry;

   ------------------
   -- Unlock_Entry --
   ------------------

   procedure Unlock_Entry (Object : Protection_Entry_Access) is
      Self_Id : constant Task_Id := STPO.Self;

   begin
      --  Calls to this procedure can only take place when being within a
      --  protected action and when the caller is the protected object's
      --  owner.

      pragma Assert (Self_Id.Common.Protected_Action_Nesting > 0
                     and then Object.Owner = Self_Id);

      --  Remove ownership of the protected object

      Object.Owner := Null_Task;

      --  We are exiting from a protected action, so that we decrease the
      --  protected object nesting level.

      Self_Id.Common.Protected_Action_Nesting :=
        Self_Id.Common.Protected_Action_Nesting - 1;

      STPO.Set_Priority (Self_Id, Object.Caller_Priority);
   end Unlock_Entry;

end System.Tasking.Protected_Objects.Single_Entry;
