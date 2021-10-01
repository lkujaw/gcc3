------------------------------------------------------------------------------
--                                                                          --
--                         GNAT RUN-TIME COMPONENTS                         --
--                                                                          --
--                             A D A . T A G S                              --
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

--  This is the HI-E version of this file. Some functionality has been
--  removed in order to simplify this run-time unit.

with System.Storage_Elements; use System.Storage_Elements;

package body Ada.Tags is

--  Structure of the GNAT Primary Dispatch Table

--           +----------------------+
--           |       table of       |
--           : predefined primitive :
--           |     ops pointers     |
--           +----------------------+
--           | Typeinfo_Ptr/TSD_Ptr ---> Type Specific Data
--  Tag ---> +----------------------+   +-------------------+
--           |       table of       |   | inheritance depth |
--           :    primitive ops     :   +-------------------+
--           |       pointers       |   |   access level    |
--           +----------------------+   +-------------------+
--                                      |   expanded name   |
--                                      +-------------------+
--                                      |   external tag    |
--                                      +-------------------+
--                                      |   hash table link |
--                                      +-------------------+
--                                      | remotely callable |
--                                      +-------------------+
--                                      | rec ctrler offset |
--                                      +-------------------+
--                                      | table of          |
--                                      :    ancestor       :
--                                      |       tags        |
--                                      +-------------------+

   subtype Cstring is String (Positive);
   type Cstring_Ptr is access all Cstring;

   --  We suppress index checks because the declared size in the record below
   --  is a dummy size of one (see below).

   type Tag_Table is array (Natural range <>) of Tag;
   pragma Suppress_Initialization (Tag_Table);
   pragma Suppress (Index_Check, On => Tag_Table);

   --  Type specific data types

   type Type_Specific_Data is record
      Idepth : Natural;
      --  Inheritance Depth Level: Used to implement the membership test
      --  associated with single inheritance of tagged types in constant-time.
      --  In addition it also indicates the size of the first table stored in
      --  the Tags_Table component (see comment below).

      Access_Level : Natural;
      --  Accessibility level required to give support to Ada 2005 nested type
      --  extensions. This feature allows safe nested type extensions by
      --  shifting the accessibility checks to certain operations, rather than
      --  being enforced at the type declaration. In particular, by performing
      --  run-time accessibility checks on class-wide allocators, class-wide
      --  function return, and class-wide stream I/O, the danger of objects
      --  outliving their type declaration can be eliminated (Ada 2005: AI-344)

      Expanded_Name : Cstring_Ptr;
      External_Tag  : Cstring_Ptr;
      HT_Link       : Tag;
      --  Components used to give support to the Ada.Tags subprograms described
      --  in ARM 3.9

      Remotely_Callable : Boolean;
      --  Used to check ARM E.4 (18)

      RC_Offset : SSE.Storage_Offset;
      --  Controller Offset: Used to give support to tagged controlled objects
      --  (see Get_Deep_Controller at s-finimp)

      Tags_Table : Tag_Table (0 .. 1);
      --  Table of ancestor tags. Its size actually depends on the inheritance
      --  depth level of the tagged type. Its size is generated by the compiler
      --  by means of the same mechanism as for the Prims_Ptr array in the
      --  Dispatch_Table record. See comments below on Prims_Ptr.
   end record;

   type Dispatch_Table is record
      --  Typeinfo_Ptr is stored just "before" the Prims_Ptr component.
      Prims_Ptr : Address_Array (1 .. 1);
      --  The size of the Prims_Ptr array actually depends on the tagged type
      --  to which it applies. For each tagged type, the expander computes the
      --  actual array size, allocates the Dispatch_Table record accordingly,
      --  and generates code that displaces the base of the record after the
      --  Typeinfo_Ptr component. For this reason the first two components have
      --  been commented in the previous declaration. The access to these
      --  components is done by means of local functions.
      --
      --  To avoid the use of discriminants to define the actual size of the
      --  dispatch table, we used to declare the tag as a pointer to a record
      --  that contains an arbitrary array of addresses, using Positive as its
      --  index. This ensures that there are never range checks when accessing
      --  the dispatch table, but it prevents GDB from displaying tagged types
      --  properly. A better approach is to declare this record type as holding
      --  small number of addresses, and to explicitly suppress checks on it.
      --
      --  Note that in both cases, this type is never allocated, and serves
      --  only to declare the corresponding access type.
   end record;

   ---------------------------------------------
   -- Unchecked Conversions for String Fields --
   ---------------------------------------------

   function To_Cstring_Ptr is
     new Unchecked_Conversion (System.Address, Cstring_Ptr);

   ------------------------------------------------
   -- Unchecked Conversions for other components --
   ------------------------------------------------

   type Acc_Size
     is access function (A : System.Address) return Long_Long_Integer;

   function To_Acc_Size is new Unchecked_Conversion (System.Address, Acc_Size);
   --  The profile of the implicitly defined _size primitive

   -----------------------
   -- Local Subprograms --
   -----------------------

   function Length (Str : Cstring_Ptr) return Natural;
   --  Length of string represented by the given pointer (treating the string
   --  as a C-style string, which is Nul terminated).

   -------------------
   -- CW_Membership --
   -------------------

   --  Canonical implementation of Classwide Membership corresponding to:

   --     Obj in Typ'Class

   --  Each dispatch table contains a reference to a table of ancestors (stored
   --  in the first part of the Tags_Table) and a count of the level of
   --  inheritance "Idepth".

   --  Obj is in Typ'Class if Typ'Tag is in the table of ancestors that are
   --  contained in the dispatch table referenced by Obj'Tag . Knowing the
   --  level of inheritance of both types, this can be computed in constant
   --  time by the formula:

   --   Obj'tag.TSD.Ancestor_Tags (Obj'tag.TSD.Idepth - Typ'tag.TSD.Idepth)
   --     = Typ'tag

   function CW_Membership (Obj_Tag : Tag; Typ_Tag : Tag) return Boolean is
      Pos : Integer;

   begin
      Pos := TSD (Obj_Tag).Idepth - TSD (Typ_Tag).Idepth;
      return Pos >= 0 and then TSD (Obj_Tag).Tags_Table (Pos) = Typ_Tag;
   end CW_Membership;

   -------------------
   -- Expanded_Name --
   -------------------

   function Expanded_Name (T : Tag) return String is
      Result : Cstring_Ptr;
   begin
      if T = No_Tag then
         raise Tag_Error;
      end if;

      Result := TSD (T).Expanded_Name;
      return Result (1 .. Length (Result));
   end Expanded_Name;

   ------------------
   -- External_Tag --
   ------------------

   function External_Tag (T : Tag) return String is
      Result : Cstring_Ptr;
   begin
      if T = No_Tag then
         raise Tag_Error;
      end if;

      Result := TSD (T).External_Tag;
      return Result (1 .. Length (Result));
   end External_Tag;

   ----------------------
   -- Get_Access_Level --
   ----------------------

   function Get_Access_Level (T : Tag) return Natural is
   begin
      return TSD (T).Access_Level;
   end Get_Access_Level;

   ------------------------------------
   -- Get_Predefined_Prim_Op_Address --
   ------------------------------------

   function Get_Predefined_Prim_Op_Address
     (T        : Tag;
      Position : Positive) return System.Address
   is
      Prim_Ops_DT : Tag;
   begin
      Prim_Ops_DT := To_Tag (To_Address (T) - DT_Prologue_Size);
      return Prim_Ops_DT.Prims_Ptr (Position);
   end Get_Predefined_Prim_Op_Address;

   -------------------------
   -- Get_Prim_Op_Address --
   -------------------------

   function Get_Prim_Op_Address
     (T        : Tag;
      Position : Positive) return System.Address
   is
   begin
      return T.Prims_Ptr (Position);
   end Get_Prim_Op_Address;

   -------------------
   -- Get_RC_Offset --
   -------------------

   function Get_RC_Offset (T : Tag) return SSE.Storage_Offset is
   begin
      return TSD (T).RC_Offset;
   end Get_RC_Offset;

   ----------------
   -- Inherit_DT --
   ----------------

   procedure Inherit_DT (Old_T : Tag; New_T : Tag; Entry_Count : Natural) is
      Old_T_Prim_Ops : Tag;
      New_T_Prim_Ops : Tag;
      Size           : Positive;

   begin
      if Old_T /= null then
         New_T.Prims_Ptr (1 .. Entry_Count) :=
           Old_T.Prims_Ptr (1 .. Entry_Count);
         Old_T_Prim_Ops := To_Tag (To_Address (Old_T) - DT_Prologue_Size);
         New_T_Prim_Ops := To_Tag (To_Address (New_T) - DT_Prologue_Size);
         Size := Default_Prim_Op_Count;
         New_T_Prim_Ops.Prims_Ptr (1 .. Size) :=
           Old_T_Prim_Ops.Prims_Ptr (1 .. Size);
      end if;
   end Inherit_DT;

   -----------------
   -- Inherit_TSD --
   -----------------

   procedure Inherit_TSD (Old_Tag : Tag; New_Tag : Tag) is
      New_TSD_Ptr : constant Type_Specific_Data_Ptr := TSD (New_Tag);
      Old_TSD_Ptr : Type_Specific_Data_Ptr;

   begin
      if Old_Tag /= null then
         Old_TSD_Ptr := TSD (Old_Tag);
         New_TSD_Ptr.Idepth := Old_TSD_Ptr.Idepth + 1;

         --  Copy the "table of ancestor tags" of the parent

         New_TSD_Ptr.Tags_Table (1 .. New_TSD_Ptr.Idepth) :=
           Old_TSD_Ptr.Tags_Table (0 .. Old_TSD_Ptr.Idepth);

      else
         New_TSD_Ptr.Idepth := 0;
      end if;

      New_TSD_Ptr.Tags_Table (0) := New_Tag;
   end Inherit_TSD;

   ------------
   -- Length --
   ------------

   function Length (Str : Cstring_Ptr) return Natural is
      Len : Integer := 1;

   begin
      while Str (Len) /= ASCII.Nul loop
         Len := Len + 1;
      end loop;

      return Len - 1;
   end Length;

   -----------------
   -- Parent_Size --
   -----------------

   function Parent_Size
     (Obj : System.Address;
      T   : Tag) return SSE.Storage_Count
   is
      Parent_Tag : constant Tag := TSD (T).Tags_Table (1);
      --  The tag of the parent type

      Prim_Ops_DT : constant Tag :=
                      To_Tag (To_Address (Parent_Tag) - DT_Prologue_Size);
      --  The table of primitive operations of the parent

      F : constant Acc_Size := To_Acc_Size (Prim_Ops_DT.Prims_Ptr (1));
      --  Access to the _size primitive of the parent. We assume that it is
      --  always in the first slot of the dispatch table.

   begin
      --  Compute the size of the _parent field of the object

      return SSE.Storage_Count (F.all (Obj));
   end Parent_Size;

   ----------------
   -- Parent_Tag --
   ----------------

   function Parent_Tag (T : Tag) return Tag is
   begin
      if T = No_Tag then
         raise Tag_Error;
      end if;

      --  The Parent_Tag of a root-level tagged type is defined to be No_Tag.
      --  The first entry in the Ancestors_Tags array will be null for such
      --  a type, but it's better to be explicit about returning No_Tag in
      --  this case.

      if TSD (T).Idepth = 0 then
         return No_Tag;
      else
         return TSD (T).Tags_Table (1);
      end if;
   end Parent_Tag;

   ----------------------
   -- Set_Access_Level --
   ----------------------

   procedure Set_Access_Level (T : Tag; Value : Natural) is
   begin
      TSD (T).Access_Level := Value;
   end Set_Access_Level;

   -----------------------
   -- Set_Expanded_Name --
   -----------------------

   procedure Set_Expanded_Name (T : Tag; Value : System.Address) is
   begin
      TSD (T).Expanded_Name := To_Cstring_Ptr (Value);
   end Set_Expanded_Name;

   ----------------------
   -- Set_External_Tag --
   ----------------------

   procedure Set_External_Tag (T : Tag; Value : System.Address) is
   begin
      TSD (T).External_Tag := To_Cstring_Ptr (Value);
   end Set_External_Tag;

   ------------------------------------
   -- Set_Predefined_Prim_Op_Address --
   ------------------------------------

   procedure Set_Predefined_Prim_Op_Address
     (T        : Tag;
      Position : Positive;
      Value    : System.Address)
   is
      Prim_Ops_DT : constant Tag := To_Tag (To_Address (T) - DT_Prologue_Size);

   begin
      Prim_Ops_DT.Prims_Ptr (Position) := Value;
   end Set_Predefined_Prim_Op_Address;

   -------------------------
   -- Set_Prim_Op_Address --
   -------------------------

   procedure Set_Prim_Op_Address
     (T        : Tag;
      Position : Positive;
      Value    : System.Address)
   is
   begin
      T.Prims_Ptr (Position) := Value;
   end Set_Prim_Op_Address;

   -------------------
   -- Set_RC_Offset --
   -------------------

   procedure Set_RC_Offset (T : Tag; Value : SSE.Storage_Offset) is
   begin
      TSD (T).RC_Offset := Value;
   end Set_RC_Offset;

   ---------------------------
   -- Set_Remotely_Callable --
   ---------------------------

   procedure Set_Remotely_Callable (T : Tag; Value : Boolean) is
   begin
      TSD (T).Remotely_Callable := Value;
   end Set_Remotely_Callable;

   -------------
   -- Set_TSD --
   -------------

   procedure Set_TSD (T : Tag; Value : System.Address) is
      TSD_Ptr : constant Addr_Ptr :=
                  To_Addr_Ptr (To_Address (T) - DT_Typeinfo_Ptr_Size);
   begin
      TSD_Ptr.all := Value;
   end Set_TSD;

   ---------
   -- TSD --
   ---------

   function TSD (T : Tag) return Type_Specific_Data_Ptr is
      TSD_Ptr : constant Addr_Ptr :=
                  To_Addr_Ptr (To_Address (T) - DT_Typeinfo_Ptr_Size);
   begin
      return To_Type_Specific_Data_Ptr (TSD_Ptr.all);
   end TSD;

end Ada.Tags;
