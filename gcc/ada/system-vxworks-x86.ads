------------------------------------------------------------------------------
--                                                                          --
--                        GNAT RUN-TIME COMPONENTS                          --
--                                                                          --
--                               S Y S T E M                                --
--                                                                          --
--                                 S p e c                                  --
--                         (VxWorks Version x86)                            --
--                                                                          --
--          Copyright (C) 1992-2006, Free Software Foundation, Inc.         --
--                                                                          --
-- This specification is derived from the Ada Reference Manual for use with --
-- GNAT. The copyright notice above, and the license provisions that follow --
-- apply solely to the  contents of the part following the private keyword. --
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

package System is
   pragma Pure;
   --  Note that we take advantage of the implementation permission to make
   --  this unit Pure instead of Preelaborable; see RM 13.7.1(15). In Ada
   --  2005, this is Pure in any case (AI-362).

   type Name is (SYSTEM_NAME_GNAT);
   System_Name : constant Name := SYSTEM_NAME_GNAT;

   --  System-Dependent Named Numbers

   Min_Int               : constant := Long_Long_Integer'First;
   Max_Int               : constant := Long_Long_Integer'Last;

   Max_Binary_Modulus    : constant := 2 ** Long_Long_Integer'Size;
   Max_Nonbinary_Modulus : constant := Integer'Last;

   Max_Base_Digits       : constant := Long_Long_Float'Digits;
   Max_Digits            : constant := Long_Long_Float'Digits;

   Max_Mantissa          : constant := 63;
   Fine_Delta            : constant := 2.0 ** (-Max_Mantissa);

   Tick                  : constant := 1.0 / 60.0;

   --  Storage-related Declarations

   type Address is private;
   Null_Address : constant Address;

   Storage_Unit : constant := 8;
   Word_Size    : constant := 32;
   Memory_Size  : constant := 2 ** 32;

   --  Address comparison

   function "<"  (Left, Right : Address) return Boolean;
   function "<=" (Left, Right : Address) return Boolean;
   function ">"  (Left, Right : Address) return Boolean;
   function ">=" (Left, Right : Address) return Boolean;
   function "="  (Left, Right : Address) return Boolean;

   pragma Import (Intrinsic, "<");
   pragma Import (Intrinsic, "<=");
   pragma Import (Intrinsic, ">");
   pragma Import (Intrinsic, ">=");
   pragma Import (Intrinsic, "=");

   --  Other System-Dependent Declarations

   type Bit_Order is (High_Order_First, Low_Order_First);
   Default_Bit_Order : constant Bit_Order := Low_Order_First;
   pragma Warnings (Off, Default_Bit_Order); -- kill constant condition warning

   --  Priority-related Declarations (RM D.1)

   --  256        is reserved for the VxWorks kernel
   --  248 - 255  correspond to hardware interrupt levels 0 .. 7
   --  247        is a catchall default "interrupt" priority for signals,
   --             allowing higher priority than normal tasks, but lower than
   --             hardware priority levels.  Protected Object ceilings can
   --             override these values.
   --  246        is used by the Interrupt_Manager task

   Max_Priority           : constant Positive := 245;
   Max_Interrupt_Priority : constant Positive := 255;

   subtype Any_Priority       is Integer      range   0 .. 255;
   subtype Priority           is Any_Priority range   0 .. 245;
   subtype Interrupt_Priority is Any_Priority range 246 .. 255;

   Default_Priority : constant Priority := 122;

private

   type Address is mod Memory_Size;
   Null_Address : constant Address := 0;

   --------------------------------------
   -- System Implementation Parameters --
   --------------------------------------

   --  These parameters provide information about the target that is used
   --  by the compiler. They are in the private part of System, where they
   --  can be accessed using the special circuitry in the Targparm unit
   --  whose source should be consulted for more detailed descriptions
   --  of the individual switch values.

   AAMP                      : constant Boolean := False;
   Backend_Divide_Checks     : constant Boolean := False;
   Backend_Overflow_Checks   : constant Boolean := False;
   Command_Line_Args         : constant Boolean := False;
   Compiler_System_Version   : constant Boolean := False;
   Configurable_Run_Time     : constant Boolean := False;
   Denorm                    : constant Boolean := True;
   Duration_32_Bits          : constant Boolean := False;
   Exit_Status_Supported     : constant Boolean := True;
   Fractional_Fixed_Ops      : constant Boolean := False;
   Frontend_Layout           : constant Boolean := False;
   Functions_Return_By_DSP   : constant Boolean := False;
   Machine_Overflows         : constant Boolean := False;
   Machine_Rounds            : constant Boolean := True;
   OpenVMS                   : constant Boolean := False;
   Preallocated_Stacks       : constant Boolean := False;
   Signed_Zeros              : constant Boolean := True;
   Stack_Check_Default       : constant Boolean := False;
   Stack_Check_Probes        : constant Boolean := False;
   Support_64_Bit_Divides    : constant Boolean := True;
   Support_Aggregates        : constant Boolean := True;
   Support_Composite_Assign  : constant Boolean := True;
   Support_Composite_Compare : constant Boolean := True;
   Support_Long_Shifts       : constant Boolean := True;
   Suppress_Standard_Library : constant Boolean := False;
   Use_Ada_Main_Program_Name : constant Boolean := True;
   ZCX_By_Default            : constant Boolean := False;
   GCC_ZCX_Support           : constant Boolean := False;
   Front_End_ZCX_Support     : constant Boolean := False;

   --  Obsolete entries, to be removed eventually (bootstrap issues!)

   High_Integrity_Mode       : constant Boolean := False;
   Long_Shifts_Inlined       : constant Boolean := False;

end System;
