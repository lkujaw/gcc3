/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                                   C A L                                  *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *          Copyright (C) 1992-2005, Free Software Foundation, Inc.         *
 *                                                                          *
 * GNAT is free software;  you can  redistribute it  and/or modify it under *
 * terms of the  GNU General Public License as published  by the Free Soft- *
 * ware  Foundation;  either version 2,  or (at your option) any later ver- *
 * sion.  GNAT is distributed in the hope that it will be useful, but WITH- *
 * OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License *
 * for  more details.  You should have  received  a copy of the GNU General *
 * Public License  distributed with GNAT;  see file COPYING.  If not, write *
 * to  the  Free Software Foundation,  51  Franklin  Street,  Fifth  Floor, *
 * Boston, MA 02110-1301, USA.                                              *
 *                                                                          *
--
--
--
--
--
--
 * GNAT was originally developed  by the GNAT team at  New York University. *
 * Extensive contributions were provided by Ada Core Technologies Inc.      *
 *                                                                          *
 ****************************************************************************/

/*  This file contains those routines named by Import pragmas in package    */
/*  GNAT.Calendar. It is used to do Duration to timeval conversion.         */
/*  These are simple wrappers function to abstract the fact that the C      */
/*  struct timeval fields type are not normalized (they are generally       */
/*  defined as int or long values).                                         */

#if defined(VMS)

/* this is temporary code to avoid build failure under VMS */

void
__gnat_timeval_to_duration (void *t, long *sec, long *usec)
{
}

void
__gnat_duration_to_timeval (long sec, long usec, void *t)
{
}

#else

#if defined (__vxworks)
#ifdef __RTP__
#include <time.h>
#else
#include <sys/times.h>
#endif
#else
#include <sys/time.h>
#endif

#ifdef __MINGW32__
#include "mingw32.h"
#if STD_MINGW
#include <winsock.h>
#endif
#endif

void
__gnat_timeval_to_duration (struct timeval *t, long *sec, long *usec)
{
  *sec  = (long) t->tv_sec;
  *usec = (long) t->tv_usec;
}

void
__gnat_duration_to_timeval (long sec, long usec, struct timeval *t)
{
  /* here we are doing implicit conversion from a long to the struct timeval
     fields types. */

  t->tv_sec = sec;
  t->tv_usec = usec;
}
#endif

#ifdef __alpha_vxworks
#include "vxWorks.h"
#elif defined (__vxworks)
#include <types/vxTypesOld.h>
#endif

/* Return the value of the "time" C library function.  We always return
   a long and do it this way to avoid problems with not knowing
   what time_t is on the target.  */

long
gnat_time (void)
{
  return time (0);
}
