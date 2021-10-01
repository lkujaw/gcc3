------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                              G N A T V S N                               --
--                                                                          --
--                                 B o d y                                  --
--                                                                          --
--          Copyright (C) 1992-2006 Free Software Foundation, Inc.          --
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

package body Gnatvsn is

   -------------------------
   -- Get_Gnat_Build_Type --
   -------------------------

   function Get_Gnat_Build_Type return Gnat_Build_Type is
   begin
      case Gnat_Static_Version_String (5) is
         when 'w' | 'a' | 'c' =>
            return Gnatpro;
         when others =>
            return GPL;
      end case;
   end Get_Gnat_Build_Type;

   -------------------------
   -- Gnat_Version_String --
   -------------------------

   function Gnat_Version_String return String is
   begin
      case Get_Gnat_Build_Type is
         when Gnatpro =>
            return "Pro " & Gnat_Static_Version_String;
         when GPL =>
            return "GPL " & Gnat_Static_Version_String;
         when FSF =>
            return Gnat_Static_Version_String;
      end case;
   end Gnat_Version_String;

end Gnatvsn;
