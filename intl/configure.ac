AC_PREREQ(2.59)
AC_INIT
AC_CONFIG_SRCDIR(gettext.c)
AC_CONFIG_HEADER(config.h)
AM_GNU_GETTEXT_VERSION(0.12.1)
AM_GNU_GETTEXT

# This replaces the extensive use of DEFS in the original Makefile.in.
AC_DEFINE(IN_LIBINTL, 1, [Define because this is libintl.])
AC_DEFINE(IN_LIBRARY, 1, [Define because this is a library.])
AC_DEFINE(DEPENDS_ON_LIBICONV, 1, [Define because we depend on libiconv.])
AC_DEFINE(ENABLE_RELOCATABLE, 1, [Define to enable relocation.])
AC_DEFINE(NO_XMALLOC, 1, [Define if there is no xmalloc.])
AC_DEFINE(set_relocation_prefix, libintl_set_relocation_prefix,
[Define this entry point correctly.])
AC_DEFINE(relocate, libintl_relocate,
[Define this entry point correctly.])

# Additional info for config.intl.
AC_SUBST(LIBINTL_DEP)
AC_SUBST(INCINTL)

AC_PROG_CC_C_O
# autoconf is lame and doesn't give us any substitution variable for this.
if eval "test \"`echo '$ac_cv_prog_cc_'${ac_cc}_c_o`\" = no"; then
  NO_MINUS_C_MINUS_O=yes
else
  OUTPUT_OPTION='-o $@'
fi
AC_SUBST(NO_MINUS_C_MINUS_O)
AC_SUBST(OUTPUT_OPTION)

LIBINTL_DEP=
INCINTL=
case $USE_INCLUDED_LIBINTL in
  yes)
    LIBINTL_DEP='${top_builddir}/intl/libintl.a'
    INCINTL='-I${top_builddir}/intl'
    ;;
esac

AC_CONFIG_FILES(Makefile config.intl)
AC_OUTPUT
