------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                       S Y S T E M . W C H _ C O N                        --
--                                                                          --
--                                 B o d y                                  --
--                                                                          --
--          Copyright (C) 2005-2006, Free Software Foundation, Inc.         --
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

package body System.WCh_Con is

   ----------------------------
   -- Get_WC_Encoding_Method --
   ----------------------------

   function Get_WC_Encoding_Method (C : Character) return WC_Encoding_Method is
   begin
      for Method in WC_Encoding_Method loop
         if C = WC_Encoding_Letters (Method) then
            return Method;
         end if;
      end loop;

      raise Constraint_Error;
   end Get_WC_Encoding_Method;

   function Get_WC_Encoding_Method (S : String) return WC_Encoding_Method is
   begin
      if    S = "hex"       then
         return WCEM_Hex;
      elsif S = "upper"     then
         return WCEM_Upper;
      elsif S = "shift_jis" then
         return WCEM_Shift_JIS;
      elsif S = "euc"       then
         return WCEM_EUC;
      elsif S = "utf8"      then
         return WCEM_UTF8;
      elsif S = "brackets"  then
         return WCEM_Brackets;
      else
         raise Constraint_Error;
      end if;
   end Get_WC_Encoding_Method;

end System.WCh_Con;
