------------------------------------------------------------------------------
--                                                                          --
--                 GNAT RUN-TIME LIBRARY (GNARL) COMPONENTS                 --
--                                                                          --
--                  S Y S T E M . O S _ P R I M I T I V E S                 --
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
-- As a special exception,  if other files  instantiate  generics from this --
-- unit, or you link  this unit with other files  to produce an executable, --
-- this  unit  does not  by itself cause  the resulting  executable  to  be --
-- covered  by the  GNU  General  Public  License.  This exception does not --
-- however invalidate  any other reasons why  the executable file  might be --
-- covered by the  GNU Public License.                                      --
--                                                                          --
-- GNARL was developed by the GNARL team at Florida State University.       --
-- Extensive contributions were provided by Ada Core Technologies, Inc.     --
--                                                                          --
------------------------------------------------------------------------------

--  This file is suitable for SCO UnixWare and uses gettimeofday and poll.

with Interfaces.C;
use type Interfaces.C.int;

package body System.OS_Primitives is

   type struct_timeval is record
      tv_sec  : Long_Integer;
      tv_usec : Long_Integer;
   end record;
   pragma Convention (C, struct_timeval);

   function gettimeofday
     (tv : access struct_timeval;
      tz : Address) return Integer;
   pragma Import (C, gettimeofday);

   function poll
     (fds     : Address;
      nfds    : Interfaces.C.unsigned_long;
      timeout : Interfaces.C.int) return Interfaces.C.int;
   pragma Import (C, poll);

   -----------
   -- Clock --
   -----------

   function Clock return Duration is
      TV : aliased struct_timeval;
   begin
      if gettimeofday (TV'Access, Null_Address) = -1 then
         raise Program_Error;
      end if;
      return Duration (TV.tv_sec) + Duration (TV.tv_usec) / 10#1#E6;
   end Clock;

   ---------------------
   -- Monotonic_Clock --
   ---------------------

   function Monotonic_Clock return Duration renames Clock;

   -----------------
   -- Timed_Delay --
   -----------------

   procedure Timed_Delay
     (Time : Duration;
      Mode : Integer)
   is
      Rel_Time   : Duration;
      Abs_Time   : Duration;
      Check_Time : Duration := Clock;
      Timeout    : Interfaces.C.int;
      Result     : Interfaces.C.int;
      pragma Unreferenced (Result);
   begin
      if Mode = Relative then
         Rel_Time := Time;
         Abs_Time := Time + Check_Time;
      else
         Rel_Time := Time - Check_Time;
         Abs_Time := Time;
      end if;

      if Rel_Time > 0.0 then
         loop
            if Rel_Time >= Duration (Interfaces.C.int'Last) then
               Timeout := Interfaces.C.int'Last;
            elsif Interfaces.C.int (Rel_Time) = 0 then
               Timeout := 1;
            else
               Timeout := Interfaces.C.int (Rel_Time);
            end if;

            Result := poll (Null_Address, 0, Timeout);
            Check_Time := Clock;

            exit when Abs_Time <= Check_Time;

            Rel_Time := Abs_Time - Check_Time;
         end loop;
      end if;
   end Timed_Delay;

   ----------------
   -- Initialize --
   ----------------

   procedure Initialize is
   begin
      null;
   end Initialize;

end System.OS_Primitives;
