------------------------------------------------------------------------------
--                                                                          --
--                 GNAT RUN-TIME LIBRARY (GNARL) COMPONENTS                 --
--                                                                          --
--         S Y S T E M . T A S K I N G . I N I T I A L I Z A T I O N        --
--                                                                          --
--                                  S p e c                                 --
--                                                                          --
--          Copyright (C) 1992-2005, Free Software Foundation, Inc.         --
--                                                                          --
-- GNARL is free software; you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 2,  or (at your option) any later ver- --
-- sion. GNARL is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License --
-- for  more details.  You should have  received  a copy of the GNU General --
-- Public License  distributed with GNARL; see file COPYING.  If not, write --
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
-- GNARL was developed by the GNARL team at Florida State University.       --
-- Extensive contributions were provided by Ada Core Technologies, Inc.     --
--                                                                          --
------------------------------------------------------------------------------

--  This package provides overall initialization of the tasking portion of the
--  RTS. This package must be elaborated before any tasking features are used.

package System.Tasking.Initialization is

   procedure Remove_From_All_Tasks_List (T : Task_Id);
   --  Remove T from All_Tasks_List. Call this function with RTS_Lock taken

   ---------------------------------
   -- Tasking-Specific Soft Links --
   ---------------------------------

   --  These permit us to leave out certain portions of the tasking
   --  run-time system if they are not used.  They are only used internally
   --  by the tasking run-time system.

   --  So far, the only example is support for Ada.Task_Attributes

   type Proc_T is access procedure (T : Task_Id);

   procedure Finalize_Attributes (T : Task_Id);
   procedure Initialize_Attributes (T : Task_Id);

   Finalize_Attributes_Link : Proc_T := Finalize_Attributes'Access;
   --  should be called with abort deferred and T.L write-locked

   Initialize_Attributes_Link : Proc_T := Initialize_Attributes'Access;
   --  should be called with abort deferred, but holding no locks

   -------------------------
   -- Abort Defer/Undefer --
   -------------------------

   --  Defer_Abort defers the affects of low-level abort and priority change
   --  in the calling task until a matching Undefer_Abort call is executed.

   --  Undefer_Abort DOES MORE than just undo the effects of one call to
   --  Defer_Abort. It is the universal "polling point" for deferred
   --  processing, including the following:

   --  1) base priority changes

   --  2) abort/ATC

   --  Abort deferral MAY be nested (Self_ID.Deferral_Level is a count), but
   --  to avoid waste and undetected errors, it generally SHOULD NOT be
   --  nested. The symptom of over-deferring abort is that an exception may
   --  fail to be raised, or an abort may fail to take place.

   --  Therefore, there are two sets of the inlinable defer/undefer routines,
   --  which are the ones to be used inside GNARL. One set allows nesting. The
   --  other does not. People who maintain the GNARL should try to avoid using
   --  the nested versions, or at least look very critically at the places
   --  where they are used.

   --  In general, any GNARL call that is potentially blocking, or whose
   --  semantics require that it sometimes raise an exception, or that is
   --  required to be an abort completion point, must be made with abort
   --  Deferral_Level = 1.

   --  In general, non-blocking GNARL calls, which may be made from inside a
   --  protected action, are likely to need to allow nested abort deferral.

   --  With some critical exceptions (which are supposed to be documented),
   --  internal calls to the tasking runtime system assume abort is already
   --  deferred, and do not modify the deferral level.

   --  There is also a set of non-linable defer/undefer routines, for direct
   --  call from the compiler. These are not in-lineable because they may need
   --  to be called via pointers ("soft links"). For the sake of efficiency,
   --  the version with Self_ID as parameter should used wherever possible.
   --  These are all nestable.

   --  Non-nestable inline versions

   procedure Defer_Abort (Self_ID : Task_Id);
   pragma Inline (Defer_Abort);

   procedure Undefer_Abort (Self_ID : Task_Id);
   pragma Inline (Undefer_Abort);

   --  Nestable inline versions

   procedure Defer_Abort_Nestable (Self_ID : Task_Id);
   pragma Inline (Defer_Abort_Nestable);

   procedure Undefer_Abort_Nestable (Self_ID : Task_Id);
   pragma Inline (Undefer_Abort_Nestable);

   procedure Do_Pending_Action (Self_ID : Task_Id);
   --  Only call with no locks, and when Self_ID.Pending_Action = True Perform
   --  necessary pending actions (e.g. abort, priority change). This procedure
   --  is usually called when needed as a result of calling Undefer_Abort,
   --  although in the case of e.g. No_Abort restriction, it can be necessary
   --  to force execution of pending actions.

   function Check_Abort_Status return Integer;
   --  Returns Boolean'Pos (True) iff abort signal should raise
   --  Standard.Abort_Signal. Only used by IRIX currently.

   --------------------------
   -- Change Base Priority --
   --------------------------

   procedure Change_Base_Priority (T : Task_Id);
   --  Change the base priority of T. Has to be called with the affected
   --  task's ATCB write-locked. May temporariliy release the lock.

   procedure Poll_Base_Priority_Change (Self_ID : Task_Id);
   --  Has to be called with Self_ID's ATCB write-locked.
   --  May temporariliy release the lock.
   pragma Inline (Poll_Base_Priority_Change);

   ----------------------
   -- Task Lock/Unlock --
   ----------------------

   procedure Task_Lock (Self_ID : Task_Id);
   pragma Inline (Task_Lock);

   procedure Task_Unlock (Self_ID : Task_Id);
   pragma Inline (Task_Unlock);
   --  These are versions of Lock_Task and Unlock_Task created for use
   --  within the GNARL.

   procedure Final_Task_Unlock (Self_ID : Task_Id);
   --  This version is only for use in Terminate_Task, when the task is
   --  relinquishing further rights to its own ATCB. There is a very
   --  interesting potential race condition there, where the old task may run
   --  concurrently with a new task that is allocated the old tasks (now
   --  reused) ATCB. The critical thing here is to not make any reference to
   --  the ATCB after the lock is released. See also comments on
   --  Terminate_Task and Unlock.

   procedure Wakeup_Entry_Caller
     (Self_ID    : Task_Id;
      Entry_Call : Entry_Call_Link;
      New_State  : Entry_Call_State);
   pragma Inline (Wakeup_Entry_Caller);
   --  This is called at the end of service of an entry call, to abort the
   --  caller if he is in an abortable part, and to wake up the caller if he
   --  is on Entry_Caller_Sleep. Call it holding the lock of Entry_Call.Self.
   --
   --  Timed_Call or Simple_Call:
   --    The caller is waiting on Entry_Caller_Sleep, in Wait_For_Completion,
   --    or Wait_For_Completion_With_Timeout.
   --
   --  Conditional_Call:
   --    The caller might be in Wait_For_Completion,
   --    waiting for a rendezvous (possibly requeued without abort) to
   --    complete.
   --
   --  Asynchronous_Call:
   --    The caller may be executing in the abortable part an async. select,
   --    or on a time delay, if Entry_Call.State >= Was_Abortable.

   procedure Locked_Abort_To_Level
     (Self_ID : Task_Id;
      T       : Task_Id;
      L       : ATC_Level);
   pragma Inline (Locked_Abort_To_Level);
   --  Abort a task to a specified ATC level. Call this only with T locked

end System.Tasking.Initialization;
