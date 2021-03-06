------------------------------------------------------------------------------
--                                                                          --
--                         GNAT RUN-TIME COMPONENTS                         --
--                                                                          --
--                       S Y S T E M . I M G _ I N T                        --
--                                                                          --
--                                 B o d y                                  --
--                                                                          --
--           Copyright (C) 1992-2005 Free Software Foundation, Inc.         --
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

package body System.Img_Int is

   -------------------
   -- Image_Integer --
   -------------------

   function Image_Integer (V : Integer) return String is
      P : Natural;
      S : String (1 .. Integer'Width);

   begin
      if V >= 0 then
         P := 1;
         S (P) := ' ';
      else
         P := 0;
      end if;

      Set_Image_Integer (V, S, P);
      return S (1 .. P);
   end Image_Integer;

   -----------------------
   -- Set_Image_Integer --
   -----------------------

   procedure Set_Image_Integer
     (V : Integer;
      S : out String;
      P : in out Natural)
   is
      procedure Set_Digits (T : Integer);
      --  Set digits of absolute value of T, which is zero or negative. We work
      --  with the negative of the value so that the largest negative number is
      --  not a special case.

      procedure Set_Digits (T : Integer) is
      begin
         if T <= -10 then
            Set_Digits (T / 10);
            P := P + 1;
            S (P) := Character'Val (48 - (T rem 10));

         else
            P := P + 1;
            S (P) := Character'Val (48 - T);
         end if;
      end Set_Digits;

   --  Start of processing for Set_Image_Integer

   begin
      if V >= 0 then
         Set_Digits (-V);

      else
         P := P + 1;
         S (P) := '-';
         Set_Digits (V);
      end if;
   end Set_Image_Integer;

end System.Img_Int;
