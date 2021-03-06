------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                       S Y S T E M . V A L _ D E C                        --
--                                                                          --
--                                 B o d y                                  --
--                                                                          --
--          Copyright (C) 1992-2005, Free Software Foundation, Inc.         --
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

with System.Val_Real; use System.Val_Real;

package body System.Val_Dec is

   ------------------
   -- Scan_Decimal --
   ------------------

   --  For decimal types where Size < Integer'Size, it is fine to use
   --  the floating-point circuit, since it certainly has sufficient
   --  precision for any reasonable hardware, and we just don't support
   --  things on junk hardware!

   function Scan_Decimal
     (Str   : String;
      Ptr   : access Integer;
      Max   : Integer;
      Scale : Integer) return Integer
   is
      Val : Long_Long_Float;
   begin
      Val := Scan_Real (Str, Ptr, Max);
      return Integer (Val * 10.0 ** Scale);
   end Scan_Decimal;

   -------------------
   -- Value_Decimal --
   -------------------

   --  Again, we use the real circuit for this purpose

   function Value_Decimal (Str : String; Scale : Integer) return Integer is
   begin
      return Integer (Value_Real (Str) * 10.0 ** Scale);
   end Value_Decimal;

end System.Val_Dec;
