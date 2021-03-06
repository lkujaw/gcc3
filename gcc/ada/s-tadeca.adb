------------------------------------------------------------------------------
--                                                                          --
--                 GNAT RUN-TIME LIBRARY (GNARL) COMPONENTS                 --
--                                                                          --
--           S Y S T E M . T A S K I N G . A S Y N C _ D E L A Y S .        --
--                      E N Q U E U E _ C A L E N D A R                     --
--                                                                          --
--                                  B o d y                                 --
--                                                                          --
--          Copyright (C) 1998-2005 Free Software Foundation, Inc.          --
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

with Ada.Calendar.Delays;
with System.Task_Primitives.Operations;
with System.Tasking.Initialization;

function System.Tasking.Async_Delays.Enqueue_Calendar
  (T    : Ada.Calendar.Time;
   D    : Delay_Block_Access)
   return Boolean
is
   use type Ada.Calendar.Time;
begin
   if T <= Ada.Calendar.Clock then
      D.Timed_Out := True;
      System.Task_Primitives.Operations.Yield;
      return False;
   end if;

   System.Tasking.Initialization.Defer_Abort
     (System.Task_Primitives.Operations.Self);
   Time_Enqueue (Ada.Calendar.Delays.To_Duration (T), D);
   return True;
end System.Tasking.Async_Delays.Enqueue_Calendar;
