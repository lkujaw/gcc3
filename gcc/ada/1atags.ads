------------------------------------------------------------------------------
--                                                                          --
--                         GNAT RUN-TIME COMPONENTS                         --
--                                                                          --
--                             A D A . T A G S                              --
--                                                                          --
--                                 S p e c                                  --
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

--  This is the HI-E version of this file. It provides full object oriented
--  semantics (including dynamic dispatching and no support for abstract
--  interface types), assuming that tagged types are declared at the library
--  level. Some functionality has been removed in order to simplify this
--  run-time unit. Compared to the full version of this package, the following
--  subprograms have been removed:

--     Internal_Tag, Get_External_Tag, Register_Tag, Descendant_Tag, and
--     Is_Descendant_At_Same_Level: These subprograms are used for
--     cross-referencing the external and internal representation of tags.
--     The implementation of these seach routines was considered neither
--     simple nor esential for this restricted run-time, and hence these
--     functions were removed.

--     Get_Remotely_Callable: Used for distributed systems which are not
--     supported by this restricted run-time.

--     Get_Entry_Index, Get_Offset_Index, Get_Prim_Op_Kind, Get_Tagged_Kind,
--     OSD, Set_Entry_Index, Set_Offset_Index, Set_OSD, Set_Prim_Op_Kind,
--     Set_SSD, SSD, Set_Tagged_Kind: They are used with types that implement
--     limited interfaces and are only invoked when there are selective waits
--     and ATC's where the trigger is a call to an interface operation. These
--     functions have been removed because selective waits and ATC's are not
--     supported by the restricted run time.

--     Displace, IW_Membership, Offset_To_Top, Register_Interface_Tag,
--     Set_Interface_Table, Set_Offset_To_Top, Set_Num_Prim_Ops: They are
--     used with abstract interface types.

with System;
with System.Storage_Elements;
with Unchecked_Conversion;

package Ada.Tags is
   pragma Preelaborate_05;
   --  In accordance with Ada 2005 AI-362

   type Tag is private;
   pragma Preelaborable_Initialization (Tag);

   No_Tag : constant Tag;

   function Expanded_Name (T : Tag) return String;

   function External_Tag (T : Tag) return String;

   function Parent_Tag (T : Tag) return Tag;
   pragma Ada_05 (Parent_Tag);

   Tag_Error : exception;

private
   --  The following subprogram specifications are placed here instead of
   --  the package body to see them from the frontend through rtsfind.

   ---------------------------------------------------------------
   -- Abstract Procedural Interface For The GNAT Dispatch Table --
   ---------------------------------------------------------------

   --  GNAT's Dispatch Table format is customizable in order to match the
   --  format used in another language. GNAT supports programs that use two
   --  different dispatch table formats at the same time: the native format
   --  that supports Ada 95 tagged types and which is described in Ada.Tags,
   --  and a foreign format for types that are imported from some other
   --  language (typically C++) which is described in Interfaces.CPP. The
   --  runtime information kept for each tagged type is separated into two
   --  objects: the Dispatch Table and the Type Specific Data record. These
   --  two objects are allocated statically using the constants:

   --      DT Size  = DT_Prologue_Size  + Nb_Prim * DT_Entry_Size
   --      TSD Size = TSD_Prologue_Size + (1 + Idepth)  * TSD_Entry_Size

   --  where Nb_prim is the number of primitive operations of the given
   --  type and Idepth its inheritance depth.

   --  In order to set or retrieve information from the Dispatch Table or
   --  the Type Specific Data record, GNAT generates calls to Set_XXX or
   --  Get_XXX routines, where XXX is the name of the field of interest.

   type Dispatch_Table;
   type Tag is access all Dispatch_Table;

   No_Tag : constant Tag := null;

   type Type_Specific_Data;
   type Type_Specific_Data_Ptr is access all Type_Specific_Data;

   Default_Prim_Op_Count : constant Positive := 10;
   --  Number of predefined primitive operations added by the Expander for a
   --  tagged type (must match Exp_Disp.Default_Prim_Op_Count).

   package SSE renames System.Storage_Elements;

   function CW_Membership (Obj_Tag : Tag; Typ_Tag : Tag) return Boolean;
   --  Given the tag of an object and the tag associated to a type, return
   --  true if Obj is in Typ'Class.

   function Get_Access_Level (T : Tag) return Natural;
   --  Given the tag associated with a type, returns the accessibility level
   --  of the type.

   function Get_Predefined_Prim_Op_Address
     (T        : Tag;
      Position : Positive) return System.Address;
   --  Given a pointer to a dispatch table (T) and a position in the DT, this
   --  function returns the address of the predefined virtual function stored
   --  in it (used for dispatching calls).

   function Get_Prim_Op_Address
     (T        : Tag;
      Position : Positive) return System.Address;
   --  Given a pointer to a dispatch table (T) and a position in the DT
   --  this function returns the address of the virtual function stored
   --  in it (used for dispatching calls).

   function Get_RC_Offset (T : Tag) return SSE.Storage_Offset;
   --  Return the Offset of the implicit record controller when the object
   --  has controlled components; otherwise return cero.

   pragma Export (Ada, Get_RC_Offset, "ada__tags__get_rc_offset");
   --  This procedure is used in s-finimp to compute the deep routines
   --  it is exported manually in order to avoid changing completely the
   --  organization of the run time.

   procedure Inherit_DT (Old_T : Tag; New_T : Tag; Entry_Count : Natural);
   --  Entry point used to initialize the DT of a type knowing the tag
   --  of the direct ancestor and the number of primitive ops that are
   --  inherited (Entry_Count).

   procedure Inherit_TSD (Old_Tag : Tag; New_Tag : Tag);
   --  Initialize the TSD of a type knowing the tag of the direct ancestor

   function Parent_Size
     (Obj : System.Address;
      T   : Tag) return SSE.Storage_Count;
   --  Computes the size the ancestor part of a tagged extension object whose
   --  address is 'obj' by calling indirectly the ancestor _size function. The
   --  ancestor is the parent of the type represented by tag T. This function
   --  assumes that _size is always in slot one of the dispatch table.

   pragma Export (Ada, Parent_Size, "ada__tags__parent_size");
   --  This procedure is used in s-finimp and is thus exported manually

   procedure Set_Access_Level (T : Tag; Value : Natural);
   --  Sets the accessibility level of the tagged type associated with T
   --  in its TSD.

   procedure Set_Expanded_Name (T : Tag; Value : System.Address);
   --  Set the address of the string containing the expanded name in the
   --  Dispatch table.

   procedure Set_External_Tag (T : Tag; Value : System.Address);
   --  Set the address of the string containing the external tag in the
   --  Dispatch table.

   procedure Set_Predefined_Prim_Op_Address
     (T        : Tag;
      Position : Positive;
      Value    : System.Address);
   --  Given a pointer to a dispatch Table (T) and a position in the dispatch
   --  table associated with a predefined primitive operation, put the address
   --  of the virtual function in it (used for overriding).

   procedure Set_Prim_Op_Address
     (T        : Tag;
      Position : Positive;
      Value    : System.Address);
   --  Given a pointer to a dispatch Table (T) and a position in the dispatch
   --  Table put the address of the virtual function in it (used for
   --  overriding).

   procedure Set_RC_Offset (T : Tag; Value : SSE.Storage_Offset);
   --  Sets the Offset of the implicit record controller when the object
   --  has controlled components. Set to O otherwise.

   procedure Set_Remotely_Callable (T : Tag; Value : Boolean);
   --  Set to true if the type has been declared in a context described
   --  in E.4 (18).

   procedure Set_TSD (T : Tag; Value : System.Address);
   --  Given a pointer T to a dispatch Table, stores the address of the record
   --  containing the Type Specific Data generated by GNAT.

   function TSD (T : Tag) return Type_Specific_Data_Ptr;
   --  Given a pointer T to a dispatch Table, retrieves the address of the
   --  record containing the Type Specific Data generated by GNAT.

   DT_Prologue_Size : constant SSE.Storage_Count :=
                        SSE.Storage_Count
                          ((Default_Prim_Op_Count + 1) *
                            (Standard'Address_Size / System.Storage_Unit));
   --  Size of the hidden part of the dispatch table. It contains the table of
   --  predefined primitive operations plus the address of the TSD.

   DT_Typeinfo_Ptr_Size : constant SSE.Storage_Count :=
                            SSE.Storage_Count
                              (1 * (Standard'Address_Size /
                                      System.Storage_Unit));
   --  Size of the Typeinfo_Ptr field of the Dispatch Table

   DT_Entry_Size : constant SSE.Storage_Count :=
                     SSE.Storage_Count
                       (1 * (Standard'Address_Size / System.Storage_Unit));
   --  Size of each primitive operation entry in the Dispatch Table

   TSD_Prologue_Size : constant SSE.Storage_Count :=
                         SSE.Storage_Count
                           (7 * (Standard'Address_Size /
                                   System.Storage_Unit));
   --  Size of the first part of the type specific data

   TSD_Entry_Size : constant SSE.Storage_Count :=
                      SSE.Storage_Count
                        (1 * (Standard'Address_Size / System.Storage_Unit));
   --  Size of each ancestor tag entry in the TSD

   type Address_Array is array (Natural range <>) of System.Address;
   pragma Suppress (Index_Check, On => Address_Array);
   --  The reason we suppress index checks is that in the body, objects
   --  of this type are declared with a dummy size of 1, the actual size
   --  depending on the number of primitive operations.

   --  Unchecked Conversions

   type Addr_Ptr is access System.Address;
   type Tag_Ptr  is access Tag;

   function To_Addr_Ptr is
      new Unchecked_Conversion (System.Address, Addr_Ptr);

   function To_Type_Specific_Data_Ptr is
     new Unchecked_Conversion (System.Address, Type_Specific_Data_Ptr);

   function To_Address is
     new Unchecked_Conversion (Tag, System.Address);

   function To_Address is
     new Unchecked_Conversion (Type_Specific_Data_Ptr, System.Address);

   function To_Tag is
     new Unchecked_Conversion (System.Address, Tag);

   --  Primitive dispatching operations are always inlined, to facilitate
   --  use in a minimal/no run-time environment for high integrity use.

   pragma Inline_Always (CW_Membership);
   pragma Inline_Always (Get_Access_Level);
   pragma Inline_Always (Get_Predefined_Prim_Op_Address);
   pragma Inline_Always (Get_Prim_Op_Address);
   pragma Inline_Always (Get_RC_Offset);
   pragma Inline_Always (Inherit_DT);
   pragma Inline_Always (Inherit_TSD);
   pragma Inline_Always (Set_Access_Level);
   pragma Inline_Always (Set_Expanded_Name);
   pragma Inline_Always (Set_External_Tag);
   pragma Inline_Always (Set_Predefined_Prim_Op_Address);
   pragma Inline_Always (Set_Prim_Op_Address);
   pragma Inline_Always (Set_RC_Offset);
   pragma Inline_Always (Set_Remotely_Callable);
   pragma Inline_Always (Set_TSD);
   pragma Inline_Always (TSD);

end Ada.Tags;
