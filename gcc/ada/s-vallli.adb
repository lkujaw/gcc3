------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                       S Y S T E M . V A L _ L L I                        --
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

with System.Unsigned_Types; use System.Unsigned_Types;
with System.Val_LLU;        use System.Val_LLU;
with System.Val_Util;       use System.Val_Util;

package body System.Val_LLI is

   ----------------------------
   -- Scan_Long_Long_Integer --
   ----------------------------

   function Scan_Long_Long_Integer
     (Str  : String;
      Ptr  : access Integer;
      Max  : Integer) return Long_Long_Integer
   is
      Uval : Long_Long_Unsigned;
      --  Unsigned result

      Minus : Boolean := False;
      --  Set to True if minus sign is present, otherwise to False

      Start : Positive;
      --  Saves location of first non-blank (not used in this case)

   begin
      Scan_Sign (Str, Ptr, Max, Minus, Start);

      if Str (Ptr.all) not in '0' .. '9' then
         Ptr.all := Start;
         raise Constraint_Error;
      end if;

      Uval := Scan_Raw_Long_Long_Unsigned (Str, Ptr, Max);

      --  Deal with overflow cases, and also with maximum negative number

      if Uval > Long_Long_Unsigned (Long_Long_Integer'Last) then
         if Minus
           and then Uval = Long_Long_Unsigned (-(Long_Long_Integer'First))
         then
            return Long_Long_Integer'First;
         else
            raise Constraint_Error;
         end if;

      --  Negative values

      elsif Minus then
         return -(Long_Long_Integer (Uval));

      --  Positive values

      else
         return Long_Long_Integer (Uval);
      end if;
   end Scan_Long_Long_Integer;

   -----------------------------
   -- Value_Long_Long_Integer --
   -----------------------------

   function Value_Long_Long_Integer (Str : String) return Long_Long_Integer is
      V : Long_Long_Integer;
      P : aliased Integer := Str'First;
   begin
      V := Scan_Long_Long_Integer (Str, P'Access, Str'Last);
      Scan_Trailing_Blanks (Str, P);
      return V;
   end Value_Long_Long_Integer;

end System.Val_LLI;
