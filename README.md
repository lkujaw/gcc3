# GNAT 3.4 #

This repository contains a copy of the latest GCC 3.4.x source code
combined with the GNAT GPL 2006 Ada front-end updates. The aim is to
facilitate porting GNAT to additional platforms, as GCC 3.4.x has
few dependencies and is quick to bootstrap.

# Platform Support #

| Platform         | ACATS 95 | Exception Handling | GNAT Runtime Library | Stack Checking | Tasking          |
|------------------|----------|--------------------|----------------------|----------------|------------------|
| Solaris 8 (i386) | 100%     | ZCX                | Shared and static    | Yes (probes)   | Yes (UI Threads) |
| UnixWare 7       | 100%     | ZCX                | Shared and static    | Yes (probes)   | Yes (UI Threads) |

If you would like support for another Unix or Unix-like system that is
not included in upstream GCC, please feel free to open an issue.

# Binaries #

No binaries are provided at present, but if you are having trouble
bootstrapping GNAT, please file a request via a new issue.

# TODO #

* Add ACVC, the Ada '83/'87 test suite, to complement ACATS.
* Complete the partial support for Ada 2005.
* Add support for these other CDE reference platforms:
  * AIX (PowerPC)
  * HP-UX (PA-RISC)
  * Solaris (SPARC)
  * Tru64 (Alpha)
