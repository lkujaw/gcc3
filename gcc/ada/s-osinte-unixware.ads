------------------------------------------------------------------------------
--                                                                          --
--                 GNAT RUN-TIME LIBRARY (GNARL) COMPONENTS                 --
--                                                                          --
--                   S Y S T E M . O S _ I N T E R F A C E                  --
--                                                                          --
--                                  S p e c                                 --
--                                                                          --
--             Copyright (C) 1991-1994, Florida State University            --
--             Copyright (C) 1995-2006, Free Software Foundation, Inc.      --
--                                                                          --
-- GNARL is free software; you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 2,  or (at your option) any later ver- --
-- sion. GNARL is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License --
-- for  more details.  You should have  received  a copy of the GNU General --
-- Public License  distributed with GNARL; see file COPYING.  If not, write --
-- to  the  Free Software Foundation,  51  Franklin  Street,  Fifth  Floor, --
-- Boston, MA 02110-1301, USA.                                              --
--                                                                          --
-- As a special exception,  if other files  instantiate  generics from this --
-- unit, or you link  this unit with other files  to produce an executable, --
-- this  unit  does not  by itself cause  the resulting  executable  to  be --
-- covered  by the  GNU  General  Public  License.  This exception does not --
-- however invalidate  any other reasons why  the executable file  might be --
-- covered by the  GNU Public License.                                      --
--                                                                          --
-- GNARL was developed by the GNARL team at Florida State University.       --
-- Extensive contributions were provided by Ada Core Technologies, Inc.     --
--                                                                          --
------------------------------------------------------------------------------

--  This is a UnixWare (native threads) version of this package

--  This package includes all direct interfaces to OS services
--  that are needed by children of System.

--  PLEASE DO NOT add any with-clauses to this package or remove the pragma
--  Preelaborate. This package is designed to be a bottom-level (leaf) package.

with Interfaces.C;
with Unchecked_Conversion;

package System.OS_Interface is
   pragma Preelaborate;

   pragma Linker_Options ("-lthread");

   subtype int            is Interfaces.C.int;
   subtype char           is Interfaces.C.char;
   subtype short          is Interfaces.C.short;
   subtype long           is Interfaces.C.long;
   subtype unsigned       is Interfaces.C.unsigned;
   subtype unsigned_short is Interfaces.C.unsigned_short;
   subtype unsigned_long  is Interfaces.C.unsigned_long;
   subtype unsigned_char  is Interfaces.C.unsigned_char;
   subtype plain_char     is Interfaces.C.plain_char;
   subtype size_t         is Interfaces.C.size_t;

   -----------
   -- Errno --
   -----------

   function errno return int;
   pragma Import (C, errno, "__get_errno");

   EAGAIN    : constant := 11;
   EINTR     : constant := 4;
   EINVAL    : constant := 22;
   ENOMEM    : constant := 12;
   ETIME     : constant := 62;
   ETIMEDOUT : constant := 145;

   -------------
   -- Signals --
   -------------

   Max_Interrupt : constant := 64;
   type Signal is new int range 0 .. Max_Interrupt;
   for Signal'Size use int'Size;

   SIGHUP     : constant := 1; --  hangup
   SIGINT     : constant := 2; --  interrupt (rubout)
   SIGQUIT    : constant := 3; --  quit (ASCD FS)
   SIGILL     : constant := 4; --  illegal instruction (not reset)
   SIGTRAP    : constant := 5; --  trace trap (not reset)
   SIGIOT     : constant := 6; --  IOT instruction
   SIGABRT    : constant := 6; --  used by abort, replace SIGIOT in the future
   SIGEMT     : constant := 7; --  EMT instruction
   SIGFPE     : constant := 8; --  floating point exception
   SIGKILL    : constant := 9; --  kill (cannot be caught or ignored)
   SIGBUS     : constant := 10; --  bus error
   SIGSEGV    : constant := 11; --  segmentation violation
   SIGSYS     : constant := 12; --  bad argument to system call
   SIGPIPE    : constant := 13; --  write on a pipe with no one to read it
   SIGALRM    : constant := 14; --  alarm clock
   SIGTERM    : constant := 15; --  software termination signal from kill
   SIGUSR1    : constant := 16; --  user defined signal 1
   SIGUSR2    : constant := 17; --  user defined signal 2
   SIGCLD     : constant := 18; --  alias for SIGCHLD
   SIGCHLD    : constant := 18; --  child status change
   SIGPWR     : constant := 19; --  power-fail restart
   SIGWINCH   : constant := 20; --  window size change
   SIGURG     : constant := 21; --  urgent condition on IO channel
   SIGPOLL    : constant := 22; --  pollable event occurred
   SIGIO      : constant := 22; --  I/O possible (SIGPOLL alias)
   SIGSTOP    : constant := 23; --  stop (cannot be caught or ignored)
   SIGTSTP    : constant := 24; --  user stop requested from tty
   SIGCONT    : constant := 25; --  stopped process has been continued
   SIGTTIN    : constant := 26; --  background tty read attempted
   SIGTTOU    : constant := 27; --  background tty write attempted
   SIGVTALRM  : constant := 28; --  virtual timer expired
   SIGPROF    : constant := 29; --  profiling timer expired
   SIGXCPU    : constant := 30; --  CPU time limit exceeded
   SIGXFSZ    : constant := 31; --  filesize limit exceeded
   SIGWAITING : constant := 32; --  all LWPs blocked interruptibly notific.
   SIGLWP     : constant := 33; --  signal reserved for thread lib impl.
   SIGAIO     : constant := 34; --  Asynchronous I/O signal
   SIGMIGRATE : constant := 35; --  SSI - migrate process
   SIGCLUSTER : constant := 36; --  SSI - cluster reconfiguration

   type Signal_Set is array (Natural range <>) of Signal;

   Unmasked    : constant Signal_Set := (
      SIGTRAP,
      --  To enable debugging on multithreaded applications, mark SIGTRAP to
      --  be kept unmasked.

      SIGLWP, SIGWAITING,

      SIGTTIN, SIGTTOU, SIGTSTP,
      --  Keep these three signals unmasked so that background processes
      --  and IO behaves as normal "C" applications

      SIGPROF,
      --  To avoid confusing the profiler.

      SIGKILL, SIGSTOP, SIGCONT);
      --  UnixWare does not permit these signals to be masked.

   Reserved    : constant Signal_Set := (0 => SIGABRT);
   --  SIGABRT is used by the runtime to abort tasks

   type sigset_t is private;

   function sigaddset (set : access sigset_t; sig : Signal) return int;
   pragma Import (C, sigaddset, "sigaddset");

   function sigdelset (set : access sigset_t; sig : Signal) return int;
   pragma Import (C, sigdelset, "sigdelset");

   function sigfillset (set : access sigset_t) return int;
   pragma Import (C, sigfillset, "sigfillset");

   function sigismember (set : access sigset_t; sig : Signal) return int;
   pragma Import (C, sigismember, "sigismember");

   function sigemptyset (set : access sigset_t) return int;
   pragma Import (C, sigemptyset, "sigemptyset");

   type union_type_3 is new String (1 .. 116);
   type siginfo_t is record
      si_signo     : int;
      si_code      : int;
      si_errno     : int;
      X_data       : union_type_3;
   end record;
   for siginfo_t'Size use 128 * 8;
   pragma Convention (C, siginfo_t);

   FPE_INTDIV  : constant := 1; --  integer divide by zero
   FPE_INTOVF  : constant := 2; --  integer overflow
   FPE_FLTDIV  : constant := 3; --  floating point divide by zero
   FPE_FLTOVF  : constant := 4; --  floating point overflow
   FPE_FLTUND  : constant := 5; --  floating point underflow
   FPE_FLTRES  : constant := 6; --  floating point inexact result
   FPE_FLTINV  : constant := 7; --  invalid floating point operation
   FPE_FLTSUB  : constant := 8; --  subscript out of range

   --  From <sys/regset.h>:
   type greg_t is new int;

   type gregset_t is array (0 .. 18) of greg_t;
   for gregset_t'Size use 76 * 8;

   type union_fp_reg_set is new String (1 .. 248);
   type array_f_wregs is array (0 .. 32) of long;
   type fpregset_t is record
      fp_reg_set : union_fp_reg_set;
      f_wregs    : array_f_wregs;
   end record;
   for fpregset_t'Size use 380 * 8;
   pragma Convention (C, fpregset_t);

   type array_type_7 is array (Integer range 0 .. 20) of long;
   type mcontext_t is record
      gregs  : gregset_t;
      fpregs : fpregset_t;
   end record;
   for mcontext_t'Size use 456 * 8;
   pragma Convention (C, mcontext_t);

   type stack_t is record
      ss_sp    : System.Address;
      ss_size  : size_t;
      ss_flags : int;
   end record;
   for stack_t'Size use 12 * 8;
   pragma Convention (C, stack_t);

   type array_uc_filler is array (Integer range 0 .. 2) of long;
   type ucontext_t is record
      uc_flags        : unsigned_long;
      uc_link         : System.Address;
      uc_sigmask      : sigset_t;
      uc_stack        : stack_t;
      uc_mcontext     : mcontext_t;
      uc_privatedatap : System.Address;
      uc_ucontextp    : System.Address;
      uc_filler       : array_uc_filler;
   end record;
   pragma Convention (C, ucontext_t);

   type array_sa_resv is array (Integer range 0 .. 1) of int;
   type struct_sigaction is record
      sa_flags   : int;
      sa_handler : System.Address;
      sa_mask    : sigset_t;
      sa_resv    : array_sa_resv;
   end record;
   pragma Convention (C, struct_sigaction);
   type struct_sigaction_ptr is access all struct_sigaction;

   SIG_BLOCK   : constant := 1;
   SIG_UNBLOCK : constant := 2;
   SIG_SETMASK : constant := 3;

   SIG_DFL : constant := 0;
   SIG_IGN : constant := 1;

   function sigaction
     (sig  : Signal;
      act  : struct_sigaction_ptr;
      oact : struct_sigaction_ptr) return int;
   pragma Import (C, sigaction, "sigaction");

   ----------
   -- Time --
   ----------

   type timespec is private;

   type clockid_t is private;

   CLOCK_REALTIME : constant clockid_t;

   function clock_gettime
     (clock_id : clockid_t;
      tp       : access timespec) return int;
   --  UnixWare threads don't have clock_gettime
   --  We instead use gettimeofday()

   function To_Duration (TS : timespec) return Duration;
   pragma Inline (To_Duration);

   function To_Timespec (D : Duration) return timespec;
   pragma Inline (To_Timespec);

   type struct_timeval is private;
   --  This is needed on systems that do not have clock_gettime()
   --  but do have gettimeofday().

   function To_Duration (TV : struct_timeval) return Duration;
   pragma Inline (To_Duration);

   function To_Timeval (D : Duration) return struct_timeval;
   pragma Inline (To_Timeval);

   -------------
   -- Process --
   -------------

   type pid_t is private;

   function kill (pid : pid_t; sig : Signal) return int;
   pragma Import (C, kill, "kill");

   function getpid return pid_t;
   pragma Import (C, getpid, "getpid");

   -------------
   -- Threads --
   -------------

   type Thread_Body is access
     function (arg : System.Address) return System.Address;

   function Thread_Body_Access is new
     Unchecked_Conversion (System.Address, Thread_Body);

   --  From <thread.h>:
   THR_DETACHED  : constant := 8;
   THR_BOUND     : constant := 2;
   THR_NEW_LWP   : constant := 4;
   --  From <synch.h>
   USYNC_THREAD  : constant := 0;

   type thread_t is new long;
   subtype Thread_Id is thread_t;

   type mutex_t is limited private;

   type cond_t is limited private;

   type thread_key_t is private;

   function thr_create
     (stack_base    : System.Address;
      stack_size    : size_t;
      start_routine : Thread_Body;
      arg           : System.Address;
      flags         : long;
      new_thread    : access thread_t) return int;
   pragma Import (C, thr_create);

   function thr_self return thread_t;
   pragma Import (C, thr_self);

   function mutex_init
     (mutex : access mutex_t;
      mtype : int;
      arg   : System.Address) return int;
   pragma Import (C, mutex_init);

   function mutex_destroy (mutex : access mutex_t) return int;
   pragma Import (C, mutex_destroy);

   function mutex_lock (mutex : access mutex_t) return int;
   pragma Import (C, mutex_lock);

   function mutex_unlock (mutex : access mutex_t) return int;
   pragma Import (C, mutex_unlock);

   function cond_init
     (cond  : access cond_t;
      ctype : int;
      arg   : System.Address) return int;
   pragma Import (C, cond_init);

   function cond_wait
     (cond : access cond_t; mutex : access mutex_t) return int;
   pragma Import (C, cond_wait);

   function cond_timedwait
     (cond    : access cond_t;
      mutex   : access mutex_t;
      abstime : access timespec) return int;
   pragma Import (C, cond_timedwait);

   function cond_signal (cond : access cond_t) return int;
   pragma Import (C, cond_signal);

   function cond_destroy (cond : access cond_t) return int;
   pragma Import (C, cond_destroy);

   function thr_setspecific
     (key : thread_key_t; value : System.Address) return int;
   pragma Import (C, thr_setspecific);

   function thr_getspecific
     (key   : thread_key_t;
      value : access System.Address) return int;
   pragma Import (C, thr_getspecific);

   function thr_keycreate
     (key : access thread_key_t; destructor : System.Address) return int;
   pragma Import (C, thr_keycreate);

   function thr_setprio (thread : thread_t; priority : int) return int;
   pragma Import (C, thr_setprio);

   procedure thr_exit (status : System.Address);
   pragma Import (C, thr_exit);

   function thr_setconcurrency (new_level : int) return int;
   pragma Import (C, thr_setconcurrency);

   function sigwait (set : access sigset_t; sig : access Signal) return int;
   pragma Inline (sigwait);
   --  UnixWare provides a non standard sigwait

   function thr_kill (thread : thread_t; sig : Signal) return int;
   pragma Import (C, thr_kill);

   function thr_sigsetmask
     (how  : int;
      set  : access sigset_t;
      oset : access sigset_t) return int;
   pragma Import (C, thr_sigsetmask);

   function pthread_sigmask
     (how  : int;
      set  : access sigset_t;
      oset : access sigset_t) return int;
   pragma Import (C, pthread_sigmask);

   function thr_suspend (target_thread : thread_t) return int;
   pragma Import (C, thr_suspend);

   function thr_continue (target_thread : thread_t) return int;
   pragma Import (C, thr_continue);

   procedure thr_yield;
   pragma Import (C, thr_yield);

   ---------
   -- LWP --
   ---------

   P_PID   : constant := 0;
   P_LWPID : constant := 8;

   --  From <sys/priocntl.h>
   PC_GETCID    : constant := 0;
   PC_GETCLINFO : constant := 1;
   PC_SETPARMS  : constant := 2;
   PC_GETPARMS  : constant := 3;
   PC_ADMIN     : constant := 4;

   PC_CLNULL : constant := -1;

   --  From <sys/rtpriocntl.h>
   RT_NOCHANGE : constant := -1;
   RT_TQINF    : constant := -2;
   RT_TQDEF    : constant := -3;

   PC_CLNMSZ   : constant := 16;
   PC_CLINFOSZ : constant := 256 / long'Size;
   PC_CLPARMSZ : constant := 256 / long'Size;

   PC_VERSION : constant := 1;

   type lwpid_t is new long;

   type id_t is new long;

   P_MYID : constant := -1;
   --  the specified LWP or process is the current one.

   type struct_pcinfo is record
      pc_cid    : id_t;
      pc_clname : String (1 .. PC_CLNMSZ);
      rt_maxpri : short;
      filler    : String (1 .. 30);
   end record;
   for struct_pcinfo'Size use 52 * 8;
   pragma Convention (C, struct_pcinfo);

   type struct_pcparms is record
      pc_cid     : id_t;
      rt_pri     : short;
      rt_tqsecs  : long;
      rt_tqnsecs : long;
      filler     : String (1 .. 20);
   end record;
   for struct_pcparms'Size use 36 * 8;
   pragma Convention (C, struct_pcparms);

   function priocntl
     (id_type : int;
      id      : id_t;
      cmd     : int;
      arg     : System.Address) return long;
   pragma Import (C, priocntl);

   function lwp_self return lwpid_t;
   pragma Import (C, lwp_self, "_lwp_self");

   type processorid_t is new int;
   type processorid_t_ptr is access all processorid_t;

   --  Constants for function processor_bind

   PBIND_QUERY : constant processorid_t := -2;
   --  the processor bindings are not changed.

   PBIND_NONE  : constant processorid_t := -1;
   --  the processor bindings of the specified LWPs are cleared.

   --  Flags for function p_online

   --  P_* are renamed PR_* to avoid a clash with p_online.
   PR_OFFLINE : constant int := 2;
   --  processor is offline, as quiet as possible

   PR_ONLINE  : constant int := 1;
   --  processor online

   PR_STATUS  : constant int := 4;
   --  value passed to p_online to request status

   function p_online (processorid : processorid_t; flag : int) return int;
   pragma Import (C, p_online);

   function processor_bind
     (id_type : int;
      id      : id_t;
      proc_id : processorid_t;
      obind   : processorid_t_ptr) return int;
   pragma Import (C, processor_bind, "processor_bind");

   procedure pthread_init;
   --  This is a dummy procedure to share s-intman.adb.

private

   type sigbit_array is array (0 .. 3) of unsigned;
   type sigset_t is record
      sa_sigbits : sigbit_array;
   end record;
   for sigset_t'Size use 16 * 8;
   pragma Convention (C_Pass_By_Copy, sigset_t);

   type pid_t is new long;

   type time_t is new long;

   for ucontext_t'Size use 512 * 8;
   for struct_sigaction'Size use 32 * 8;

   type timespec is record
      tv_sec  : time_t;
      tv_nsec : long;
   end record;
   for timespec'Size use 8 * 8;
   pragma Convention (C, timespec);

   type clockid_t is new int;
   CLOCK_REALTIME : constant clockid_t := 0;

   type struct_timeval is record
      tv_sec  : long;
      tv_usec : long;
   end record;
   for struct_timeval'Size use 8 * 8;
   pragma Convention (C, struct_timeval);

   type thrq_elt_t;
   type thrq_elt_t_ptr is access all thrq_elt_t;

   type thrq_elt_t is record
      thrq_next : thrq_elt_t_ptr;
      thrq_prev : thrq_elt_t_ptr;
   end record;
   for thrq_elt_t'Size use 8 * 8;
   pragma Convention (C, thrq_elt_t);

   type lwp_mutex_t is record
      wanted : char;
      lock   : unsigned_char;
   end record;
   for lwp_mutex_t'Size use 2 * 8;
   pragma Convention (C, lwp_mutex_t);
   pragma Volatile (lwp_mutex_t);

   type array_mutex_filler is array (Integer range 0 .. 1) of int;
   type mutex_t is record
      m_lmutex    : lwp_mutex_t;
      m_sync_lock : lwp_mutex_t;
      m_type      : int;
      m_sleepq    : thrq_elt_t;
      filler      : array_mutex_filler;
   end record;
   for mutex_t'Size use 24 * 8;
   pragma Convention (C, mutex_t);
   pragma Volatile (mutex_t);

   type lwp_cond_t is record
      wanted : char;
   end record;
   for lwp_cond_t'Size use 1 * 8;
   pragma Convention (C, lwp_cond_t);
   pragma Volatile (lwp_cond_t);

   type cond_t is record
      c_lcond     : lwp_cond_t;
      c_sync_lock : lwp_mutex_t;
      c_type      : int;
      c_syncq     : thrq_elt_t;
   end record;
   for cond_t'Size use 16 * 8;
   pragma Convention (C, cond_t);
   pragma Volatile (cond_t);

   type thread_key_t is new unsigned;

end System.OS_Interface;
