------------------------------------------------------------------------------
--                                                                          --
--                         GNAT RUN-TIME COMPONENTS                         --
--                                                                          --
--                      S Y S T E M . W W D _ C H A R                       --
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

package body System.WWd_Char is

   --------------------------
   -- Wide_Width_Character --
   --------------------------

   function Wide_Width_Character (Lo, Hi : Character) return Natural is
      W : Natural;

   begin
      W := 0;
      for C in Lo .. Hi loop
         --  For Character range, use length of image

         if Character'Pos (C) < 256 then
            declare
               S : constant Wide_String := Character'Wide_Image (C);
            begin
               W := Natural'Max (W, S'Length);
            end;

            --  For wide character, always max out at 12 (Hex_hhhhhhhh)

         else
            return 12;
         end if;
      end loop;

      return W;
   end Wide_Width_Character;

   -------------------------------
   -- Wide_Wide_Width_Character --
   -------------------------------

   function Wide_Wide_Width_Character (Lo, Hi : Character) return Natural is
      W : Natural;

   begin
      W := 0;
      for C in Lo .. Hi loop

         --  For Character range, use length of image

         if Character'Pos (C) < 256 then
            declare
               S : constant String := Character'Image (C);
            begin
               W := Natural'Max (W, S'Length);
            end;

            --  For wide character, always max out at 12 (Hex_hhhhhhhh)

         else
            return 12;
         end if;
      end loop;

      return W;
   end Wide_Wide_Width_Character;

end System.WWd_Char;
