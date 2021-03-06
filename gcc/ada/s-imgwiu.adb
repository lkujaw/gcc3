------------------------------------------------------------------------------
--                                                                          --
--                         GNAT RUN-TIME COMPONENTS                         --
--                                                                          --
--                       S Y S T E M . I M G _ W I U                        --
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

with System.Unsigned_Types; use System.Unsigned_Types;

package body System.Img_WIU is

   -----------------------------
   -- Set_Image_Width_Integer --
   -----------------------------

   procedure Set_Image_Width_Integer
     (V : Integer;
      W : Integer;
      S : out String;
      P : in out Natural)
   is
      Start : Natural;

   begin
      --  Positive case can just use the unsigned circuit directly

      if V >= 0 then
         Set_Image_Width_Unsigned (Unsigned (V), W, S, P);

      --  Negative case has to set a minus sign. Note also that we have to be
      --  careful not to generate overflow with the largest negative number.

      else
         P := P + 1;
         S (P) := ' ';
         Start := P;

         declare
            pragma Suppress (Overflow_Check);
            pragma Suppress (Range_Check);
         begin
            Set_Image_Width_Unsigned (Unsigned (-V), W - 1, S, P);
         end;

         --  Set minus sign in last leading blank location. Because of the
         --  code above, there must be at least one such location.

         while S (Start + 1) = ' ' loop
            Start := Start + 1;
         end loop;

         S (Start) := '-';
      end if;

   end Set_Image_Width_Integer;

   ------------------------------
   -- Set_Image_Width_Unsigned --
   ------------------------------

   procedure Set_Image_Width_Unsigned
     (V : Unsigned;
      W : Integer;
      S : out String;
      P : in out Natural)
   is
      Start : constant Natural := P;
      F, T  : Natural;

      procedure Set_Digits (T : Unsigned);
      --  Set digits of absolute value of T

      procedure Set_Digits (T : Unsigned) is
      begin
         if T >= 10 then
            Set_Digits (T / 10);
            P := P + 1;
            S (P) := Character'Val (T mod 10 + Character'Pos ('0'));
         else
            P := P + 1;
            S (P) := Character'Val (T + Character'Pos ('0'));
         end if;
      end Set_Digits;

   --  Start of processing for Set_Image_Width_Unsigned

   begin
      Set_Digits (V);

      --  Add leading spaces if required by width parameter

      if P - Start < W then
         F := P;
         P := P + (W - (P - Start));
         T := P;

         while F > Start loop
            S (T) := S (F);
            T := T - 1;
            F := F - 1;
         end loop;

         for J in Start + 1 .. T loop
            S (J) := ' ';
         end loop;
      end if;

   end Set_Image_Width_Unsigned;

end System.Img_WIU;
