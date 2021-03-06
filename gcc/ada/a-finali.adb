------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                     A D A . F I N A L I Z A T I O N                      --
--                                                                          --
--                                 B o d y                                  --
--                                                                          --
--          Copyright (C) 1992-2006, Free Software Foundation, Inc.         --
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

with System.Finalization_Root; use System.Finalization_Root;

package body Ada.Finalization is

   ---------
   -- "=" --
   ---------

   function "=" (A, B : Controlled) return Boolean is
   begin
      return Empty_Root_Controlled (A) = Empty_Root_Controlled (B);
   end "=";

   ------------
   -- Adjust --
   ------------

   procedure Adjust (Object : in out Controlled) is
      pragma Warnings (Off, Object);
   begin
      null;
   end Adjust;

   --------------
   -- Finalize --
   --------------

   procedure Finalize (Object : in out Controlled) is
      pragma Warnings (Off, Object);
   begin
      null;
   end Finalize;

   procedure Finalize (Object : in out Limited_Controlled) is
      pragma Warnings (Off, Object);
   begin
      null;
   end Finalize;

   ----------------
   -- Initialize --
   ----------------

   procedure Initialize (Object : in out Controlled) is
      pragma Warnings (Off, Object);
   begin
      null;
   end Initialize;

   procedure Initialize (Object : in out Limited_Controlled) is
      pragma Warnings (Off, Object);
   begin
      null;
   end Initialize;

end Ada.Finalization;
