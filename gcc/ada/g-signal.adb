------------------------------------------------------------------------------
--                                                                          --
--                         GNAT RUN-TIME COMPONENTS                         --
--                                                                          --
--                         G N A T . S I G N A L S                          --
--                                                                          --
--                                 B o d y                                  --
--                                                                          --
--          Copyright (C) 2003-2005, Free Software Foundation, Inc.         --
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

with System.Interrupts;

package body GNAT.Signals is

   package SI renames System.Interrupts;

   ------------------
   -- Block_Signal --
   ------------------

   procedure Block_Signal (Signal : Ada.Interrupts.Interrupt_ID) is
   begin
      SI.Block_Interrupt (SI.Interrupt_ID (Signal));
   end Block_Signal;

   ----------------
   -- Is_Blocked --
   ----------------

   function Is_Blocked (Signal : Ada.Interrupts.Interrupt_ID) return Boolean is
   begin
      return SI.Is_Blocked (SI.Interrupt_ID (Signal));
   end Is_Blocked;

   --------------------
   -- Unblock_Signal --
   --------------------

   procedure Unblock_Signal (Signal : Ada.Interrupts.Interrupt_ID) is
   begin
      SI.Unblock_Interrupt (SI.Interrupt_ID (Signal));
   end Unblock_Signal;

end GNAT.Signals;
