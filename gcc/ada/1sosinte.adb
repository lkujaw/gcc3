------------------------------------------------------------------------------
--                                                                          --
--                  GNAT RUN-TIME LIBRARY (GNARL) COMPONENTS                --
--                                                                          --
--                   S Y S T E M . O S _ I N T E R F A C E                  --
--                                                                          --
--                                   B o d y                                --
--                                                                          --
--          Copyright (C) 1997-2005, Free Software Foundation, Inc.         --
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

--  This is the VxWorks/HI-E version

--  This package encapsulates all direct interfaces to OS services
--  that are needed by children of System.

pragma Polling (Off);
--  Turn off polling, we do not want ATC polling to take place during
--  tasking operations. It causes infinite loops and other problems.

package body System.OS_Interface is

   use type Interfaces.C.int;

   ----------
   -- kill --
   ----------

   --  This function is not supposed to be used by the HI run time, and it is
   --  not supposed to be with'ed by the user either (it is an internal GNAT
   --  unit). It is kept here (returning a junk value) just for sharing the
   --  same package specification with the regular run time.

   function kill (pid : t_id; sig : Signal) return int is
      pragma Unreferenced (pid, sig);
   begin
      return -1;
   end kill;

   --------------------
   -- Set_Time_Slice --
   --------------------

   --  This function is not supposed to be used by the HI run time, and it is
   --  not supposed to be with'ed by the user either (it is an internal GNAT
   --  unit). It is kept here (returning a junk value) just for sharing the
   --  same package specification with the regular run time.

   function Set_Time_Slice (ticks : int) return int is
      pragma Unreferenced (ticks);
   begin
      return -1;
   end Set_Time_Slice;

   -------------
   -- sigwait --
   -------------

   --  This function is not supposed to be used by the HI run time, and it is
   --  not supposed to be with'ed by the user either (it is an internal GNAT
   --  unit). It is kept here (returning a junk value) just for sharing the
   --  same package specification with the regular run time.

   function sigwait
     (set : access sigset_t;
      sig : access Signal) return int
   is
      pragma Unreferenced (set, sig);
   begin
      return -1;
   end sigwait;

   -----------------
   -- To_Duration --
   -----------------

   --  This function is not supposed to be used by the HI run time, and it is
   --  not supposed to be with'ed by the user either (it is an internal GNAT
   --  unit). It is kept here (returning a junk value) just for sharing the
   --  same package specification with the regular run time.

   function To_Duration (TS : timespec) return Duration is
      pragma Unreferenced (TS);
   begin
      return 0.0;
   end To_Duration;

   -----------------
   -- To_Timespec --
   -----------------

   --  This function is not supposed to be used by the HI run time, and it is
   --  not supposed to be with'ed by the user either (it is an internal GNAT
   --  unit). It is kept here (returning a junk value) just for sharing the
   --  same package specification with the regular run time.

   function To_Timespec (D : Duration) return timespec is
      pragma Unreferenced (D);
   begin
      return timespec'(0, 0);
   end To_Timespec;

   -------------------------
   -- To_VxWorks_Priority --
   -------------------------

   --  This function is not supposed to be used by the HI run time, and it is
   --  not supposed to be with'ed by the user either (it is an internal GNAT
   --  unit). It is kept here (returning a junk value) just for sharing the
   --  same package specification with the regular run time.

   function To_VxWorks_Priority (Priority : in int) return int is
      pragma Unreferenced (Priority);
   begin
      return 0;
   end To_VxWorks_Priority;

   --------------------
   -- To_Clock_Ticks --
   --------------------

   --  This function is not supposed to be used by the HI run time, and it is
   --  not supposed to be with'ed by the user either (it is an internal GNAT
   --  unit). It is kept here (returning a junk value) just for sharing the
   --  same package specification with the regular run time.

   function To_Clock_Ticks (D : Duration) return int is
      pragma Unreferenced (D);
   begin
      return 0;
   end To_Clock_Ticks;

   ----------------
   -- VX_FP_TASK --
   ----------------

   function VX_FP_TASK return int is
   begin
      return 16#0008#;
   end VX_FP_TASK;

end System.OS_Interface;
