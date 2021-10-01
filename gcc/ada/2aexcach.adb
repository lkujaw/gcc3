------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                         ADA.EXCEPTIONS.CALL_CHAIN                        --
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
-- to  the Free Software Foundation,  59 Temple Place - Suite 330,  Boston, --
-- MA 02111-1307, USA.                                                      --
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

--  This version is for the AE653 Level A run time and for bare board targets

pragma Warnings (Off);
--  Allow withing of non-Preelaborated units in Ada 2005 mode where this
--  package will be categorized as Preelaborate. See AI-362 for details.
--  It is safe in the context of the run-time to violate the rules!

with System.Traceback;
with Ada.Exceptions.Traceback;

pragma Warnings (On);

separate (Ada.Exceptions)

procedure Call_Chain (Excep : EOA) is

   pragma Assert (Excep.Num_Tracebacks = 0);
   --  This is not a reraise situation

   Exception_Tracebacks : Integer;
   pragma Import (C, Exception_Tracebacks, "__gl_exception_tracebacks");
   --  Boolean indicating whether tracebacks should be stored in exception
   --  occurrences.

begin
   if Exception_Tracebacks /= 0 then

      --  If Exception_Tracebacks = 0 then the program was not compiled for
      --  storing tracebacks in exception occurrences (-bargs -E switch) so
      --  that we do not generate them.

      --  We ask System.Traceback.Call_Chain to skip 3 frames to ensure that
      --  itself, ourselves and our caller are not part of the result. Our
      --  caller is always an exception propagation actor that we don't want
      --  to see, and it may be part of a separate subunit which pulls it
      --  outside the AAA/ZZZ range.

      System.Traceback.Call_Chain
        (Traceback   =>
           Ada.Exceptions.Traceback.Tracebacks_Array (Excep.Tracebacks),
         Max_Len     => Max_Tracebacks,
         Len         => Excep.Num_Tracebacks,
         Exclude_Min => Code_Address_For_AAA,
         Exclude_Max => Code_Address_For_ZZZ,
         Skip_Frames => 3);
   end if;
end Call_Chain;
