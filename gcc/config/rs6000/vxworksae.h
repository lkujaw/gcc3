/* Target overrides for VxWorks AE653 platform.  */

/* This platform supports the probing method of stack checking and
   requires 4K of space for executing a possible last chance handler.  */
#define STACK_CHECK_PROTECT 4096

/* Define this to be nonzero if static stack checking is supported.  */
#define STACK_CHECK_STATIC_BUILTIN 1
