/* VMS linker wrapper.
   Copyright (C) 2005
   Free Software Foundation, Inc.
   Contributed by Douglas B. Rupp (rupp@gnat.com).

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* This program is a wrapper around the VMS linker.
   It translates Unix style command line options into corresponding
   VMS style qualifiers and then spawns the VMS linker.  */

/* #include "config.h" */
/* #include "system.h" */
/* #include "coretypes.h" */
/* #include "tm.h" */

#define _POSIX_EXIT 1

#ifdef IN_GCC
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include <ctype.h>
#else
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdlib.h>
#define xstrdup strdup
#define xmalloc malloc
#define xcalloc calloc
#define xrealloc realloc
#endif

#define LNM__STRING 2
#define LNM_C_NAMLENGTH 255
#define PSL_C_SUPER 2
#define PSL_C_USER 3

#undef PATH_SEPARATOR
#undef PATH_SEPARATOR_STR
#define PATH_SEPARATOR ','
#define PATH_SEPARATOR_STR ","

/* %s ~= $link /exe='exenam 'optnam./opt */

static char *remotelink_cmd=\
(char *) "\
$!set verify\n\
$ exenam = f$parse (p1,,, \"NAME\")\n\
$ exeext = f$parse (p1,,, \"TYPE\")\n\
$ optnam = p2\n\
$ inhibit = p3\n\
$ share = p4\n\
$ open/read optnamchan 'optnam\n\
$ optnamloop:\n\
$ read/end_of_file=optnamend optnamchan objfile\n\
$ if f$locate (\"cluster=\", objfile).eq.0\n\
$ then\n\
$   objfile=f$element(3,\",\",objfile)\n\
$ endif\n\
$ if f$getsyi (\"ARCH_TYPE\").eq.2\n\
$ then\n\
$   ext = f$parse (objfile,,, \"TYPE\")\n\
$   if (ext.eqs.\".OBJ\").or. (ext.eqs.\".O\")\n\
$   then\n\
$     set file/attr=(rfm=var,rat=none) 'objfile\n\
$   endif\n\
$ endif\n\
$ goto optnamloop\n\
$ optnamend:\n\
$ if f$search (\"''exenam'.exe\").nes.\"\"\n\
$ then\n\
$   del 'exenam.exe.0\n\
$ endif\n\
%s\n\
$ if $status\n\
$ then\n\
$!  Link sucessful\n\
$   if share.eq.0\n\
$   then\n\
$     if f$locate (\"V8\",f$getsyi (\"VERSION\")).eq.0\n\
$     then\n\
$       if f$trnlnm(\"GNAT$LD_NOCALL_DEBUG\").nes.\"\" then -\n\
          set image/flags=nocall_debug 'exenam\n\
$       if f$trnlnm(\"GNAT$LD_MKTHREADS\").nes.\"\" then -\n\
          set image/flags=mkthreads 'exenam\n\
$       if f$trnlnm(\"GNAT$LD_UPCALLS\").nes.\"\" then -\n\
          set image/flags=upcalls 'exenam\n\
$     else\n\
$       if f$trnlnm(\"GNAT$LD_NOCALL_DEBUG\").nes.\"\" then -\n\
          @gnu:[bin]set_exe 'exenam /nodebug\n\
$       if f$trnlnm(\"GNAT$LD_MKTHREADS\").nes.\"\" then -\n\
          threadcp /enable=multiple_kernel 'exenam\n\
$       if f$trnlnm(\"GNAT$LD_UPCALLS\").nes.\"\" then -\n\
          threadcp /enable=upcalls 'exenam\n\
$     endif\n\
$   endif\n\
$   del 'optnam.0\n\
$   define/user sys$output 'exenam.sh\n\
$   echo \"ssh %s \"\"mcr []''exenam'.exe\"\"\"\n\
$   set file/attr=(rfm=stmlf,rat=none) 'exenam.sh\n\
$   del 'exenam.sh.0\n\
$ else\n\
$!  Link failed\n\
$   if inhibit.eq.1 then del 'optnam.0, 'exenam.exe.0\n\
$ endif\n\
$ self = f$environment  (\"PROCEDURE\")\n\
$ del 'self\n\
";

/* Local variable declarations.  */
static char *rcp_target;
static char *rcp_target_username;
static int ld_nocall_debug = 0;
static int ld_mkthreads = 0;
static int ld_upcalls = 0;

/* Operation in cross remote mode */
#ifdef CROSS_COMPILE
static int remote = 1;
#else
static int remote = 0;
#endif

/* File specification for vms-dwarf2.o.  */
static char *vmsdwarf2spec = 0;

/* File specification for vms-dwarf2eh.o.  */
static char *vmsdwarf2ehspec = 0;

/* verbose = 1 if -v passed.  */
static int verbose = 0;

/* save_temps = 1 if -save-temps passed.  */
static int save_temps = 0;

/* By default don't generate executable file if there are errors
   in the link. Override with --noinhibit-exec.  */
static int inhibit_exec = 1;

/* debug = 1 if -g passed.  */
static int debug = 0;

/* By default prefer to link with shareable image libraries.
   Override with -static.  */
static int staticp = 0;

/* By default generate an executable, not a shareable image library.
   Override with -shared.  */
static int share = 0;

/* Remember if IDENTIFICATION given on command line.  */
static int ident = 0;

/* Keep track of arg translations.  */
static int link_arg_max = -1;
static const char **link_args = 0;
static int link_arg_index = -1;

static int rcp_arg_max = -1;
static const char **rcp_args = 0;
static int rcp_arg_index = -1;

/* Keep track of filenames */
static char *sharefullfilename = (char *) "";
static char *exefullfilename = (char *) "";

static char *exefilename = (char *) "";

/* System search dir list. Leave blank since link handles this
   internally.  */
static char *system_search_dirs = (char *) "";

/* Search dir list passed on command line (with -L).  */
static char *search_dirs;

/* Local function declarations.  */

/* Add STR to the list of arguments to pass to the linker. Expand the list as
   necessary to accommodate.  */
static void addarg (const char *);
static void rcparg (const char *);

/* Check to see if NAME is a regular file, i.e. not a directory */
static int is_regular_file (char *);

/* Translate a Unix syntax file specification FILESPEC into VMS syntax.
   If indicators of VMS syntax found, return input string.  */
static char *to_host_file_spec (char *);

/* Locate the library named LIB_NAME in the set of paths PATH_VAL.  */
static char *locate_lib (char *, char *);

/* Given a library name NAME, i.e. foo,  Look for libfoo.lib and then
   libfoo.a in the set of directories we are allowed to search in.  */
static const char *expand_lib (char *);

/* Preprocess the number of args P_ARGC in ARGV.
   Look for special flags, etc. that must be handled first.  */
static void preprocess_args (int *, char **);

/* Preprocess the number of args P_ARGC in ARGV.  Look for
   special flags, etc. that must be handled for the VMS linker.  */
static void process_args (int *, char **);

/* Link with OpenVMS 7.1 libraries if logical name set */
static void maybe_set_link_compat (void);

/* set environment defined executable attributes */
static int set_exe (const char *);

#ifdef VMS
/* Action routine called by decc$to_vms. NAME is a file name or
   directory name. TYPE is unused.  */
static int translate_unix (char *, int);
#endif

int main (int, char **);

static void
addarg (const char *str)
{
  int i;

  if (++link_arg_index >= link_arg_max)
    {
      const char **new_link_args
	= (const char **) xcalloc (link_arg_max + 1000, sizeof (char *));

      for (i = 0; i <= link_arg_max; i++)
	new_link_args [i] = link_args [i];

      if (link_args)
	free (link_args);

      link_arg_max += 1000;
      link_args = new_link_args;
    }

  if (str)
    link_args [link_arg_index] = xstrdup (str);
  else
    link_args [link_arg_index] = str;

}

static void
rcparg (const char *str)
{
  int i;

  if (++rcp_arg_index >= rcp_arg_max)
    {
      const char **new_rcp_args
	= (const char **) xcalloc (rcp_arg_max + 1000, sizeof (char *));

      for (i = 0; i <= rcp_arg_max; i++)
	new_rcp_args [i] = rcp_args [i];

      if (rcp_args)
	free (rcp_args);

      rcp_arg_max += 1000;
      rcp_args = new_rcp_args;
    }

  if (str)
    rcp_args [rcp_arg_index] = xstrdup (str);
  else
    rcp_args [rcp_arg_index] = str;
}

static char *
locate_lib (char *lib_name, char *path_val)
{
  int lib_len = strlen (lib_name);
  char *eptr, *sptr;

  for (sptr = path_val; *sptr; sptr = eptr)
    {
      char *buf, *ptr;

      while (*sptr == PATH_SEPARATOR)
	sptr ++;

      eptr = strchr (sptr, PATH_SEPARATOR);
      if (eptr == 0)
	eptr = strchr (sptr, 0);

      buf = xmalloc ((eptr-sptr) + lib_len + 4 + 2);
      strncpy (buf, sptr, eptr-sptr);
      buf [eptr-sptr] = 0;
      strcat (buf, "/");
      strcat (buf, lib_name);
      ptr = strchr (buf, 0);

      if (staticp)
	{
	  /* For debug or static links, look for shareable image libraries
	     last.  */
	  strcpy (ptr, ".a");
	  if (is_regular_file (buf))
	    return xstrdup (to_host_file_spec (buf));

	  strcpy (ptr, ".olb");
	  if (is_regular_file (buf))
	    return xstrdup (to_host_file_spec (buf));

	  strcpy (ptr, ".exe");
	  if (is_regular_file (buf))
	    return xstrdup (to_host_file_spec (buf));
	}
      else
	{
	  /* Otherwise look for shareable image libraries first.  */
	  strcpy (ptr, ".exe");
	  if (is_regular_file (buf))
	    return xstrdup (to_host_file_spec (buf));

	  strcpy (ptr, ".a");
	  if (is_regular_file (buf))
	    return xstrdup (to_host_file_spec (buf));

	  strcpy (ptr, ".olb");
	  if (is_regular_file (buf))
	    return xstrdup (to_host_file_spec (buf));
	}
    }

  return 0;
}

static const char *
expand_lib (char *name)
{
  char *lib, *lib_path;

  if (strcmp (name, "c") == 0)
    return "";

  else if (strcmp (name, "m") == 0)
    /* No separate library for math functions */
    return "";

  else
    {
      lib = xmalloc (strlen (name) + 14);

      strcpy (lib, "lib");
      strcat (lib, name);
      lib_path = locate_lib (lib, search_dirs);

      if (lib_path)
	return lib_path;
    }

  fprintf (stderr,
	   "Couldn't locate library: lib%s.exe, lib%s.a or lib%s.olb\n",
	   name, name, name);

  exit (1);
}

static int
is_regular_file (char *name)
{
  int ret;
  struct stat statbuf;

  ret = stat (name, &statbuf);
  return !ret && S_ISREG (statbuf.st_mode);
}

static void
preprocess_args (int *p_argc, char **argv)
{
  int i;

  for (i = 1; i < *p_argc; i++)
    if (strlen (argv[i]) >= 6 && strncmp (argv[i], "-shared", 7) == 0)
      share = 1;

  for (i = 1; i < *p_argc; i++)
    if (strcmp (argv[i], "-o") == 0)
      {
	char *buff, *ptr;
	int out_len;
	int len;

	i++;
	ptr = to_host_file_spec (argv[i]);
	exefullfilename = xstrdup (ptr);
	out_len = strlen (ptr);
	buff = xmalloc (out_len + 18);

	if (share)
	  strcpy (buff, "/share=");
	else
	  strcpy (buff, "/exe=");

	if (remote)
	  strcat (buff, basename (ptr));
	else
	  strcat (buff, ptr);

	addarg (buff);

	if (share)
	  {
	    sharefullfilename = xmalloc (out_len+5);
	    if (ptr == strchr (argv[i], ']'))
	      strcpy (sharefullfilename, ++ptr);
	    else if (ptr == strchr (argv[i], ':'))
	      strcpy (sharefullfilename, ++ptr);
	    else if (ptr == strrchr (argv[i], '/'))
	      strcpy (sharefullfilename, ++ptr);
	    else
	      strcpy (sharefullfilename, argv[i]);

	    len = strlen (sharefullfilename);
	    if (strncasecmp (&sharefullfilename[len-4], ".exe", 4) == 0)
	      sharefullfilename[len-4] = 0;

	    for (ptr = sharefullfilename; *ptr; ptr++)
	      *ptr = toupper (*ptr);
	  }
      }

  if (strlen (exefullfilename) == 0 && !share)
    {
      exefullfilename = (char *) "a_out.exe";
      addarg (xstrdup ("/exe=a_out.exe"));
    }

  exefilename = xstrdup (basename (exefullfilename));
}

static void
process_args (int *p_argc, char **argv)
{
  int i;

  for (i = 1; i < *p_argc; i++)
    {
      if (strlen (argv[i]) < 2)
	continue;

      if (strncmp (argv[i], "-L", 2) == 0)
	{
	  char *nbuff, *ptr;
	  int new_len, search_dirs_len;

	  ptr = &argv[i][2];
	  new_len = strlen (ptr);
	  search_dirs_len = strlen (search_dirs);

	  nbuff = xmalloc (new_len + 1);
	  strcpy (nbuff, ptr);

	  /* Remove trailing slashes.  */
	  while (new_len > 1 && nbuff [new_len - 1] == '/')
	    {
	      nbuff [new_len - 1] = 0;
	      new_len--;
	    }

	  search_dirs = xrealloc (search_dirs, search_dirs_len + new_len + 2);
	  if (search_dirs_len > 0)
	    strcat (search_dirs, PATH_SEPARATOR_STR);

	  strcat (search_dirs, nbuff);
	  free (nbuff);
	}

      /* -v turns on verbose option here and is passed on to gcc.  */
      else if (strcmp (argv[i], "-v") == 0)
	verbose = 1;
      else if (strcmp (argv[i], "--version") == 0)
	{
	  fprintf (stdout, "VMS Linker\n");
          exit (0);
	}
      else if (strcmp (argv[i], "--help") == 0)
	{
	  fprintf (stdout, "VMS Linker\n");
          exit (0);
	}
      else if (strcmp (argv[i], "-g0") == 0)
	addarg ("/notraceback");
      else if (strncmp (argv[i], "-g", 2) == 0)
	{
	  addarg ("/debug");
	  debug = 1;
	}
      else if (strcmp (argv[i], "-static") == 0)
	staticp = 1;
      else if (strcmp (argv[i], "-map") == 0)
	{
	  char *buff, *ptr;

	  buff = xmalloc (strlen (exefullfilename) + 5);
	  strcpy (buff, exefullfilename);
	  ptr = strrchr (buff, '.');
	  if (ptr)
	    *ptr = 0;

	  strcat (buff, ".map");
	  addarg ("/map=");
	  addarg (buff);
	  addarg ("/full");
	}
      else if (strcmp (argv[i], "-save-temps") == 0)
	save_temps = 1;
      else if (strcmp (argv[i], "--noinhibit-exec") == 0)
	inhibit_exec = 0;
    }
}

#ifdef VMS
typedef struct dsc {
  unsigned short len, mbz;
  char *adr;
} Descriptor;

struct lst {
  unsigned short buflen, item_code;
  void *bufaddr;
  void *retlenaddr;
};

static struct {
  struct lst items [1];
  unsigned int terminator;
} item_lst1;

static struct {
  struct lst items [2];
  unsigned int terminator;
} item_lst2;

/* Checks if logical names are defined for setting system library path and
   linker program to enable compatibility with earlier VMS versions. */

static void
maybe_set_link_compat ()
{
  char lnm_buff [LNM_C_NAMLENGTH];
  unsigned int lnm_buff_len;
  int status;
  Descriptor tabledsc, linkdsc;

  tabledsc.adr = "LNM$JOB";
  tabledsc.len = strlen (tabledsc.adr);
  tabledsc.mbz = 0;

  linkdsc.adr = "GCC_LD_SYS$LIBRARY";
  linkdsc.len = strlen (linkdsc.adr);
  linkdsc.mbz = 0;

  item_lst1.items[0].buflen = LNM_C_NAMLENGTH;
  item_lst1.items[0].item_code = LNM__STRING;
  item_lst1.items[0].bufaddr = lnm_buff;
  item_lst1.items[0].retlenaddr = &lnm_buff_len;
  item_lst1.terminator = 0;

  status = SYS$TRNLNM
    (0,          /* attr */
     &tabledsc,  /* tabnam */
     &linkdsc,   /* lognam */
     0,          /* acmode */
     &item_lst1);

  /* if GCC_LD_SYS$LIBRARY is defined, redefine SYS$LIBRARY to search
     the equivalence name first for system libraries, then the default
     system library directory */

  if ((status & 1) == 1)
    {
      unsigned char acmode = PSL_C_USER; /* Don't retain after image exit */
      char *syslib = "SYS$SYSROOT:[SYSLIB]"; /* Default SYS$LIBRARY */

      /* Only visible to current and child processes */
      tabledsc.adr = "LNM$PROCESS";
      tabledsc.len = strlen (tabledsc.adr);
      tabledsc.mbz = 0;

      linkdsc.adr = "SYS$LIBRARY";
      linkdsc.len = strlen (linkdsc.adr);
      linkdsc.mbz = 0;

      item_lst2.items[0].buflen = lnm_buff_len;
      item_lst2.items[0].item_code = LNM__STRING;
      item_lst2.items[0].bufaddr = lnm_buff;
      item_lst2.items[0].retlenaddr = 0;

      item_lst2.items[1].buflen = strlen (syslib);
      item_lst2.items[1].item_code = LNM__STRING;
      item_lst2.items[1].bufaddr = syslib;
      item_lst2.items[1].retlenaddr = 0;
      item_lst2.terminator = 0;

      status = SYS$CRELNM
	(0,          /* attr */
	 &tabledsc,  /* tabnam */
	 &linkdsc,   /* lognam */
	 &acmode,    /* acmode */
	 &item_lst2);

    }

  tabledsc.adr = "LNM$JOB";
  tabledsc.len = strlen (tabledsc.adr);
  tabledsc.mbz = 0;

  linkdsc.adr = "GCC_LD_LINK";
  linkdsc.len = strlen (linkdsc.adr);
  linkdsc.mbz = 0;

  item_lst1.items[0].buflen = LNM_C_NAMLENGTH;
  item_lst1.items[0].item_code = LNM__STRING;
  item_lst1.items[0].bufaddr = lnm_buff;
  item_lst1.items[0].retlenaddr = &lnm_buff_len;
  item_lst1.terminator = 0;

  status = SYS$TRNLNM
    (0,          /* attr */
     &tabledsc,  /* tabnam */
     &linkdsc,   /* lognam */
     0,          /* acmode */
     &item_lst1);

  /* if GCC_LD_LINK is defined, redefine LINK to use the equivalence name
     (sometimes the LINK program version is used by VMS to determine
     compatibility) */

  if ((status & 1) == 1)
    {
      unsigned char acmode = PSL_C_USER; /* Don't retain after image exit */

      /* Only visible to current and child processes */
      tabledsc.adr = "LNM$PROCESS";
      tabledsc.len = strlen (tabledsc.adr);
      tabledsc.mbz = 0;

      linkdsc.adr = "LINK";
      linkdsc.len = strlen (linkdsc.adr);
      linkdsc.mbz = 0;

      item_lst1.items[0].buflen = lnm_buff_len;
      item_lst1.items[0].item_code = LNM__STRING;
      item_lst1.items[0].bufaddr = lnm_buff;
      item_lst1.items[0].retlenaddr = 0;
      item_lst1.terminator = 0;

      status = SYS$CRELNM
	(0,          /* attr */
	 &tabledsc,  /* tabnam */
	 &linkdsc,   /* lognam */
	 &acmode,    /* acmode */
	 &item_lst1);
    }
}
#else
static void
maybe_set_link_compat () {;}
#endif

static int set_exe (const char *arg)
{
  char allargs [1024], buff [256];
  FILE *linkfile;

  strcpy (allargs, "$@gnu:[bin]set_exe ");
  strcat (allargs, exefullfilename);
  strcat (allargs, " ");
  strcat (allargs, arg);
  linkfile = popen (allargs, "r");

  if (verbose)
    printf ("%s\n", allargs);

  if (!linkfile)
    {
      perror ("ld popen set_exe");
      return (1);
    }

  fgets (buff, sizeof (buff), linkfile);
  while (!feof (linkfile) ) {
    fputs (buff, stdout);
    fgets (buff, sizeof (buff), linkfile);
  }

  if (pclose (linkfile) != 0)
    {
      perror ("ld pclose set_exe");
      return (1);
    }

  return (0);
}

/* The main program.  Spawn the VMS linker after fixing up the Unix-like flags
   and args to be what the VMS linker wants.  */

int
main (int argc, char **argv)
{
  int i;
  char cwdev [128], *devptr;
  int devlen;
  FILE *optfile;
  char *cwd, *ptr;
  char *optfilename;
  char *comfilename = '\0';
  int status;

  rcp_target = getenv ("GCC_CROSS_TARGET");
  rcp_target_username = getenv ("GCC_CROSS_TARGET_USER");
  if (remote && (!rcp_target || !rcp_target_username))
    {
      fputs ("GCC_CROSS_TARGET or GCC_CROSS_TARGET_USER undefined\n",
             stderr);
      return 1;
    }

  if (!remote )
    { 
      if (getenv ("GNAT$LD_NOCALL_DEBUG"))
        ld_nocall_debug = 1;
      if (getenv ("GNAT$LD_MKTHREADS"))
        ld_mkthreads = 1;
      if (getenv ("GNAT$LD_UPCALLS"))
        ld_upcalls = 1;
    }

#ifdef VMS
  cwd = getcwd (0, 1024, 1);
#else
  cwd = getcwd (0, 1024);
  strcat (cwd, "/");
#endif

  devptr = strchr (cwd, ':');
  if (devptr)
    devlen = (devptr - cwd) + 1;
  else
    devlen = 0;
  strncpy (cwdev, cwd, devlen);
  cwdev [devlen] = '\0';

  maybe_set_link_compat ();

  search_dirs = xstrdup (system_search_dirs);

  addarg ("link");

  /* Pass to find args that have to be append first.  */
  preprocess_args (&argc , argv);

  /* Pass to find the rest of the args.  */
  process_args (&argc , argv);

  if (!verbose)
    addarg ("/noinform");

  /* Create a temp file to hold args, otherwise we can easily exceed the VMS
     command line length limits.  */

  optfilename = xmalloc (strlen (exefilename) + 13);
  strcpy (optfilename, exefilename);
  ptr = strrchr (optfilename, '.');
  if (ptr)
    *ptr = 0;
  strcat (optfilename, ".opt_tmpfile");
  optfile = fopen (optfilename, "w");

  if (remote)
    {
      comfilename = xmalloc (strlen (exefilename) + 13);
      strcpy (comfilename, exefilename);
      ptr = strrchr (comfilename, '.');
      if (ptr)
        *ptr = 0;
      strcat (comfilename, ".com_tmpfile");
    }

  /* Write out the IDENTIFICATION argument first so that it can be overridden
     by an options file.  */
  for (i = 1; i < argc; i++)
    {
      int arg_len = strlen (argv[i]);

      if (arg_len > 6 && strncasecmp (argv[i], "IDENT=", 6) == 0)
	{
	  /* Comes from command line. If present will always appear before
	     IDENTIFICATION=... and will override.  */

	  if (!ident)
	    ident = 1;
	}
      else if (arg_len > 15
	       && strncasecmp (argv[i], "IDENTIFICATION=", 15) == 0)
	{
	  /* Comes from pragma Ident ().  */

	  if (!ident)
	    {
	      fprintf (optfile, "case_sensitive=yes\n");
	      fprintf (optfile, "IDENTIFICATION=\"%15.15s\"\n", &argv[i][15]);
	      fprintf (optfile, "case_sensitive=NO\n");
	      ident = 1;
	    }
	}
    }

  for (i = 1; i < argc; i++)
    {
      int arg_len = strlen (argv[i]);

      if (strcmp (argv[i], "-o") == 0)
	i++;
      else if (arg_len > 2 && strncmp (argv[i], "-l", 2) == 0)
	{
	  char *libname = (char *) expand_lib (&argv[i][2]);
	  const char *ext;
	  int len;

	  if ((len = strlen (libname)) > 0)
	    {
	      char *buff;

	      buff = (char *) xmalloc (strlen (cwdev) + strlen (libname) + 2);

	      if (remote)
		{
		  /* Libraries assumed to already be in working dir
		     on remote machine */
		  libname = basename (libname);
		  len = strlen (libname);
	          sprintf (buff, "%s\n", libname);
	          /* rcparg (buff); */
		}

	      if (len > 4 && strcasecmp (&libname [len-4], ".exe") == 0)
		ext = "/shareable";
	      else
		ext = "/library";

	      if (libname[0] == '[' || libname[0] == '/')
		{
		  sprintf (buff, "%s%s", cwdev, libname);
		}
	      else
		{
		  sprintf (buff, "%s", libname);
		}

	      fprintf (optfile, "%s%s%s\n",
		       remote ? "sys$login:" : "", buff, ext);
	    }
	}

      else if (strcmp (argv[i], "-v" ) == 0
	       || strncmp (argv[i], "-g", 2 ) == 0
	       || strcmp (argv[i], "-static" ) == 0
	       || strcmp (argv[i], "-map" ) == 0
	       || strcmp (argv[i], "-save-temps") == 0
	       || strcmp (argv[i], "--noinhibit-exec") == 0
	       || (arg_len > 2 && strncmp (argv[i], "-L", 2) == 0)
	       || (arg_len >= 6 && strncmp (argv[i], "-share", 6) == 0))
	;
      else if (arg_len > 1 && argv[i][0] == '@')
	{
	  FILE *atfile;
	  char *ptr, *ptr1;
	  struct stat statbuf;
	  char *buff;
	  int len;

	  if (stat (&argv[i][1], &statbuf))
	    {
	      fprintf (stderr, "Couldn't open linker response file: %s\n",
		       &argv[i][1]);
	      exit (1);
	    }

	  buff = xmalloc (statbuf.st_size + 1);
	  atfile = fopen (&argv[i][1], "r");
	  fgets (buff, statbuf.st_size + 1, atfile);
	  fclose (atfile);

	  len = strlen (buff);
	  if (buff [len - 1] == '\n')
	    {
	      buff [len - 1] = 0;
	      len--;
	    }

	  ptr = buff;

	  do
	  {
	     ptr1 = strchr (ptr, ' ');
	     if (ptr1)
	       *ptr1 = 0;
	     ptr = to_host_file_spec (ptr);
	     if (ptr[0] == '[')
	       fprintf (optfile, "%s%s\n", cwdev, ptr);
	     else
	       fprintf (optfile, "%s\n", ptr);
	     ptr = ptr1 + 1;
	  } while (ptr1);
	}

      /* Unix style file specs and VMS style switches look alike, so assume an
	 arg consisting of one and only one slash, and that being first, is
	 really a switch.  */
      else if ((argv[i][0] == '/') && (strchr (&argv[i][1], '/') == 0))
	addarg (argv[i]);
      else if (arg_len > 4
	       && strncasecmp (&argv[i][arg_len-4], ".opt", 4) == 0)
	{
	  FILE *optfile1;
	  char buff [256];

	  /* Disable __UNIX_FOPEN redefinition in case user supplied .opt
	     file is not stream oriented. */

	  optfile1 = (fopen) (argv[i], "r");
	  if (optfile1 == 0)
	    {
	      perror (argv[i]);
	      status = 1;
	      goto cleanup_and_exit;
	    }
	  
	  while (fgets (buff, 256, optfile1))
	    fputs (buff, optfile);

	  fclose (optfile1);
	}
      else if (arg_len > 7 && strncasecmp (argv[i], "GSMATCH", 7) == 0)
	fprintf (optfile, "%s\n", argv[i]);
      else if (arg_len > 6 && strncasecmp (argv[i], "IDENT=", 6) == 0)
	{
	  /* Comes from command line and will override pragma.  */
	  fprintf (optfile, "case_sensitive=yes\n");
	  fprintf (optfile, "IDENT=\"%15.15s\"\n", &argv[i][6]);
	  fprintf (optfile, "case_sensitive=NO\n");
	  ident = 1;
	}
      else if (arg_len > 15
	       && strncasecmp (argv[i], "IDENTIFICATION=", 15) == 0)
	;
      else
	{
	  /* Assume filename arg.  */
	  const char *addswitch = "";
	  char *buff, *buff1;
	  int buff_len;
	  int is_cld = 0;

	  argv[i] = to_host_file_spec (argv[i]);
	  arg_len = strlen (argv[i]);

	  /* Handle shareable image libraries */

	  if (arg_len > 4 && strcasecmp (&argv[i][arg_len-4], ".exe") == 0)
	    addswitch = "/shareable";

	  if (arg_len > 4 && strcasecmp (&argv[i][arg_len-4], ".cld") == 0)
	    {
	      addswitch = "/shareable";
	      is_cld = 1;
	    }

	  /* Handle object libraries */

	  if (arg_len > 2 && strcasecmp (&argv[i][arg_len-2], ".a") == 0)
	    addswitch = "/lib";

	  if (arg_len > 4 && strcasecmp (&argv[i][arg_len-4], ".olb") == 0)
	    addswitch = "/lib";

	  /* Absolutize file location */

	  if (argv[i][0] == '[' || argv[i][0] == '/')
	    {
	      buff1 = (char *) xmalloc (strlen (cwdev) + strlen (argv[i]) + 1);
	      sprintf (buff1, "%s%s", cwdev, argv[i]);
	    }
	  else if (strchr (argv[i], ':'))
	    {
	      buff1 = (char *) xmalloc (strlen (argv[i]) + 1);
	      sprintf (buff1, "%s", argv[i]);
	    }
	  else
	    {
	      buff1 = (char *) xmalloc (strlen (cwd) + strlen (argv[i]) + 1);
	      sprintf (buff1, "%s%s", cwd, argv[i]);
	    }

	  buff = (char *) xmalloc (strlen (buff1) + strlen (addswitch) + 2);

	  if (remote && strlen (addswitch))
	    {
	      /* Libraries assumed to already be in working dir
		 on remote machine */
	      buff1 = basename (buff1);
	      sprintf (buff, "%s\n", buff1);
	      /* rcparg (buff); */
	    }

	  sprintf (buff, "%s%s", buff1, addswitch);

	  buff_len = strlen (buff);

	  if (buff_len >= 15
	      && strcasecmp (&buff[buff_len - 14], "vms-dwarf2eh.o") == 0)
	    {
	      if (remote)
		{
		  rcparg (buff);
	          vmsdwarf2ehspec = xstrdup (basename (buff));
		}
	      else
	        vmsdwarf2ehspec = xstrdup (buff);
	    }
	  else if (buff_len >= 13
	      && strcasecmp (&buff[buff_len - 12],"vms-dwarf2.o") == 0)
            {
	      if (remote)
		{
		  rcparg (buff);
	          vmsdwarf2spec = xstrdup (basename (buff));
		}
	      else
	        vmsdwarf2spec = xstrdup (buff);
            }
	  else if (is_cld)
	    {
	      addarg (buff);
	      addarg (",");
	    }
	  else
	    {
	      int len = strlen (buff);

	      if ((len > 2 && strncasecmp (&buff[len-2], ".o", 2) == 0)
		|| (len > 4 && strncasecmp (&buff[len-4], ".obj", 4) == 0))
		{
		  rcparg (buff);
		  fprintf (optfile, "%s\n", basename (buff));
		}
	      else
	        {
		  fprintf (optfile, "%s\n", buff);
		}
	    }
	}
    }

#if 0
  if (share)
    fprintf (optfile, "symbol_vector=(main=procedure)\n");
#endif

  if (vmsdwarf2ehspec)
    {
      /* Sequentialize exception handling info */

      fprintf (optfile, "case_sensitive=yes\n");
      fprintf (optfile, "cluster=DWARF2eh,,,%s\n", vmsdwarf2ehspec);
      fprintf (optfile, "collect=DWARF2eh,eh_frame\n");
      fprintf (optfile, "case_sensitive=NO\n");
    }

  if (debug && vmsdwarf2spec)
    {
      /* Sequentialize the debug info */

      fprintf (optfile, "case_sensitive=yes\n");
      fprintf (optfile, "cluster=DWARF2debug,,,%s\n", vmsdwarf2spec);
      fprintf (optfile, "collect=DWARF2debug,debug_abbrev,debug_aranges,-\n");
      fprintf (optfile, " debug_frame,debug_info,debug_line,debug_loc,-\n");
      fprintf (optfile, " debug_macinfo,debug_pubnames,debug_str,-\n");
      fprintf (optfile, " debug_zzzzzz\n");
      fprintf (optfile, "case_sensitive=NO\n");
    }

#ifdef VMS_DEBUGGING_INFO
/* VMS_DEBUGGING_INFO is defined in alpha/vms.h but not ia64/vms.h */
  if (debug && share)
    {
      /* Sequentialize the shared library debug info */

      char *sharefilename = basename (sharefullfilename);

      fprintf (optfile, "case_sensitive=yes\n");
      fprintf (optfile, "symbol_vector=(-\n");
      fprintf (optfile,
	       "%s$DWARF2.DEBUG_ABBREV/$dwarf2.debug_abbrev=DATA,-\n",
	       sharefilename);
      fprintf (optfile,
	       "%s$DWARF2.DEBUG_ARANGES/$dwarf2.debug_aranges=DATA,-\n",
	       sharefilename);
      fprintf (optfile, "%s$DWARF2.DEBUG_FRAME/$dwarf2.debug_frame=DATA,-\n",
	       sharefilename);
      fprintf (optfile, "%s$DWARF2.DEBUG_INFO/$dwarf2.debug_info=DATA,-\n",
	       sharefilename);
      fprintf (optfile, "%s$DWARF2.DEBUG_LINE/$dwarf2.debug_line=DATA,-\n",
	       sharefilename);
      fprintf (optfile, "%s$DWARF2.DEBUG_LOC/$dwarf2.debug_loc=DATA,-\n",
	       sharefilename);
      fprintf (optfile,
	       "%s$DWARF2.DEBUG_MACINFO/$dwarf2.debug_macinfo=DATA,-\n",
	       sharefilename);
      fprintf (optfile,
	       "%s$DWARF2.DEBUG_PUBNAMES/$dwarf2.debug_pubnames=DATA,-\n",
	       sharefilename);
      fprintf (optfile, "%s$DWARF2.DEBUG_STR/$dwarf2.debug_str=DATA,-\n",
	       sharefilename);
      fprintf (optfile, "%s$DWARF2.DEBUG_ZZZZZZ/$dwarf2.debug_zzzzzz=DATA)\n",
	       sharefilename);
      fprintf (optfile, "case_sensitive=NO\n");
    }
#endif

  fprintf (optfile, "PSECT_ATTR=LIB$INITIALIZE,GBL\n");
  fclose (optfile);
  addarg (optfilename);
  addarg ("/opt");
  rcparg (optfilename);

  if (remote)
    {
      rcparg (comfilename);
    }

  addarg (NULL);
  rcparg (NULL);

  if (verbose)
    {
      int i;

      for (i = 0; i < link_arg_index; i++)
	printf ("%s ", link_args [i]);
      putchar ('\n');

      if (remote)
	{
	  printf ("rcp ");
	  for (i = 0; i < rcp_arg_index; i++)
	    printf ("%s ", rcp_args [i]);
	  putchar ('\n');
	}
      }

  {
    int i;
    int len = 0;
    int rcplen = 0;
    char *rcpbuff;

    for (i = 0; link_args[i]; i++)
      len = len + strlen (link_args[i]) + 1;

    for (i = 0; rcp_args[i]; i++)
      rcplen = rcplen + strlen (rcp_args[i]) + 1;

    if (remote)
      {
        rcpbuff = (char *) xmalloc (rcplen + strlen (rcp_target) +
                                    strlen (rcp_target_username) + 8);
        strcpy (rcpbuff, "rcp ");

	for (i = 0; rcp_args [i]; i++)
	  {
	    strcat (rcpbuff, rcp_args [i]);
	    strcat (rcpbuff, " ");
	  }

	sprintf (rcpbuff + strlen (rcpbuff), "%s@%s:\n",
		 rcp_target_username, rcp_target);
      }


    {
      char *allargs = (char *) xmalloc (len + 2);
      int status = 0;
      char buff [256];
      FILE *linkfile;

      for (i = 0; i < len + 2; i++)
	allargs [i] = 0;

      strcpy (allargs, "$");
      for (i = 0; link_args [i]; i++)
	{
	  strcat (allargs, link_args [i]);
	  strcat (allargs, " ");
	}

      if (remote)
	{
	  FILE *comfile;
	  char *buff = (char *) xmalloc (
	    strlen (rcp_target) +
	    strlen (rcp_target_username) +
	    strlen (comfilename) +
	    strlen (exefilename) +
	    strlen (optfilename) +
	    64);

	  comfile = fopen (comfilename, "w");
	  if (verbose) fprintf (stdout, "%s\n", allargs);
	  fprintf (comfile, remotelink_cmd, allargs, rcp_target);
	  fclose (comfile);

	  if (verbose) fprintf (stdout, "%s", rcpbuff);
	  system (rcpbuff);

          sprintf (buff, "rsh %s -l %s \"@%s \\\"%s\\\" %s %d %d\"\n",
	     rcp_target,
	     rcp_target_username,
	     comfilename,
	     exefilename,
	     optfilename,
	     inhibit_exec,
	     share);
	  if (verbose) fprintf (stdout, buff);
	  system (buff);

	  sprintf (buff, "rcp \"%s@%s:%s\" %s\n",
		   rcp_target_username, rcp_target,
		   exefilename, exefullfilename);
	  if (verbose) fprintf (stdout, buff);
	  status = system (buff); 
	}
      else
	{
          linkfile = popen (allargs, "r");
          if (!linkfile)
	    {
	      perror ("ld popen link");
	      status = 1;
	      goto cleanup_and_exit;
	    }

          fgets (buff, sizeof (buff), linkfile);
          while (!feof (linkfile) ) {
            fputs (buff, stdout);
            fgets (buff, sizeof (buff), linkfile);
          }
          status = pclose (linkfile);
          if (status != 0)
	    {
	      perror ("ld pclose link");
	      goto cleanup_and_exit;
	    }

          if (debug && !share && ld_nocall_debug)
	    {
	      status = set_exe ("/flags=nocall_debug");
	      if (status != 0)
	        goto cleanup_and_exit;
	    }

          if (!share && ld_mkthreads)
	    {
	      status = set_exe ("/flags=mkthreads");
	      if (status != 0)
	        goto cleanup_and_exit;
	    }

          if (!share && ld_upcalls)
	    {
	      status = set_exe ("/flags=upcalls");
	      if (status != 0)
	        goto cleanup_and_exit;
	    }
	}

    cleanup_and_exit:
      if (!save_temps)
	{
	  remove (optfilename);
	 }

      if (remote)
	{
	  if (comfilename && !save_temps)
	    remove (comfilename);
	}

      if (status == 0)
	exit (0);

      if (exefullfilename && inhibit_exec == 1)
	remove (exefullfilename);

      exit (1);
    }
  }
}

static char new_host_filespec [255];
static char filename_buff [256];

#ifdef VMS
static int
translate_unix (char *name, int type)
{
  strcpy (filename_buff, name);
  return 0;
}
#endif

static char *
to_host_file_spec (char *filespec)
{
  strcpy (new_host_filespec, "");
  if (strchr (filespec, ']') || strchr (filespec, ':'))
    strcpy (new_host_filespec, filespec);
  else
    {
#ifdef VMS
      decc$to_vms (filespec, translate_unix, 1, 1);
#else
     strcpy (filename_buff, filespec);
#endif
      strcpy (new_host_filespec, filename_buff);
    }

  return new_host_filespec;
}
