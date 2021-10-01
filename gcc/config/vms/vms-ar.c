/* VMS archive wrapper.
   Copyright (C) 1998,2003,2004 Free Software Foundation, Inc.
   Contributed by Douglas B. Rupp (rupp@gnat.com).

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define xstrdup strdup
#define xmalloc malloc
#define FATAL_EXIT_CODE (44 | 0x10000000)
#define SUCCESS_EXIT_CODE 0

static int libr_arg_max = -1;
static char **libr_args = (char **) 0;
static int libr_arg_index = -1;
static int libr_args_len = 0;
static int replace_mode = 0;
static int create_mode = 0;
static int verbose_mode = 0;

static char *remotear_cmd=\
(char *) "\
$ if p1.nes.\"\" then goto op1\n\
$!\n\
$! <tempdir> = rsh <targ> \"@ar_remote_command\"\n\
$!\n\
$ tempdir = f$unique()\n\
$ create/dir [.'tempdir']\n\
$ write sys$output tempdir\n\
$ exit 1\n\
$!\n\
$! rcp <lib> <objs...> \"<targ>:[.<tempdir>]\"\n\
$! rsh target \"@ar_remote_command <tempdir> <lib> <libflags>\"\n\
$!\n\
$ op1:\n\
$ tempdir = p1\n\
$ libname = p2\n\
$ libflags = p3\n\
$ set def [.'tempdir]\n\
$ set file/attr=(rfm:var,rat:none) *.* /excl='libname\n\
$ library 'libflags 'libname *.*\n\
$ copy 'libname [-]\n\
$ exit $status\n\
$!\n\
$! rcp \"<targ>:[.<tempdir>]<lib>\" .\n\
";

static char arcmd[] = "@ar_remote_command.com";
static char modecmd [32];
static char libname [256];

/* Local variable declarations.  */
static char *rcp_target;
static char *rcp_target_username;

/* Operation in cross remote mode */
#ifdef CROSS_COMPILE
static int remote = 1;
#else
static int remote = 0;
#endif
static int copy_lib_flag = 0;

static int is_regular_file (char *name);

/* Check to see if the file named in NAME is a regular file, i.e. not a
   directory */

static int
is_regular_file (name)
     char *name;
{
  int ret;
  struct stat statbuf;

  ret = stat(name, &statbuf);
  return !ret && S_ISREG (statbuf.st_mode);
}

/* Add the argument contained in STR to the list of arguments to pass to the
   linker */

static void
addarg (str)
     char *str;
{
  int i;

  if (++libr_arg_index >= libr_arg_max)
    {
      char **new_libr_args
	= (char **) calloc (libr_arg_max + 1000, sizeof (char *));

      for (i = 0; i <= libr_arg_max; i++)
	new_libr_args [i] = libr_args [i];

      if (libr_args)
	free (libr_args);

      libr_arg_max += 1000;
      libr_args = new_libr_args;
    }

  libr_args [libr_arg_index] = str;
}


int
main (argc, argv)
     int argc;
     char *argv[];
{
  int i, nexti, iarg;
  int status;
  char *rcpargs;
  FILE *libfile;
  char ar_tmpdir [256], ar_command [256], buff [256];

  for (i=0; i<argc; i++)
    printf ("%d: %s\n", i, argv[i]);

  rcp_target = getenv ("GCC_CROSS_TARGET");
  rcp_target_username = getenv ("GCC_CROSS_TARGET_USER");
  if (remote && (!rcp_target || !rcp_target_username))
    {
      fputs ("GCC_CROSS_TARGET or GCC_CROSS_TARGET_USER undefined\n", stderr);
      return 1;
    } 

  if (argc < 2)
    {
      fputs ("Too few parameters", stderr);
      return 1;
    }
 
  if (argv[1][0] != '-')
    {
      for (i = 0; i < (int) strlen (argv[1]); i++)
	{
	  if (argv[1][i] == 'r')
	    {
	      replace_mode = 1;
	    }
	  else if (argv[1][i] == 'c')
	    {
	      create_mode = 1;
	    }
	  else if (argv[1][i] == 'v')
	    {
	      verbose_mode = 1;
	    }
	}
      nexti = 2;
    }
  else
    {
      for (i = 1; i < argc; i++)
	{
	  if (argv[i][0] != '-')
	    {
	      nexti = i;
	      break;
	    }
	  else if (strcmp (argv[i], "-r") == 0)
	    {
	      replace_mode = 1;
	    }
	  else if (strcmp (argv[i], "-c") == 0)
	    {
	      create_mode = 1;
	    }
	  else if (strcmp (argv[i], "-v") == 0)
	    {
	      verbose_mode = 1;
	    }
	}
    }

  if (replace_mode == 0 && create_mode == 0)
   exit (FATAL_EXIT_CODE);

  strcpy (libname, argv[nexti]);

  if (replace_mode)
    {
      strcat (modecmd, "/replace");
    }

  if ((create_mode && !is_regular_file (argv[nexti])) ||
      (create_mode && !replace_mode))
    {
      strcat (modecmd, "/create");
    }
  else
    {
      copy_lib_flag = 1;
    }

  nexti++;

  for (i = nexti; i < argc; i++)
    {
      char *arg = xstrdup (argv[i]);

      libr_args_len += strlen (arg) + 1;
      addarg (arg);
    }

  addarg (NULL);

  if (remote)
    {
      FILE *comfile;

      comfile = fopen ("ar_remote_command.com", "w");
      fprintf (comfile, remotear_cmd);
      fclose (comfile);

      sprintf (ar_command, "rcp %s %s@%s:\n",
	       &arcmd[1], rcp_target_username, rcp_target);
      system (ar_command);

      sprintf (ar_command, "rsh %s -l %s \"%s\"",
               rcp_target, rcp_target_username, arcmd);
    }
  else
    strcpy (ar_command, arcmd);

  libfile = popen (ar_command, "r");
  if (!libfile)
    {
      perror ("ar popen lib");
      status = 1;
      goto cleanup_and_exit;
    }

  fgets (ar_tmpdir, sizeof (ar_tmpdir), libfile);
  while (!feof (libfile) ) {
    fgets (buff, sizeof (buff), libfile);
  }
  status = pclose (libfile);
  if (status != 0)
    {
      perror ("ar pclose lib");
      goto cleanup_and_exit;
    }

  for (iarg=0; isalnum (ar_tmpdir [iarg]); iarg++);
  ar_tmpdir[iarg] = 0;

  rcpargs = (char *) xmalloc (strlen ("rcp ") + strlen (libname) + 1 +
                             libr_args_len + strlen (rcp_target) + 3 +
                             strlen (rcp_target_username) + 1 +
                             strlen (ar_tmpdir) + 5);

  if (remote)
    strcpy (rcpargs, "rcp ");
  else
    strcpy (rcpargs, "cp ");

  if (copy_lib_flag)
    {
      strcat (rcpargs, libname);
      strcat (rcpargs, " ");
    }

  iarg = 0;
      
  while (libr_args [iarg])
    {
      strcat (rcpargs, libr_args [iarg]);
      strcat (rcpargs, " ");
      iarg++;
    }

  if (remote)
    sprintf (rcpargs + strlen (rcpargs), "\"%s@%s:",
             rcp_target_username, rcp_target);

  strcat (rcpargs, "[.");
  strcat (rcpargs, ar_tmpdir);
  strcat (rcpargs, "]");

  if (remote)
    {
      strcat (rcpargs, "\"");
    }

  strcat (rcpargs, "\n");

  system (rcpargs);

  if (remote)
    sprintf (ar_command, "rsh %s -l %s \"%s %s %s %s\"\n",
             rcp_target, rcp_target_username, arcmd, ar_tmpdir,
             basename (libname), modecmd);
  else
    sprintf (ar_command, "%s %s %s %s\n",
             arcmd, ar_tmpdir, basename (libname), modecmd);

  system (ar_command);

  if (remote)
    sprintf (ar_command, "rcp \"%s@%s:[.%s]%s\" %s\n",
             rcp_target_username, rcp_target, ar_tmpdir,
             basename (libname), libname);
  else
    sprintf (ar_command, "cp %s/%s %s\n",
             ar_tmpdir, basename (libname), libname);

  system (ar_command);

cleanup_and_exit:
  if (status == 0)
    exit (SUCCESS_EXIT_CODE);
  else
    exit (FATAL_EXIT_CODE);
}
