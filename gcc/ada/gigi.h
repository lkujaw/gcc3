/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                                 G I G I                                  *
 *                                                                          *
 *                              C Header File                               *
 *                                                                          *
 *          Copyright (C) 1992-2006 Free Software Foundation, Inc.          *
 *                                                                          *
 * GNAT is free software;  you can  redistribute it  and/or modify it under *
 * terms of the  GNU General Public License as published  by the Free Soft- *
 * ware  Foundation;  either version 2,  or (at your option) any later ver- *
 * sion.  GNAT is distributed in the hope that it will be useful, but WITH- *
 * OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License *
 * for  more details.  You should have  received  a copy of the GNU General *
 * Public License  distributed with GNAT;  see file COPYING.  If not, write *
 * to  the Free Software Foundation,  59 Temple Place - Suite 330,  Boston, *
 * MA 02111-1307, USA.                                                      *
 *                                                                          *
 * GNAT was originally developed  by the GNAT team at  New York University. *
 * Extensive contributions were provided by Ada Core Technologies Inc.      *
 *                                                                          *
 ****************************************************************************/

/* The largest alignment, in bits, that is needed for using the widest
   move instruction.  */
extern unsigned int largest_move_alignment;

/* Declare all functions and types used by gigi.  */

/* See if DECL has an RTL that is indirect via a pseudo-register or a
   memory location and replace it with an indirect reference if so.
   This improves the debugger's ability to display the value.  */
extern void adjust_decl_rtl (tree);

/* Search the chain of currently reachable declarations for a builtin
   FUNCTION_DECL node corresponding to function NAME (an IDENTIFIER_NODE).
   Return the first node found, if any, or NULL_TREE otherwise.  */
extern tree builtin_decl_for (tree);

/* Record the current code position in GNAT_NODE.  */
extern void record_code_position (Node_Id);

/* Insert the code for GNAT_NODE at the position saved for that node.  */
extern void insert_code_for (Node_Id);

/* Compute the alignment of the largest mode that can be used for copying
   objects.  */
extern void gnat_compute_largest_alignment (void);

/* Routine called by gcc for emitting a stack check. GNU_EXPR is the
   expression that contains the last address on the stack to check. */
extern tree emit_stack_check (tree);

/* Make a TRANSFORM_EXPR to later expand GNAT_NODE into code.  */
extern tree make_transform_expr (Node_Id);

/* Update the setjmp buffer BUF with the current stack pointer.  We assume
   here that a __builtin_setjmp was done to BUF.  */
extern void update_setjmp_buf (tree);

/* GNU_TYPE is a type. Determine if it should be passed by reference by
   default.  */
extern int default_pass_by_ref (tree);

/* GNU_TYPE is the type of a subprogram parameter.  Determine from the type
   if it should be passed by reference.  */
extern int must_pass_by_ref (tree);

/* Elaboration routines for the front end.  */
extern void elab_all_gnat (void);

/* Initialize DUMMY_NODE_TABLE.  */
extern void init_dummy_type (void);

/* Given GNAT_ENTITY, a GNAT defining identifier node, which denotes some Ada
   entity, this routine returns the equivalent GCC tree for that entity
   (an ..._DECL node) and associates the ..._DECL node with the input GNAT
   defining identifier.

   If GNAT_ENTITY is a variable or a constant declaration, GNU_EXPR gives its
   initial value (in GCC tree form). This is optional for variables.
   For renamed entities, GNU_EXPR gives the object being renamed.

   DEFINITION is nonzero if this call is intended for a definition.  This is
   used for separate compilation where it necessary to know whether an
   external declaration or a definition should be created if the GCC equivalent
   was not created previously.  The value of 1 is normally used for a non-zero
   DEFINITION, but a value of 2 is used in special circumstances, defined in
   the code.  */
extern tree gnat_to_gnu_entity (Entity_Id, tree, int);

/* Similar, but if the returned value is a COMPONENT_REF, return the
   FIELD_DECL.  */
extern tree gnat_to_gnu_field_decl (Entity_Id);

/* Given GNAT_ENTITY, an entity in the incoming GNAT tree, return a
   GCC type corresponding to that entity.  GNAT_ENTITY is assumed to
   refer to an Ada type.  */
extern tree gnat_to_gnu_type (Entity_Id);

/* Given GNAT_ENTITY, elaborate all expressions that are required to
   be elaborated at the point of its definition, but do nothing else.  */
extern void elaborate_entity (Entity_Id);

/* Mark GNAT_ENTITY as going out of scope at this point.  Recursively mark
   any entities on its entity chain similarly.  */
extern void mark_out_of_scope (Entity_Id);

/* Make a dummy type corresponding to GNAT_TYPE.  */
extern tree make_dummy_type (Entity_Id);

/* Get the unpadded version of a GNAT type.  */
extern tree get_unpadded_type (Entity_Id);

/* Called when we need to protect a variable object using a save_expr.  */
extern tree maybe_variable (tree, Node_Id);

/* Create a record type that contains a field of TYPE with a starting bit
   position so that it is aligned to ALIGN bits.  */
/* Create a record type that contains a field of TYPE with a starting bit
   position so that it is aligned to ALIGN bits and is SIZE bytes long.  */
extern tree make_aligning_type (tree, int, tree);

/* Given a GNU tree and a GNAT list of choices, generate an expression to test
   the value passed against the list of choices.  */
extern tree choices_to_gnu (tree, Node_Id);

/* Given a type T, a FIELD_DECL F, and a replacement value R,
   return a new type with all size expressions that contain F
   updated by replacing F with R.  This is identical to GCC's
   substitute_in_type except that it knows about TYPE_INDEX_TYPE.  */
extern tree gnat_substitute_in_type (tree, tree, tree);

/* Return the "RM size" of GNU_TYPE.  This is the actual number of bits
   needed to represent the object.  */
extern tree rm_size (tree);

/* Given GNU_ID, an IDENTIFIER_NODE containing a name and SUFFIX, a
   string, return a new IDENTIFIER_NODE that is the concatenation of
   the name in GNU_ID and SUFFIX.  */
extern tree concat_id_with_name (tree, const char *);

/* Return the name to be used for GNAT_ENTITY.  If a type, create a
   fully-qualified name, possibly with type information encoding.
   Otherwise, return the name.  */
extern tree get_entity_name (Entity_Id);

/* Return a name for GNAT_ENTITY concatenated with two underscores and
   SUFFIX.  */
extern tree create_concat_name (Entity_Id, const char *);

/* Flag indicating whether file names are discarded in exception messages */
extern int discard_file_names;

/* If true, then gigi is being called on an analyzed but unexpanded
   tree, and the only purpose of the call is to properly annotate
   types with representation information */
extern int type_annotate_only;

/* Current file name without path */
extern const char *ref_filename;

/* List of TREE_LIST nodes representing a block stack.  TREE_VALUE
   of each gives the variable used for the setjmp buffer in the current
   block, if any.  */
extern GTY(()) tree gnu_block_stack;

/* This is the main program of the back-end.  It sets up all the table
   structures and then generates code.  */

extern void gigi (Node_Id, int, int, struct Node *, Node_Id *, Node_Id *,
		  struct Elist_Header *, struct Elmt_Item *,
		  struct String_Entry *, Char_Code *, struct List_Header *,
		  Int, char *, Entity_Id, Entity_Id, Entity_Id, Int);

/* This function is the driver of the GNAT to GCC tree transformation process.
   GNAT_NODE is the root of some gnat tree.  It generates code for that
   part of the tree.  */
extern void gnat_to_code (Node_Id);

/* GNAT_NODE is the root of some GNAT tree.  Return the root of the
   GCC tree corresponding to that GNAT tree.  Normally, no code is generated;
   we just return an equivalent tree which is used elsewhere to generate
   code.  */
extern tree gnat_to_gnu (Node_Id);

/* GNU_STMT is a statement.  We generate code for that statement.  */
extern void gnat_expand_stmt (tree);

/* Do the processing for the declaration of a GNAT_ENTITY, a type.  If
   a separate Freeze node exists, delay the bulk of the processing.  Otherwise
   make a GCC type for GNAT_ENTITY and set up the correspondance.  */

extern void process_type (Entity_Id);

/* Determine the input_filename and the input_line from the source location
   (Sloc) of GNAT_NODE node.  Set the global variable input_filename and
   input_line.  If WRITE_NOTE_P is true, emit a line number note. */
extern void set_lineno (Node_Id, int);

/* Likewise, but passed a Sloc.  */
extern void set_lineno_from_sloc (Source_Ptr, int);

/* Post an error message.  MSG is the error message, properly annotated.
   NODE is the node at which to post the error and the node to use for the
   "&" substitution.  */
extern void post_error (const char *, Node_Id);

/* Similar, but NODE is the node at which to post the error and ENT
   is the node to use for the "&" substitution.  */
extern void post_error_ne (const char *, Node_Id, Entity_Id);

/* Similar, but NODE is the node at which to post the error, ENT is the node
   to use for the "&" substitution, and N is the number to use for the ^.  */
extern void post_error_ne_num (const char *, Node_Id, Entity_Id, int);

/* Similar to post_error_ne_num, but T is a GCC tree representing the number
   to write.  If the tree represents a constant that fits within a
   host integer, the text inside curly brackets in MSG will be output
   (presumably including a '^').  Otherwise that text will not be output
   and the text inside square brackets will be output instead.  */
extern void post_error_ne_tree (const char *, Node_Id, Entity_Id, tree);

/* Similar to post_error_ne_tree, except that NUM is a second
   integer to write in the message.  */
extern void post_error_ne_tree_2 (const char *, Node_Id, Entity_Id, tree, int);

/* Set the node for a second '&' in the error message.  */
extern void set_second_error_entity (Entity_Id);

/* Protect EXP from multiple evaluation.  This may make a SAVE_EXPR.  */
extern tree protect_multiple_eval (tree);

/* Signal abort, with "Gigi abort" as the error label, and error_gnat_node
   as the relevant node that provides the location info for the error.
   The single parameter CODE is an integer code that is included in the
   additional error message generated. */
extern void gigi_abort (int) ATTRIBUTE_NORETURN;

/* Initialize the table that maps GNAT codes to GCC codes for simple
   binary and unary operations.  */
extern void init_code_table (void);

/* Current node being treated, in case gigi_abort or Check_Elaboration_Code
   called.  */
extern Node_Id error_gnat_node;

/* This is equivalent to stabilize_reference in GCC's tree.c, but we know how
   to handle our new nodes and we take extra arguments.

   FORCE says whether to force evaluation of everything,

   SUCCESS we set to true unless we walk through something we don't
   know how to stabilize, or through something which is not an lvalue
   and LVALUES_ONLY is true, in which cases we set to false.  */
extern tree maybe_stabilize_reference (tree, int, int, int *);

/* Wrapper around maybe_stabilize_reference, for common uses without
   lvalue restrictions and without need to examine the success
   indication.  */
extern tree gnat_stabilize_reference (tree, int);

/* Highest number in the front-end node table.  */
extern int max_gnat_nodes;

/* If nonzero, pretend we are allocating at global level.  */
extern int force_global;

/* Standard data type sizes.  Most of these are not used.  */

#ifndef CHAR_TYPE_SIZE
#define CHAR_TYPE_SIZE BITS_PER_UNIT
#endif

#ifndef SHORT_TYPE_SIZE
#define SHORT_TYPE_SIZE (BITS_PER_UNIT * MIN ((UNITS_PER_WORD + 1) / 2, 2))
#endif

#ifndef INT_TYPE_SIZE
#define INT_TYPE_SIZE BITS_PER_WORD
#endif

#ifndef LONG_TYPE_SIZE
#define LONG_TYPE_SIZE BITS_PER_WORD
#endif

#ifndef LONG_LONG_TYPE_SIZE
#define LONG_LONG_TYPE_SIZE (BITS_PER_WORD * 2)
#endif

#ifndef FLOAT_TYPE_SIZE
#define FLOAT_TYPE_SIZE BITS_PER_WORD
#endif

#ifndef DOUBLE_TYPE_SIZE
#define DOUBLE_TYPE_SIZE (BITS_PER_WORD * 2)
#endif

#ifndef LONG_DOUBLE_TYPE_SIZE
#define LONG_DOUBLE_TYPE_SIZE (BITS_PER_WORD * 2)
#endif

/* The choice of SIZE_TYPE here is very problematic.  We need a signed
   type whose bit width is Pmode.  Assume "long" is such a type here.  */
#undef SIZE_TYPE
#define SIZE_TYPE "long int"


/* Data structures used to represent attributes.  */

enum attr_type
{
  ATTR_MACHINE_ATTRIBUTE,
  ATTR_LINK_ALIAS,
  ATTR_LINK_SECTION,
  ATTR_LINK_CONSTRUCTOR,
  ATTR_LINK_DESTRUCTOR,
  ATTR_WEAK_EXTERNAL
};

struct attrib
{
  struct attrib *next;
  enum attr_type type;
  tree name;
  tree args;
  Node_Id error_point;
};

/* Define the entries in the standard data array.  */
enum standard_datatypes
{
/* Various standard data types and nodes.  */
  ADT_longest_float_type,
  ADT_void_type_decl,

  /* The type of an exception.  */
  ADT_except_type,

  /* Type declaration node  <==> typedef void *T */
  ADT_ptr_void_type,

  /* Function type declaration -- void T() */
  ADT_void_ftype,

  /* Type declaration node  <==> typedef void *T() */
  ADT_ptr_void_ftype,

  /* A function declaration node for a run-time function for allocating memory.
     Ada allocators cause calls to this function to be generated.   */
  ADT_malloc_decl,

  /* Likewise for freeing memory.  */
  ADT_free_decl,

  /* Types and decls used by our temporary exception mechanism.  See
     init_gigi_decls for details.  */
  ADT_jmpbuf_type,
  ADT_jmpbuf_ptr_type,
  ADT_get_jmpbuf_decl,
  ADT_set_jmpbuf_decl,
  ADT_get_excptr_decl,
  ADT_setjmp_decl,
  ADT_longjmp_decl,
  ADT_raise_nodefer_decl,
  ADT_begin_handler_decl,
  ADT_end_handler_decl,
  ADT_others_decl,
  ADT_all_others_decl,
  ADT_LAST};

extern GTY(()) tree gnat_std_decls[(int) ADT_LAST];
extern GTY(()) tree gnat_raise_decls[(int) LAST_REASON_CODE + 1];

extern GTY(()) tree static_ctors;
extern GTY(()) tree static_dtors;

#define longest_float_type_node gnat_std_decls[(int) ADT_longest_float_type]
#define void_type_decl_node gnat_std_decls[(int) ADT_void_type_decl]
#define except_type_node gnat_std_decls[(int) ADT_except_type]
#define ptr_void_type_node gnat_std_decls[(int) ADT_ptr_void_type]
#define void_ftype gnat_std_decls[(int) ADT_void_ftype]
#define ptr_void_ftype gnat_std_decls[(int) ADT_ptr_void_ftype]
#define malloc_decl gnat_std_decls[(int) ADT_malloc_decl]
#define free_decl gnat_std_decls[(int) ADT_free_decl]
#define jmpbuf_type gnat_std_decls[(int) ADT_jmpbuf_type]
#define jmpbuf_ptr_type gnat_std_decls[(int) ADT_jmpbuf_ptr_type]
#define get_jmpbuf_decl gnat_std_decls[(int) ADT_get_jmpbuf_decl]
#define set_jmpbuf_decl gnat_std_decls[(int) ADT_set_jmpbuf_decl]
#define get_excptr_decl gnat_std_decls[(int) ADT_get_excptr_decl]
#define setjmp_decl gnat_std_decls[(int) ADT_setjmp_decl]
#define longjmp_decl gnat_std_decls[(int) ADT_longjmp_decl]
#define raise_nodefer_decl gnat_std_decls[(int) ADT_raise_nodefer_decl]
#define begin_handler_decl gnat_std_decls[(int) ADT_begin_handler_decl]
#define others_decl gnat_std_decls[(int) ADT_others_decl]
#define all_others_decl gnat_std_decls[(int) ADT_all_others_decl]
#define end_handler_decl gnat_std_decls[(int) ADT_end_handler_decl]

/* Routines expected by the gcc back-end. They must have exactly the same
   prototype and names as below.  */

/* Returns non-zero if we are currently in the global binding level       */
extern int global_bindings_p (void);

/* Returns the list of declarations in the current level. Note that this list
   is in reverse order (it has to be so for back-end compatibility).  */
extern tree getdecls (void);

/* Nonzero if the current level needs to have a BLOCK made.  */
extern int kept_level_p (void);

/* Enter a new binding level. The input parameter is ignored, but has to be
   specified for back-end compatibility.  */
extern void pushlevel (int);

/* Exit a binding level.
   Pop the level off, and restore the state of the identifier-decl mappings
   that were in effect when this level was entered.

   If KEEP is nonzero, this level had explicit declarations, so
   and create a "block" (a BLOCK node) for the level
   to record its declarations and subblocks for symbol table output.

   If FUNCTIONBODY is nonzero, this level is the body of a function,
   so create a block as if KEEP were set and also clear out all
   label names.

   If REVERSE is nonzero, reverse the order of decls before putting
   them into the BLOCK.  */
extern tree poplevel (int, int, int);

/* Insert BLOCK at the end of the list of subblocks of the
   current binding level.  This is used when a BIND_EXPR is expanded,
   to handle the BLOCK node inside the BIND_EXPR.  */
extern void insert_block (tree);

/* Set the BLOCK node for the innermost scope
   (the one we are currently in).  */
extern void set_block (tree);

/* Records a ..._DECL node DECL as belonging to the current lexical scope.
   Returns the ..._DECL node. */
extern tree pushdecl (tree);

/* Create the predefined scalar types such as `integer_type_node' needed
   in the gcc back-end and initialize the global binding level.  */
extern void gnat_init_decl_processing (void);
extern void init_gigi_decls (tree, tree);
extern void gnat_init_gcc_eh (void);

/* Return an integer type with the number of bits of precision given by
   PRECISION.  UNSIGNEDP is nonzero if the type is unsigned; otherwise
   it is a signed type.  */
extern tree gnat_type_for_size (unsigned, int);

/* Return a data type that has machine mode MODE.  UNSIGNEDP selects
   an unsigned type; otherwise a signed type is returned.  */
extern tree gnat_type_for_mode (enum machine_mode, int);

/* Return the unsigned version of a TYPE_NODE, a scalar type.  */
extern tree gnat_unsigned_type (tree);

/* Return the signed version of a TYPE_NODE, a scalar type.  */
extern tree gnat_signed_type (tree);

/* Return a type the same as TYPE except unsigned or signed according to
   UNSIGNEDP.  */
extern tree gnat_signed_or_unsigned_type (int, tree);

/* Create an expression whose value is that of EXPR,
   converted to type TYPE.  The TREE_TYPE of the value
   is always TYPE.  This function implements all reasonable
   conversions; callers should filter out those that are
   not permitted by the language being compiled.  */
extern tree convert (tree, tree);

/* Routines created solely for the tree translator's sake. Their prototypes
   can be changed as desired.  */

/* GNAT_ENTITY is a GNAT tree node for a defining identifier.
   GNU_DECL is the GCC tree which is to be associated with
   GNAT_ENTITY. Such gnu tree node is always an ..._DECL node.
   If NO_CHECK is nonzero, the latter check is suppressed.
   If GNU_DECL is zero, a previous association is to be reset.  */
extern void save_gnu_tree (Entity_Id, tree, int);

/* GNAT_ENTITY is a GNAT tree node for a defining identifier.
   Return the ..._DECL node that was associated with it.  If there is no tree
   node associated with GNAT_ENTITY, abort.  */
extern tree get_gnu_tree (Entity_Id);

/* Return nonzero if a GCC tree has been associated with GNAT_ENTITY.  */
extern int present_gnu_tree (Entity_Id);

/* Initialize tables for above routines.  */
extern void init_gnat_to_gnu (void);

/* Given a record type (RECORD_TYPE) and a chain of FIELD_DECL
   nodes (FIELDLIST), finish constructing the record or union type.
   If HAS_REP is nonzero, this record has a rep clause; don't call
   layout_type but merely set the size and alignment ourselves.
   If DEFER_DEBUG is nonzero, do not call the debugging routines
   on this type; it will be done later. */
extern void finish_record_type (tree, tree, int, int);

/*  Output the debug information associated to a record type.  */
extern void write_record_type_debug_info (tree);

/* Returns a FUNCTION_TYPE node. RETURN_TYPE is the type returned by the
   subprogram. If it is void_type_node, then we are dealing with a procedure,
   otherwise we are dealing with a function. PARAM_DECL_LIST is a list of
   PARM_DECL nodes that are the subprogram arguments.  CICO_LIST is the
   copy-in/copy-out list to be stored into TYPE_CI_CO_LIST.
   RETURNS_UNCONSTRAINED is nonzero if the function returns an unconstrained
   object.  RETURNS_BY_REF is nonzero if the function returns by reference.
   RETURNS_WITH_DSP is nonzero if the function is to return with a
   depressed stack pointer.  */
extern tree create_subprog_type (tree, tree, tree, int, int, int);

/* Return a copy of TYPE, but safe to modify in any way.  */
extern tree copy_type (tree);

/* Return an INTEGER_TYPE of SIZETYPE with range MIN to MAX and whose
   TYPE_INDEX_TYPE is INDEX.  */
extern tree create_index_type (tree, tree, tree);

/* Return a TYPE_DECL node. TYPE_NAME gives the name of the type (a character
   string) and TYPE is a ..._TYPE node giving its data type.
   ARTIFICIAL_P is nonzero if this is a declaration that was generated
   by the compiler.  DEBUG_INFO_P is nonzero if we need to write debugging
   information about this type.  */
extern tree create_type_decl (tree, tree, struct attrib *, int, int);

/* Returns a GCC VAR_DECL or CONST_DECL node.

   VAR_NAME gives the name of the variable.  ASM_NAME is its assembler name
   (if provided).  TYPE is its data type (a GCC ..._TYPE node).  VAR_INIT is
   the GCC tree for an optional initial expression; NULL_TREE if none.

   CONST_FLAG is nonzero if this variable is constant.

   PUBLIC_FLAG is nonzero if this definition is to be made visible outside of
   the current compilation unit. This flag should be set when processing the
   variable definitions in a package specification.  EXTERN_FLAG is nonzero
   when processing an external variable declaration (as opposed to a
   definition: no storage is to be allocated for the variable here).
   STATIC_FLAG is only relevant when not at top level.  In that case
   it indicates whether to always allocate storage to the variable.  */
extern tree create_var_decl (tree, tree, tree, tree, int, int, int, int,
			     struct attrib *, Node_Id);

/* Similar to create_var_decl, forcing the creation of a VAR_DECL node.  */
extern tree create_true_var_decl (tree, tree, tree, tree, int, int, int, int,
				  struct attrib *, Node_Id);

/* Given a DECL and ATTR_LIST, apply the listed attributes.  */
extern void process_attributes (tree, struct attrib *);

/* Obtain any pending elaborations and clear the old list.  */
extern tree get_pending_elaborations (void);

/* Return nonzero if there are pending elaborations.  */
extern int pending_elaborations_p (void);

/* Save a copy of the current pending elaboration list and make a new
   one.  */
extern void push_pending_elaborations (void);

/* Pop the stack of pending elaborations.  */
extern void pop_pending_elaborations (void);

/* Return the current position in pending_elaborations so we can insert
   elaborations after that point.  */
extern tree get_elaboration_location (void);

/* Insert the current elaborations after ELAB, which is in some elaboration
   list.  */
extern void insert_elaboration_list (tree);

/* Add some pending elaborations to the current list.  */
extern void add_pending_elaborations (tree, tree, Node_Id);

/* Obtain any global renaming pointers and clear the old list.  */
extern tree get_global_renaming_pointers (void);

/* Add one global renaming pointer on the list.  */
extern void add_global_renaming_pointer (tree);

/* Returns a FIELD_DECL node. FIELD_NAME the field name, FIELD_TYPE is its
   type, and RECORD_TYPE is the type of the parent.  PACKED is nonzero if
   this field is in a record type with a "pragma pack".  If SIZE is nonzero
   it is the specified size for this field.  If POS is nonzero, it is the bit
   position.  If ADDRESSABLE is nonzero, it means we are allowed to take
   the address of this field for aliasing purposes.  */
extern tree create_field_decl (tree, tree, tree, int, tree, tree, int);

/* Returns a PARM_DECL node. PARAM_NAME is the name of the parameter,
   PARAM_TYPE is its type.  READONLY is nonzero if the parameter is
   readonly (either an IN parameter or an address of a pass-by-ref
   parameter). */
extern tree create_param_decl (tree, tree, int);

/* Returns a FUNCTION_DECL node.  SUBPROG_NAME is the name of the subprogram,
   ASM_NAME is its assembler name, SUBPROG_TYPE is its type (a FUNCTION_TYPE
   node), PARAM_DECL_LIST is the list of the subprogram arguments (a list of
   PARM_DECL nodes chained through the TREE_CHAIN field).

   INLINE_FLAG, PUBLIC_FLAG, and EXTERN_FLAG are used to set the appropriate
   fields in the FUNCTION_DECL.  GNAT_NODE gives the location.  */
extern tree create_subprog_decl (tree, tree, tree, tree, int, int, int,
				 struct attrib *, Node_Id);

/* Returns a LABEL_DECL node for LABEL_NAME.  */
extern tree create_label_decl (tree);

/* Set up the framework for generating code for SUBPROG_DECL, a subprogram
   body. This routine needs to be invoked before processing the declarations
   appearing in the subprogram.  */
extern void begin_subprog_body (tree, Node_Id);

/* Finish the definition of the current subprogram and compile it all the way
   to assembler language output.  */
extern void end_subprog_body (void);

/* Build a template of type TEMPLATE_TYPE from the array bounds of ARRAY_TYPE.
   EXPR is an expression that we can use to locate any PLACEHOLDER_EXPRs.
   Return a constructor for the template.  */
extern tree build_template (tree, tree, tree);

/* Build a VMS descriptor from a Mechanism_Type, which must specify
   a descriptor type, and the GCC type of an object.  Each FIELD_DECL
   in the type contains in its DECL_INITIAL the expression to use when
   a constructor is made for the type.  GNAT_ENTITY is a gnat node used
   to print out an error message if the mechanism cannot be applied to
   an object of that type and also for the name.  */

extern tree build_vms_descriptor (tree, Mechanism_Type, Entity_Id);

/* Build a type to be used to represent an aliased object whose nominal
   type is an unconstrained array.  This consists of a RECORD_TYPE containing
   a field of TEMPLATE_TYPE and a field of OBJECT_TYPE, which is an
   ARRAY_TYPE.  If ARRAY_TYPE is that of the unconstrained array, this
   is used to represent an arbitrary unconstrained object.  Use NAME
   as the name of the record.  */
extern tree build_unc_object_type (tree, tree, tree);

/* Same as build_unc_object_type, but taking a thin or fat pointer type
   instead of the template type. */
extern tree build_unc_object_type_from_ptr (tree, tree, tree);

/* Update anything previously pointing to OLD_TYPE to point to NEW_TYPE.  In
   the normal case this is just two adjustments, but we have more to do
   if NEW is an UNCONSTRAINED_ARRAY_TYPE.  */
extern void update_pointer_to (tree, tree);

/* EXP is an expression for the size of an object.  If this size contains
   discriminant references, replace them with the maximum (if MAX_P) or
   minimum (if ! MAX_P) possible value of the discriminant.  */
extern tree max_size (tree, int);

/* Remove all conversions that are done in EXP.  This includes converting
   from a padded type or to a left-justified modular type.  If TRUE_ADDRESS
   is nonzero, always return the address of the containing object even if
   the address is not bit-aligned.  */
extern tree remove_conversions (tree, int);

/* If EXP's type is an UNCONSTRAINED_ARRAY_TYPE, return an expression that
   refers to the underlying array.  If its type has TYPE_CONTAINS_TEMPLATE_P,
   likewise return an expression pointing to the underlying array.  */
extern tree maybe_unconstrained_array (tree);

/* Return an expression that does an unchecked converstion of EXPR to TYPE.
   If NOTRUNC_P is set, truncation operations should be suppressed.  */
extern tree unchecked_convert (tree, tree, int);

/* Prepare expr to be an argument of a TRUTH_NOT_EXPR or other logical
   operation.

   This preparation consists of taking the ordinary
   representation of an expression expr and producing a valid tree
   boolean expression describing whether expr is nonzero.  We could
   simply always do build_binary_op (NE_EXPR, expr, integer_zero_node, 1),
   but we optimize comparisons, &&, ||, and !.

   The resulting type should always be the same as the input type.
   This function is simpler than the corresponding C version since
   the only possible operands will be things of Boolean type.  */
extern tree gnat_truthvalue_conversion (tree);

/* Return the base type of TYPE.  */
extern tree get_base_type (tree);

/* EXP is a GCC tree representing an address.  See if we can find how
   strictly the object at that address is aligned.   Return that alignment
   strictly the object at that address is aligned.   Return that alignment
   in bits.  If we don't know anything about the alignment, return 0.  */
extern unsigned int known_alignment (tree);

/* Return true if VALUE is a multiple of FACTOR. FACTOR must be a power
   of 2. */
extern int value_factor_p (tree, int);

/* Make a binary operation of kind OP_CODE.  RESULT_TYPE is the type
   desired for the result.  Usually the operation is to be performed
   in that type.  For MODIFY_EXPR and ARRAY_REF, RESULT_TYPE may be 0
   in which case the type to be used will be derived from the operands.  */
extern tree build_binary_op (enum tree_code, tree, tree, tree);

/* Similar, but make unary operation.   */
extern tree build_unary_op (enum tree_code, tree, tree);

/* Similar, but for COND_EXPR.  */
extern tree build_cond_expr (tree, tree, tree, tree);

/* Build a CALL_EXPR to call FUNDECL with one argument, ARG.  Return
   the CALL_EXPR.  */
extern tree build_call_1_expr (tree, tree);

/* Build a CALL_EXPR to call FUNDECL with two argument, ARG1 & ARG2.  Return
   the CALL_EXPR.  */
extern tree build_call_2_expr (tree, tree, tree);

/* Likewise to call FUNDECL with no arguments.  */
extern tree build_call_0_expr (tree);

/* Call a function that raises an exception and pass the line number and file
   name, if requested.  MSG says which exception function to call.

   GNAT_NODE is the gnat node conveying the source location for which
   the error should be signaled, or Empty in which case the error is
   signaled on the current ref_file_name/input_line.  */
extern tree build_call_raise (int, Node_Id);

/* Return a CONSTRUCTOR of TYPE whose list is LIST.  This is not the
   same as build_constructor in the language-independent tree.c.  */
extern tree gnat_build_constructor (tree, tree);

/* Return a COMPONENT_REF to access a field that is given by COMPONENT,
   an IDENTIFIER_NODE giving the name of the field, FIELD, a FIELD_DECL,
   for the field, or both.  Don't fold the result if NO_FOLD_P.  */
extern tree build_component_ref (tree, tree, tree, int);

/* Build a GCC tree to call an allocation or deallocation function.
   If GNU_OBJ is nonzero, it is an object to deallocate.  Otherwise,
   genrate an allocator.

   GNU_SIZE is the size of the object and ALIGN is the alignment.
   GNAT_PROC, if present is a procedure to call and GNAT_POOL is the
   storage pool to use.  If not preset, malloc and free will be used.  */
extern tree build_call_alloc_dealloc (tree, tree, int, Entity_Id,
				      Entity_Id, Node_Id);

/* Build a GCC tree to correspond to allocating an object of TYPE whose
   initial value if INIT, if INIT is nonzero.  Convert the expression to
   RESULT_TYPE, which must be some type of pointer.  Return the tree.
   GNAT_PROC and GNAT_POOL optionally give the procedure to call and
   the storage pool to use.  GNAT_NODE is used to provide an error
   location for restriction violations messages.  If IGNORE_INIT_TYPE is
   true, ignore the type of INIT for the purpose of determining the size;
   this will cause the maximum size to be allocated if TYPE is of
   self-referential size.  */
extern tree build_allocator (tree, tree, tree, Entity_Id, Entity_Id, Node_Id,
			     int);

/* Fill in a VMS descriptor for EXPR and return a constructor for it.
   GNAT_FORMAL is how we find the descriptor record.  */

extern tree fill_vms_descriptor (tree, Entity_Id);

/* Indicate that we need to make the address of EXPR_NODE and it therefore
   should not be allocated in a register.  Return true if successful.  */
extern bool gnat_mark_addressable (tree);

/* This function is called by the front end to enumerate all the supported
   modes for the machine.  We pass a function which is called back with
   the following integer parameters:

   FLOAT_P	nonzero if this represents a floating-point mode
   COMPLEX_P	nonzero is this represents a complex mode
   COUNT	count of number of items, nonzero for vector mode
   PRECISION	number of bits in data representation
   MANTISSA	number of bits in mantissa, if FP and known, else zero.
   SIZE		number of bits used to store data
   ALIGN	number of bits to which mode is aligned.  */
extern void enumerate_modes (void (*f) (int, int, int, int, int, int,
					unsigned int));

/* These are temporary function to deal with recent GCC changes related to
   FP type sizes and precisions.  */
extern int fp_prec_to_size (int);
extern int fp_size_to_prec (int);

/* These functions return the basic data type sizes and related parameters
   about the target machine.  */

extern Pos get_target_bits_per_unit (void);
extern Pos get_target_bits_per_word (void);
extern Pos get_target_char_size (void);
extern Pos get_target_wchar_t_size (void);
extern Pos get_target_short_size (void);
extern Pos get_target_int_size (void);
extern Pos get_target_long_size (void);
extern Pos get_target_long_long_size (void);
extern Pos get_target_float_size (void);
extern Pos get_target_double_size (void);
extern Pos get_target_long_double_size (void);
extern Pos get_target_pointer_size (void);
extern Pos get_target_maximum_alignment (void);
extern Nat get_float_words_be (void);
extern Nat get_words_be (void);
extern Nat get_bytes_be (void);
extern Nat get_bits_be (void);
extern Nat get_strict_alignment (void);
