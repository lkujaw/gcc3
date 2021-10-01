------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                              T R E E P R S                               --
--                                                                          --
--                                 S p e c                                  --
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
-- GNAT was originally developed  by the GNAT team at  New York University. --
-- Extensive contributions were provided by Ada Core Technologies Inc.      --
--                                                                          --
------------------------------------------------------------------------------

--  This package contains the declaration of the string used by the Tree_Print
--  package. It must be updated whenever the arrangements of the field names
--  in package Sinfo is changed. The utility program XTREEPRS is used to
--  do this update correctly using the template treeprs.adt as input.

with Sinfo; use Sinfo;

package Treeprs is

   --------------------------------
   -- String Data for Node Print --
   --------------------------------

   --  String data for print out. The Pchars array is a long string with the
   --  the entry for each node type consisting of a single blank, followed by
   --  a series of entries, one for each Op or Flag field used for the node.
   --  Each entry has a single character which identifies the field, followed
   --  by the synonym name. The starting location for a given node type is
   --  found from the corresponding entry in the Pchars_Pos_Array.

   --  The following characters identify the field. These are characters which
   --  could never occur in a field name, so they also mark the end of the
   --  previous name.

   subtype Fchar is Character range '#' .. '9';

   F_Field1     : constant Fchar := '#'; -- Character'Val (16#23#)
   F_Field2     : constant Fchar := '$'; -- Character'Val (16#24#)
   F_Field3     : constant Fchar := '%'; -- Character'Val (16#25#)
   F_Field4     : constant Fchar := '&'; -- Character'Val (16#26#)
   F_Field5     : constant Fchar := '''; -- Character'Val (16#27#)
   F_Flag1      : constant Fchar := '('; -- Character'Val (16#28#)
   F_Flag2      : constant Fchar := ')'; -- Character'Val (16#29#)
   F_Flag3      : constant Fchar := '*'; -- Character'Val (16#2A#)
   F_Flag4      : constant Fchar := '+'; -- Character'Val (16#2B#)
   F_Flag5      : constant Fchar := ','; -- Character'Val (16#2C#)
   F_Flag6      : constant Fchar := '-'; -- Character'Val (16#2D#)
   F_Flag7      : constant Fchar := '.'; -- Character'Val (16#2E#)
   F_Flag8      : constant Fchar := '/'; -- Character'Val (16#2F#)
   F_Flag9      : constant Fchar := '0'; -- Character'Val (16#30#)
   F_Flag10     : constant Fchar := '1'; -- Character'Val (16#31#)
   F_Flag11     : constant Fchar := '2'; -- Character'Val (16#32#)
   F_Flag12     : constant Fchar := '3'; -- Character'Val (16#33#)
   F_Flag13     : constant Fchar := '4'; -- Character'Val (16#34#)
   F_Flag14     : constant Fchar := '5'; -- Character'Val (16#35#)
   F_Flag15     : constant Fchar := '6'; -- Character'Val (16#36#)
   F_Flag16     : constant Fchar := '7'; -- Character'Val (16#37#)
   F_Flag17     : constant Fchar := '8'; -- Character'Val (16#38#)
   F_Flag18     : constant Fchar := '9'; -- Character'Val (16#39#)

   --  Note this table does not include entity field and flags whose access
   --  functions are in Einfo (these are handled by the Print_Entity_Info
   --  procedure in Treepr, which uses the routines in Einfo to get the proper
   --  symbolic information). In addition, the following fields are handled by
   --  Treepr, and do not appear in the Pchars array:

   --    Analyzed
   --    Cannot_Be_Constant
   --    Chars
   --    Comes_From_Source
   --    Error_Posted
   --    Etype
   --    Is_Controlling_Actual
   --    Is_Overloaded
   --    Is_Static_Expression
   --    Left_Opnd
   --    Must_Check_Expr
   --    Must_Not_Freeze
   --    No_Overflow_Expr
   --    Paren_Count
   --    Raises_Constraint_Error
   --    Right_Opnd

   Pchars : constant String :=
      --  Unused_At_Start
      "" &
      --  At_Clause
      "#Identifier%Expression" &
      --  Component_Clause
      "#Component_Name$Position%First_Bit&Last_Bit" &
      --  Enumeration_Representation_Clause
      "#Identifier%Array_Aggregate'Next_Rep_Item" &
      --  Mod_Clause
      "%Expression&Pragmas_Before" &
      --  Record_Representation_Clause
      "#Identifier$Mod_Clause%Component_Clauses'Next_Rep_Item" &
      --  Attribute_Definition_Clause
      "$Name%Expression&Entity'Next_Rep_Item+From_At_Mod2Check_Address_Align" &
         "ment" &
      --  Empty
      "" &
      --  Pragma
      "$Pragma_Argument_Associations%Debug_Statement'Next_Rep_Item" &
      --  Pragma_Argument_Association
      "%Expression" &
      --  Error
      "" &
      --  Defining_Character_Literal
      "$Next_Entity%Scope" &
      --  Defining_Identifier
      "$Next_Entity%Scope" &
      --  Defining_Operator_Symbol
      "$Next_Entity%Scope" &
      --  Expanded_Name
      "%Prefix$Selector_Name&Entity&Associated_Node4Redundant_Use2Has_Privat" &
         "e_View" &
      --  Identifier
      "&Entity&Associated_Node$Original_Discriminant4Redundant_Use2Has_Priva" &
         "te_View" &
      --  Operator_Symbol
      "%Strval&Entity&Associated_Node2Has_Private_View" &
      --  Character_Literal
      "$Char_Literal_Value&Entity&Associated_Node2Has_Private_View" &
      --  Op_Add
      "" &
      --  Op_Concat
      "4Is_Component_Left_Opnd5Is_Component_Right_Opnd" &
      --  Op_Expon
      "4Is_Power_Of_2_For_Shift" &
      --  Op_Subtract
      "" &
      --  Op_Divide
      "5Treat_Fixed_As_Integer4Do_Division_Check9Rounded_Result" &
      --  Op_Mod
      "5Treat_Fixed_As_Integer4Do_Division_Check" &
      --  Op_Multiply
      "5Treat_Fixed_As_Integer9Rounded_Result" &
      --  Op_Rem
      "5Treat_Fixed_As_Integer4Do_Division_Check" &
      --  Op_And
      "+Do_Length_Check" &
      --  Op_Eq
      "" &
      --  Op_Ge
      "" &
      --  Op_Gt
      "" &
      --  Op_Le
      "" &
      --  Op_Lt
      "" &
      --  Op_Ne
      "" &
      --  Op_Or
      "+Do_Length_Check" &
      --  Op_Xor
      "+Do_Length_Check" &
      --  Op_Rotate_Left
      "+Shift_Count_OK" &
      --  Op_Rotate_Right
      "+Shift_Count_OK" &
      --  Op_Shift_Left
      "+Shift_Count_OK" &
      --  Op_Shift_Right
      "+Shift_Count_OK" &
      --  Op_Shift_Right_Arithmetic
      "+Shift_Count_OK" &
      --  Op_Abs
      "" &
      --  Op_Minus
      "" &
      --  Op_Not
      "" &
      --  Op_Plus
      "" &
      --  Attribute_Reference
      "%Prefix$Attribute_Name#Expressions&Entity&Associated_Node8Do_Overflow" &
         "_Check4Redundant_Use5Must_Be_Byte_Aligned" &
      --  And_Then
      "#Actions" &
      --  Conditional_Expression
      "#Expressions$Then_Actions%Else_Actions" &
      --  Explicit_Dereference
      "%Prefix&Actual_Designated_Subtype" &
      --  Function_Call
      "$Name%Parameter_Associations&First_Named_Actual#Controlling_Argument4" &
         "Do_Tag_Check5No_Elaboration_Check8Parameter_List_Truncated9ABE_Is_" &
         "Certain" &
      --  In
      "" &
      --  Indexed_Component
      "%Prefix#Expressions" &
      --  Integer_Literal
      "$Original_Entity%Intval4Print_In_Hex" &
      --  Not_In
      "" &
      --  Null
      "" &
      --  Or_Else
      "#Actions" &
      --  Procedure_Call_Statement
      "$Name%Parameter_Associations&First_Named_Actual#Controlling_Argument4" &
         "Do_Tag_Check5No_Elaboration_Check8Parameter_List_Truncated9ABE_Is_" &
         "Certain" &
      --  Qualified_Expression
      "&Subtype_Mark%Expression" &
      --  Raise_Constraint_Error
      "#Condition%Reason" &
      --  Raise_Program_Error
      "#Condition%Reason" &
      --  Raise_Storage_Error
      "#Condition%Reason" &
      --  Aggregate
      "#Expressions$Component_Associations8Null_Record_Present%Aggregate_Bou" &
         "nds&Associated_Node+Static_Processing_OK9Compile_Time_Known_Aggreg" &
         "ate2Expansion_Delayed" &
      --  Allocator
      "%Expression2Null_Exclusion_Present#Storage_Pool$Procedure_To_Call4No_" &
         "Initialization8Do_Storage_Check" &
      --  Extension_Aggregate
      "%Ancestor_Part&Associated_Node#Expressions$Component_Associations8Nul" &
         "l_Record_Present2Expansion_Delayed" &
      --  Range
      "#Low_Bound$High_Bound2Includes_Infinities" &
      --  Real_Literal
      "$Original_Entity%Realval&Corresponding_Integer_Value2Is_Machine_Numbe" &
         "r" &
      --  Reference
      "%Prefix" &
      --  Selected_Component
      "%Prefix$Selector_Name&Associated_Node4Do_Discriminant_Check2Is_In_Dis" &
         "criminant_Check" &
      --  Slice
      "%Prefix&Discrete_Range" &
      --  String_Literal
      "%Strval2Has_Wide_Character" &
      --  Subprogram_Info
      "#Identifier" &
      --  Type_Conversion
      "&Subtype_Mark%Expression4Do_Tag_Check+Do_Length_Check8Do_Overflow_Che" &
         "ck2Float_Truncate9Rounded_Result5Conversion_OK" &
      --  Unchecked_Expression
      "%Expression" &
      --  Unchecked_Type_Conversion
      "&Subtype_Mark%Expression2Kill_Range_Check8No_Truncation" &
      --  Subtype_Indication
      "&Subtype_Mark%Constraint/Must_Not_Freeze" &
      --  Component_Declaration
      "#Defining_Identifier&Component_Definition%Expression,More_Ids-Prev_Id" &
         "s" &
      --  Entry_Declaration
      "#Defining_Identifier&Discrete_Subtype_Definition%Parameter_Specificat" &
         "ions'Corresponding_Body5Must_Override6Must_Not_Override" &
      --  Formal_Object_Declaration
      "#Defining_Identifier6In_Present8Out_Present2Null_Exclusion_Present&Su" &
         "btype_Mark%Access_Definition'Default_Expression,More_Ids-Prev_Ids" &
      --  Formal_Type_Declaration
      "#Defining_Identifier%Formal_Type_Definition&Discriminant_Specificatio" &
         "ns4Unknown_Discriminants_Present" &
      --  Full_Type_Declaration
      "#Defining_Identifier&Discriminant_Specifications%Type_Definition2Disc" &
         "r_Check_Funcs_Built" &
      --  Incomplete_Type_Declaration
      "#Defining_Identifier&Discriminant_Specifications4Unknown_Discriminant" &
         "s_Present6Tagged_Present" &
      --  Loop_Parameter_Specification
      "#Defining_Identifier6Reverse_Present&Discrete_Subtype_Definition" &
      --  Object_Declaration
      "#Defining_Identifier+Aliased_Present8Constant_Present2Null_Exclusion_" &
         "Present&Object_Definition%Expression$Handler_List_Entry'Correspond" &
         "ing_Generic_Association,More_Ids-Prev_Ids4No_Initialization6Assign" &
         "ment_OK.Exception_Junk5Delay_Finalize_Attach7Is_Subprogram_Descrip" &
         "tor" &
      --  Protected_Type_Declaration
      "#Defining_Identifier&Discriminant_Specifications$Interface_List%Prote" &
         "cted_Definition'Corresponding_Body" &
      --  Private_Extension_Declaration
      "#Defining_Identifier&Discriminant_Specifications4Unknown_Discriminant" &
         "s_Present+Abstract_Present8Limited_Present'Subtype_Indication$Inte" &
         "rface_List" &
      --  Private_Type_Declaration
      "#Defining_Identifier&Discriminant_Specifications4Unknown_Discriminant" &
         "s_Present+Abstract_Present6Tagged_Present8Limited_Present" &
      --  Subtype_Declaration
      "#Defining_Identifier2Null_Exclusion_Present'Subtype_Indication&Generi" &
         "c_Parent_Type.Exception_Junk" &
      --  Function_Specification
      "#Defining_Unit_Name$Elaboration_Boolean%Parameter_Specifications2Null" &
         "_Exclusion_Present&Result_Definition'Generic_Parent5Must_Override6" &
         "Must_Not_Override" &
      --  Procedure_Specification
      "#Defining_Unit_Name$Elaboration_Boolean%Parameter_Specifications'Gene" &
         "ric_Parent4Null_Present5Must_Override6Must_Not_Override" &
      --  Access_Function_Definition
      "2Null_Exclusion_Present-Protected_Present%Parameter_Specifications&Re" &
         "sult_Definition" &
      --  Access_Procedure_Definition
      "2Null_Exclusion_Present-Protected_Present%Parameter_Specifications" &
      --  Task_Type_Declaration
      "#Defining_Identifier&Discriminant_Specifications$Interface_List%Task_" &
         "Definition'Corresponding_Body" &
      --  Package_Body_Stub
      "#Defining_Identifier&Library_Unit'Corresponding_Body" &
      --  Protected_Body_Stub
      "#Defining_Identifier&Library_Unit'Corresponding_Body" &
      --  Subprogram_Body_Stub
      "#Specification&Library_Unit'Corresponding_Body" &
      --  Task_Body_Stub
      "#Defining_Identifier&Library_Unit'Corresponding_Body" &
      --  Function_Instantiation
      "#Defining_Unit_Name$Name%Generic_Associations&Parent_Spec'Instance_Sp" &
         "ec5Must_Override6Must_Not_Override9ABE_Is_Certain" &
      --  Procedure_Instantiation
      "#Defining_Unit_Name$Name&Parent_Spec%Generic_Associations'Instance_Sp" &
         "ec5Must_Override6Must_Not_Override9ABE_Is_Certain" &
      --  Package_Instantiation
      "#Defining_Unit_Name$Name%Generic_Associations&Parent_Spec'Instance_Sp" &
         "ec9ABE_Is_Certain" &
      --  Package_Body
      "#Defining_Unit_Name$Declarations&Handled_Statement_Sequence'Correspon" &
         "ding_Spec4Was_Originally_Stub" &
      --  Subprogram_Body
      "#Specification$Declarations&Handled_Statement_Sequence%Activation_Cha" &
         "in_Entity'Corresponding_Spec+Acts_As_Spec6Bad_Is_Detected8Do_Stora" &
         "ge_Check-Has_Priority_Pragma.Is_Protected_Subprogram_Body,Is_Task_" &
         "Master4Was_Originally_Stub" &
      --  Protected_Body
      "#Defining_Identifier$Declarations&End_Label'Corresponding_Spec4Was_Or" &
         "iginally_Stub" &
      --  Task_Body
      "#Defining_Identifier$Declarations&Handled_Statement_Sequence,Is_Task_" &
         "Master%Activation_Chain_Entity'Corresponding_Spec4Was_Originally_S" &
         "tub" &
      --  Implicit_Label_Declaration
      "#Defining_Identifier$Label_Construct" &
      --  Package_Declaration
      "#Specification'Corresponding_Body&Parent_Spec%Activation_Chain_Entity" &
      --  Single_Task_Declaration
      "#Defining_Identifier$Interface_List%Task_Definition" &
      --  Subprogram_Declaration
      "#Specification%Body_To_Inline'Corresponding_Body&Parent_Spec" &
      --  Use_Package_Clause
      "$Names%Next_Use_Clause&Hidden_By_Use_Clause" &
      --  Generic_Package_Declaration
      "#Specification'Corresponding_Body$Generic_Formal_Declarations&Parent_" &
         "Spec%Activation_Chain_Entity" &
      --  Generic_Subprogram_Declaration
      "#Specification'Corresponding_Body$Generic_Formal_Declarations&Parent_" &
         "Spec" &
      --  Constrained_Array_Definition
      "$Discrete_Subtype_Definitions&Component_Definition" &
      --  Unconstrained_Array_Definition
      "$Subtype_Marks&Component_Definition" &
      --  Exception_Renaming_Declaration
      "#Defining_Identifier$Name" &
      --  Object_Renaming_Declaration
      "#Defining_Identifier2Null_Exclusion_Present&Subtype_Mark%Access_Defin" &
         "ition$Name'Corresponding_Generic_Association" &
      --  Package_Renaming_Declaration
      "#Defining_Unit_Name$Name&Parent_Spec" &
      --  Subprogram_Renaming_Declaration
      "#Specification$Name&Parent_Spec'Corresponding_Spec%Corresponding_Form" &
         "al_Spec-From_Default" &
      --  Generic_Function_Renaming_Declaration
      "#Defining_Unit_Name$Name&Parent_Spec" &
      --  Generic_Package_Renaming_Declaration
      "#Defining_Unit_Name$Name&Parent_Spec" &
      --  Generic_Procedure_Renaming_Declaration
      "#Defining_Unit_Name$Name&Parent_Spec" &
      --  Abort_Statement
      "$Names" &
      --  Accept_Statement
      "#Entry_Direct_Name'Entry_Index%Parameter_Specifications&Handled_State" &
         "ment_Sequence$Declarations" &
      --  Assignment_Statement
      "$Name%Expression4Do_Tag_Check+Do_Length_Check,Forwards_OK-Backwards_O" &
         "K.No_Ctrl_Actions" &
      --  Asynchronous_Select
      "#Triggering_Alternative$Abortable_Part" &
      --  Block_Statement
      "#Identifier$Declarations&Handled_Statement_Sequence,Is_Task_Master%Ac" &
         "tivation_Chain_Entity6Has_Created_Identifier-Is_Task_Allocation_Bl" &
         "ock.Is_Asynchronous_Call_Block" &
      --  Case_Statement
      "%Expression&Alternatives'End_Span" &
      --  Code_Statement
      "%Expression" &
      --  Conditional_Entry_Call
      "#Entry_Call_Alternative&Else_Statements" &
      --  Delay_Relative_Statement
      "%Expression" &
      --  Delay_Until_Statement
      "%Expression" &
      --  Entry_Call_Statement
      "$Name%Parameter_Associations&First_Named_Actual" &
      --  Free_Statement
      "%Expression#Storage_Pool$Procedure_To_Call&Actual_Designated_Subtype" &
      --  Goto_Statement
      "$Name.Exception_Junk" &
      --  Loop_Statement
      "#Identifier$Iteration_Scheme%Statements&End_Label6Has_Created_Identif" &
         "ier7Is_Null_Loop" &
      --  Null_Statement
      "" &
      --  Raise_Statement
      "$Name%Expression" &
      --  Requeue_Statement
      "$Name6Abort_Present" &
      --  Return_Statement
      "'Return_Statement_Entity%Expression#Storage_Pool$Procedure_To_Call4Do" &
         "_Tag_Check,By_Ref9Comes_From_Extended_Return_Statement" &
      --  Extended_Return_Statement
      "'Return_Statement_Entity%Return_Object_Declarations&Handled_Statement" &
         "_Sequence#Storage_Pool$Procedure_To_Call4Do_Tag_Check,By_Ref" &
      --  Selective_Accept
      "#Select_Alternatives&Else_Statements" &
      --  Timed_Entry_Call
      "#Entry_Call_Alternative&Delay_Alternative" &
      --  Exit_Statement
      "$Name#Condition" &
      --  If_Statement
      "#Condition$Then_Statements%Elsif_Parts&Else_Statements'End_Span" &
      --  Accept_Alternative
      "$Accept_Statement#Condition%Statements&Pragmas_Before'Accept_Handler_" &
         "Records" &
      --  Delay_Alternative
      "$Delay_Statement#Condition%Statements&Pragmas_Before" &
      --  Elsif_Part
      "#Condition$Then_Statements%Condition_Actions" &
      --  Entry_Body_Formal_Part
      "&Entry_Index_Specification%Parameter_Specifications#Condition" &
      --  Iteration_Scheme
      "#Condition%Condition_Actions&Loop_Parameter_Specification" &
      --  Terminate_Alternative
      "#Condition&Pragmas_Before'Pragmas_After" &
      --  Formal_Abstract_Subprogram_Declaration
      "#Specification$Default_Name6Box_Present" &
      --  Formal_Concrete_Subprogram_Declaration
      "#Specification$Default_Name6Box_Present" &
      --  Abortable_Part
      "%Statements" &
      --  Abstract_Subprogram_Declaration
      "#Specification" &
      --  Access_Definition
      "2Null_Exclusion_Present6All_Present8Constant_Present&Subtype_Mark%Acc" &
         "ess_To_Subprogram_Definition" &
      --  Access_To_Object_Definition
      "6All_Present2Null_Exclusion_Present'Subtype_Indication8Constant_Prese" &
         "nt" &
      --  Case_Statement_Alternative
      "&Discrete_Choices%Statements" &
      --  Compilation_Unit
      "&Library_Unit#Context_Items6Private_Present$Unit'Aux_Decls_Node8Has_N" &
         "o_Elaboration_Code4Body_Required+Acts_As_Spec%First_Inlined_Subpro" &
         "gram" &
      --  Compilation_Unit_Aux
      "$Declarations#Actions'Pragmas_After&Config_Pragmas" &
      --  Component_Association
      "#Choices$Loop_Actions%Expression6Box_Present" &
      --  Component_Definition
      "+Aliased_Present2Null_Exclusion_Present'Subtype_Indication%Access_Def" &
         "inition" &
      --  Component_List
      "%Component_Items&Variant_Part4Null_Present" &
      --  Derived_Type_Definition
      "+Abstract_Present2Null_Exclusion_Present'Subtype_Indication%Record_Ex" &
         "tension_Part8Limited_Present,Task_Present-Protected_Present.Synchr" &
         "onized_Present$Interface_List7Interface_Present" &
      --  Decimal_Fixed_Point_Definition
      "%Delta_Expression$Digits_Expression&Real_Range_Specification" &
      --  Defining_Program_Unit_Name
      "$Name#Defining_Identifier" &
      --  Delta_Constraint
      "%Delta_Expression&Range_Constraint" &
      --  Designator
      "$Name#Identifier" &
      --  Digits_Constraint
      "$Digits_Expression&Range_Constraint" &
      --  Discriminant_Association
      "#Selector_Names%Expression" &
      --  Discriminant_Specification
      "#Defining_Identifier2Null_Exclusion_Present'Discriminant_Type%Express" &
         "ion,More_Ids-Prev_Ids" &
      --  Enumeration_Type_Definition
      "#Literals&End_Label" &
      --  Entry_Body
      "#Defining_Identifier'Entry_Body_Formal_Part$Declarations&Handled_Stat" &
         "ement_Sequence%Activation_Chain_Entity" &
      --  Entry_Call_Alternative
      "#Entry_Call_Statement%Statements&Pragmas_Before" &
      --  Entry_Index_Specification
      "#Defining_Identifier&Discrete_Subtype_Definition" &
      --  Exception_Declaration
      "#Defining_Identifier%Expression,More_Ids-Prev_Ids" &
      --  Exception_Handler
      "$Choice_Parameter&Exception_Choices%Statements,Zero_Cost_Handling" &
      --  Floating_Point_Definition
      "$Digits_Expression&Real_Range_Specification" &
      --  Formal_Decimal_Fixed_Point_Definition
      "" &
      --  Formal_Derived_Type_Definition
      "&Subtype_Mark6Private_Present+Abstract_Present8Limited_Present$Interf" &
         "ace_List" &
      --  Formal_Discrete_Type_Definition
      "" &
      --  Formal_Floating_Point_Definition
      "" &
      --  Formal_Modular_Type_Definition
      "" &
      --  Formal_Ordinary_Fixed_Point_Definition
      "" &
      --  Formal_Package_Declaration
      "#Defining_Identifier$Name%Generic_Associations6Box_Present'Instance_S" &
         "pec9ABE_Is_Certain" &
      --  Formal_Private_Type_Definition
      "+Abstract_Present6Tagged_Present8Limited_Present" &
      --  Formal_Signed_Integer_Type_Definition
      "" &
      --  Freeze_Entity
      "&Entity$Access_Types_To_Process%TSS_Elist#Actions'First_Subtype_Link" &
      --  Generic_Association
      "$Selector_Name#Explicit_Generic_Actual_Parameter6Box_Present" &
      --  Handled_Sequence_Of_Statements
      "%Statements&End_Label'Exception_Handlers#At_End_Proc$First_Real_State" &
         "ment,Zero_Cost_Handling" &
      --  Index_Or_Discriminant_Constraint
      "#Constraints" &
      --  Itype_Reference
      "#Itype" &
      --  Label
      "#Identifier.Exception_Junk" &
      --  Modular_Type_Definition
      "%Expression" &
      --  Number_Declaration
      "#Defining_Identifier%Expression,More_Ids-Prev_Ids" &
      --  Ordinary_Fixed_Point_Definition
      "%Delta_Expression&Real_Range_Specification" &
      --  Others_Choice
      "#Others_Discrete_Choices2All_Others" &
      --  Package_Specification
      "#Defining_Unit_Name$Visible_Declarations%Private_Declarations&End_Lab" &
         "el'Generic_Parent9Limited_View_Installed" &
      --  Parameter_Association
      "$Selector_Name%Explicit_Actual_Parameter&Next_Named_Actual" &
      --  Parameter_Specification
      "#Defining_Identifier6In_Present8Out_Present2Null_Exclusion_Present$Pa" &
         "rameter_Type%Expression4Do_Accessibility_Check,More_Ids-Prev_Ids'D" &
         "efault_Expression" &
      --  Protected_Definition
      "$Visible_Declarations%Private_Declarations&End_Label-Has_Priority_Pra" &
         "gma" &
      --  Range_Constraint
      "&Range_Expression" &
      --  Real_Range_Specification
      "#Low_Bound$High_Bound" &
      --  Record_Definition
      "&End_Label+Abstract_Present6Tagged_Present8Limited_Present#Component_" &
         "List4Null_Present,Task_Present-Protected_Present.Synchronized_Pres" &
         "ent7Interface_Present$Interface_List" &
      --  Signed_Integer_Type_Definition
      "#Low_Bound$High_Bound" &
      --  Single_Protected_Declaration
      "#Defining_Identifier$Interface_List%Protected_Definition" &
      --  Subunit
      "$Name#Proper_Body%Corresponding_Stub" &
      --  Task_Definition
      "$Visible_Declarations%Private_Declarations&End_Label-Has_Priority_Pra" &
         "gma,Has_Storage_Size_Pragma.Has_Task_Info_Pragma/Has_Task_Name_Pra" &
         "gma" &
      --  Triggering_Alternative
      "#Triggering_Statement%Statements&Pragmas_Before" &
      --  Use_Type_Clause
      "$Subtype_Marks%Next_Use_Clause&Hidden_By_Use_Clause" &
      --  Validate_Unchecked_Conversion
      "#Source_Type$Target_Type" &
      --  Variant
      "&Discrete_Choices#Component_List$Enclosing_Variant%Present_Expr'Dchec" &
         "k_Function" &
      --  Variant_Part
      "$Name#Variants" &
      --  With_Clause
      "$Name&Library_Unit'Corresponding_Spec,First_Name-Last_Name4Context_In" &
         "stalled+Elaborate_Present5Elaborate_All_Present0Elaborate_All_Desi" &
         "rable2Elaborate_Desirable6Private_Present7Implicit_With8Limited_Pr" &
         "esent9Limited_View_Installed.Unreferenced_In_Spec/No_Entities_Ref_" &
         "In_Spec" &
      --  With_Type_Clause
      "$Name6Tagged_Present" &
      --  Unused_At_End
      "";

   type Pchar_Pos_Array is array (Node_Kind) of Positive;
   Pchar_Pos : constant Pchar_Pos_Array := Pchar_Pos_Array'(
      N_Unused_At_Start                        => 1,
      N_At_Clause                              => 1,
      N_Component_Clause                       => 23,
      N_Enumeration_Representation_Clause      => 66,
      N_Mod_Clause                             => 107,
      N_Record_Representation_Clause           => 133,
      N_Attribute_Definition_Clause            => 187,
      N_Empty                                  => 260,
      N_Pragma                                 => 260,
      N_Pragma_Argument_Association            => 319,
      N_Error                                  => 330,
      N_Defining_Character_Literal             => 330,
      N_Defining_Identifier                    => 348,
      N_Defining_Operator_Symbol               => 366,
      N_Expanded_Name                          => 384,
      N_Identifier                             => 459,
      N_Operator_Symbol                        => 535,
      N_Character_Literal                      => 582,
      N_Op_Add                                 => 641,
      N_Op_Concat                              => 641,
      N_Op_Expon                               => 688,
      N_Op_Subtract                            => 712,
      N_Op_Divide                              => 712,
      N_Op_Mod                                 => 768,
      N_Op_Multiply                            => 809,
      N_Op_Rem                                 => 847,
      N_Op_And                                 => 888,
      N_Op_Eq                                  => 904,
      N_Op_Ge                                  => 904,
      N_Op_Gt                                  => 904,
      N_Op_Le                                  => 904,
      N_Op_Lt                                  => 904,
      N_Op_Ne                                  => 904,
      N_Op_Or                                  => 904,
      N_Op_Xor                                 => 920,
      N_Op_Rotate_Left                         => 936,
      N_Op_Rotate_Right                        => 951,
      N_Op_Shift_Left                          => 966,
      N_Op_Shift_Right                         => 981,
      N_Op_Shift_Right_Arithmetic              => 996,
      N_Op_Abs                                 => 1011,
      N_Op_Minus                               => 1011,
      N_Op_Not                                 => 1011,
      N_Op_Plus                                => 1011,
      N_Attribute_Reference                    => 1011,
      N_And_Then                               => 1121,
      N_Conditional_Expression                 => 1129,
      N_Explicit_Dereference                   => 1167,
      N_Function_Call                          => 1200,
      N_In                                     => 1342,
      N_Indexed_Component                      => 1342,
      N_Integer_Literal                        => 1361,
      N_Not_In                                 => 1397,
      N_Null                                   => 1397,
      N_Or_Else                                => 1397,
      N_Procedure_Call_Statement               => 1405,
      N_Qualified_Expression                   => 1547,
      N_Raise_Constraint_Error                 => 1571,
      N_Raise_Program_Error                    => 1588,
      N_Raise_Storage_Error                    => 1605,
      N_Aggregate                              => 1622,
      N_Allocator                              => 1778,
      N_Extension_Aggregate                    => 1878,
      N_Range                                  => 1981,
      N_Real_Literal                           => 2022,
      N_Reference                              => 2092,
      N_Selected_Component                     => 2099,
      N_Slice                                  => 2183,
      N_String_Literal                         => 2205,
      N_Subprogram_Info                        => 2231,
      N_Type_Conversion                        => 2242,
      N_Unchecked_Expression                   => 2357,
      N_Unchecked_Type_Conversion              => 2368,
      N_Subtype_Indication                     => 2423,
      N_Component_Declaration                  => 2463,
      N_Entry_Declaration                      => 2533,
      N_Formal_Object_Declaration              => 2657,
      N_Formal_Type_Declaration                => 2791,
      N_Full_Type_Declaration                  => 2892,
      N_Incomplete_Type_Declaration            => 2980,
      N_Loop_Parameter_Specification           => 3073,
      N_Object_Declaration                     => 3137,
      N_Protected_Type_Declaration             => 3407,
      N_Private_Extension_Declaration          => 3510,
      N_Private_Type_Declaration               => 3655,
      N_Subtype_Declaration                    => 3781,
      N_Function_Specification                 => 3878,
      N_Procedure_Specification                => 4030,
      N_Access_Function_Definition             => 4154,
      N_Access_Procedure_Definition            => 4238,
      N_Task_Type_Declaration                  => 4304,
      N_Package_Body_Stub                      => 4402,
      N_Protected_Body_Stub                    => 4454,
      N_Subprogram_Body_Stub                   => 4506,
      N_Task_Body_Stub                         => 4552,
      N_Function_Instantiation                 => 4604,
      N_Procedure_Instantiation                => 4722,
      N_Package_Instantiation                  => 4840,
      N_Package_Body                           => 4926,
      N_Subprogram_Body                        => 5024,
      N_Protected_Body                         => 5251,
      N_Task_Body                              => 5333,
      N_Implicit_Label_Declaration             => 5471,
      N_Package_Declaration                    => 5507,
      N_Single_Task_Declaration                => 5576,
      N_Subprogram_Declaration                 => 5627,
      N_Use_Package_Clause                     => 5687,
      N_Generic_Package_Declaration            => 5730,
      N_Generic_Subprogram_Declaration         => 5827,
      N_Constrained_Array_Definition           => 5900,
      N_Unconstrained_Array_Definition         => 5950,
      N_Exception_Renaming_Declaration         => 5985,
      N_Object_Renaming_Declaration            => 6010,
      N_Package_Renaming_Declaration           => 6123,
      N_Subprogram_Renaming_Declaration        => 6159,
      N_Generic_Function_Renaming_Declaration  => 6248,
      N_Generic_Package_Renaming_Declaration   => 6284,
      N_Generic_Procedure_Renaming_Declaration => 6320,
      N_Abort_Statement                        => 6356,
      N_Accept_Statement                       => 6362,
      N_Assignment_Statement                   => 6457,
      N_Asynchronous_Select                    => 6543,
      N_Block_Statement                        => 6581,
      N_Case_Statement                         => 6746,
      N_Code_Statement                         => 6779,
      N_Conditional_Entry_Call                 => 6790,
      N_Delay_Relative_Statement               => 6829,
      N_Delay_Until_Statement                  => 6840,
      N_Entry_Call_Statement                   => 6851,
      N_Free_Statement                         => 6898,
      N_Goto_Statement                         => 6966,
      N_Loop_Statement                         => 6986,
      N_Null_Statement                         => 7071,
      N_Raise_Statement                        => 7071,
      N_Requeue_Statement                      => 7087,
      N_Return_Statement                       => 7106,
      N_Extended_Return_Statement              => 7229,
      N_Selective_Accept                       => 7358,
      N_Timed_Entry_Call                       => 7394,
      N_Exit_Statement                         => 7435,
      N_If_Statement                           => 7450,
      N_Accept_Alternative                     => 7513,
      N_Delay_Alternative                      => 7589,
      N_Elsif_Part                             => 7641,
      N_Entry_Body_Formal_Part                 => 7685,
      N_Iteration_Scheme                       => 7746,
      N_Terminate_Alternative                  => 7803,
      N_Formal_Abstract_Subprogram_Declaration => 7842,
      N_Formal_Concrete_Subprogram_Declaration => 7881,
      N_Abortable_Part                         => 7920,
      N_Abstract_Subprogram_Declaration        => 7931,
      N_Access_Definition                      => 7945,
      N_Access_To_Object_Definition            => 8042,
      N_Case_Statement_Alternative             => 8113,
      N_Compilation_Unit                       => 8141,
      N_Compilation_Unit_Aux                   => 8280,
      N_Component_Association                  => 8330,
      N_Component_Definition                   => 8374,
      N_Component_List                         => 8450,
      N_Derived_Type_Definition                => 8492,
      N_Decimal_Fixed_Point_Definition         => 8674,
      N_Defining_Program_Unit_Name             => 8734,
      N_Delta_Constraint                       => 8759,
      N_Designator                             => 8793,
      N_Digits_Constraint                      => 8809,
      N_Discriminant_Association               => 8844,
      N_Discriminant_Specification             => 8870,
      N_Enumeration_Type_Definition            => 8960,
      N_Entry_Body                             => 8979,
      N_Entry_Call_Alternative                 => 9086,
      N_Entry_Index_Specification              => 9133,
      N_Exception_Declaration                  => 9181,
      N_Exception_Handler                      => 9230,
      N_Floating_Point_Definition              => 9295,
      N_Formal_Decimal_Fixed_Point_Definition  => 9338,
      N_Formal_Derived_Type_Definition         => 9338,
      N_Formal_Discrete_Type_Definition        => 9415,
      N_Formal_Floating_Point_Definition       => 9415,
      N_Formal_Modular_Type_Definition         => 9415,
      N_Formal_Ordinary_Fixed_Point_Definition => 9415,
      N_Formal_Package_Declaration             => 9415,
      N_Formal_Private_Type_Definition         => 9502,
      N_Formal_Signed_Integer_Type_Definition  => 9550,
      N_Freeze_Entity                          => 9550,
      N_Generic_Association                    => 9618,
      N_Handled_Sequence_Of_Statements         => 9678,
      N_Index_Or_Discriminant_Constraint       => 9770,
      N_Itype_Reference                        => 9782,
      N_Label                                  => 9788,
      N_Modular_Type_Definition                => 9814,
      N_Number_Declaration                     => 9825,
      N_Ordinary_Fixed_Point_Definition        => 9874,
      N_Others_Choice                          => 9916,
      N_Package_Specification                  => 9951,
      N_Parameter_Association                  => 10060,
      N_Parameter_Specification                => 10118,
      N_Protected_Definition                   => 10270,
      N_Range_Constraint                       => 10342,
      N_Real_Range_Specification               => 10359,
      N_Record_Definition                      => 10380,
      N_Signed_Integer_Type_Definition         => 10551,
      N_Single_Protected_Declaration           => 10572,
      N_Subunit                                => 10628,
      N_Task_Definition                        => 10664,
      N_Triggering_Alternative                 => 10802,
      N_Use_Type_Clause                        => 10849,
      N_Validate_Unchecked_Conversion          => 10900,
      N_Variant                                => 10924,
      N_Variant_Part                           => 11003,
      N_With_Clause                            => 11017,
      N_With_Type_Clause                       => 11291,
      N_Unused_At_End                          => 11311);

end Treeprs;
