!****************************************************************
!                                                               *
!                      VMS GDB KITINSTAL.COM                    *
!                                                               *
!****************************************************************
$ !
$ !         Take care of interrupts
$ !
$ ON CONTROL_Y THEN VMI$CALLBACK CONTROL_Y
$ !
$ !            Process errors
$ !
$ ON WARNING THEN GOTO ERR_EXIT
$ !
$ !         Determine course of action
$ !
$ IF P1 .EQS. "VMI$_INSTALL" THEN GOTO GDB_INSTALL
$ IF P1 .EQS. "VMI$_POSTINSTALL" THEN GOTO GDB_POSTINSTALL
$ IF P1 .EQS. "VMI$_IVP" THEN GOTO GDB_IVP
$ IF P1 .EQS. "HELP1" THEN GOTO HELP1
$ EXIT VMI$_UNSUPPORTED
$ !
$ HELP1:
$ TYPE SYS$INPUT

  There are basically 2 options for installation:

  1) Install GDB in a standard location.
     (Note: This includes directories under SYS$COMMON:[SYSLIB].)

  2) Install GDB in a non-standard location that you will specify.
     (Note: This directory must already exist, the installation procedure
            will create subdirectories under this directory to hold the
            installed files.)

$ EXIT VMI$_SUCCESS
$ !
$ !         Install the product
$ !
$ GDB_INSTALL:
$ GDB_DEST0=""
$ !
$ !       Check that OpenVMS version is 7.1 or later for OpenVMS AXP
$ !
$ VMI$CALLBACK CHECK_VMS_VERSION GDB_VERSION 7.1
$ IF .NOT. GDB_VERSION
$ THEN
$   VMI$CALLBACK MESSAGE E VERSION -
       "This kit must be installed on OpenVMS/Alpha 7.1 or above system."
$   EXIT VMI$_FAILURE
$ ENDIF
$ !
$ !             Check for disk space, warn if not enough
$ !
$ VMI$CALLBACK CHECK_NET_UTILIZATION GNAT_SPACE 22500 12500 11000
$ IF .NOT. GNAT_SPACE
$ THEN
$   VMI$CALLBACK MESSAGE W DISKSPACE -
        "Insufficient disk space available for GDB installation on"
$   VMI$CALLBACK MESSAGE W DISKSPACE -
        "ROOT device. You must either install in a non-standard"
$   VMI$CALLBACK MESSAGE W DISKSPACE -
        "directory or choose the option to delete an existing GDB"
$   VMI$CALLBACK MESSAGE W DISKSPACE -
        "installation. Proceed at your own risk."
$   VMI$CALLBACK ASK GDB_CONT "Continue" "NO" B
$   IF .NOT. GDB_CONT THEN EXIT VMI$_FAILURE
$ ENDIF
$ VMI$CALLBACK SET SAFETY NO
$ GOTO MUSTEXIST
$ !
$ MAYBEABORT:
$ VMI$CALLBACK ASK GDB_ABORT "Abort installation" "NO" B
$ IF GDB_ABORT THEN EXIT VMI$_FAILURE
$ !
$ !             Ask for install directory
$ !
$ MUSTEXIST:
$ IF GDB_DEST0.EQS."" THEN GDB_DEST0="SYS$COMMON:[SYSLIB.GNAT]"
$ VMI$CALLBACK ASK GDB_DEST "Installation Directory" "''GDB_DEST0'" U -
    "@VMI$KWD:KITINSTAL HELP1"
$ IF GDB_DEST.EQS."SYS$COMMON:[SYSLIB.GNAT]"
$ THEN
$   GDB_DEST0=GDB_DEST
$   GDB_GCCLIB="SYS$COMMON:[SYSLIB.GNAT]"
$   GDB_GCCLIBBIN="SYS$COMMON:[SYSLIB.GNAT.BIN]"
$   GDB_GCCLIBDOTDOTDOT="SYS$COMMON:[SYSLIB.GNAT...]"
$ ELSE
$   IF f$parse(GDB_DEST).EQS.""
$   THEN
$     VMI$CALLBACK MESSAGE E NODIRECTORY "User device and directory must exist"
$     GOTO MUSTEXIST
$   ENDIF
$   GDB_DEST0=GDB_DEST
$   GNAT_BASEDEV=f$parse(GDB_DEST,,,"DEVICE")
$   GNAT_BASEDIR=f$parse(GDB_DEST,,,"DIRECTORY")
$   GDB_GCCLIB=f$string(GNAT_BASEDEV+f$extract(0,f$length(GNAT_BASEDIR)-1,GNAT_BASEDIR)+"]")
$   GDB_GCCLIBBIN=f$string(GNAT_BASEDEV+f$extract(0,f$length(GNAT_BASEDIR)-1,GNAT_BASEDIR)+".BIN]")
$   GDB_GCCLIBDOTDOTDOT=f$string(f$extract(0,f$length(GDB_GCCLIB)-1,GDB_GCCLIB)+"...]")
$ ENDIF
$ GDB_GCCLIBSTARTUP=GDB_GCCLIB+"GNAT_STARTUP.COM"
$ IF f$search(GDB_GCCLIBSTARTUP).EQS.""
$ THEN
$   VMI$CALLBACK MESSAGE E NOGNAT "Must be installed over GNAT"
$   GOTO MUSTEXIST
$ ENDIF
$ !
$ !             Restore the GDB saveset to the selected directory
$ !
$ IF f$parse (GDB_GCCLIB).EQS.""
$ THEN
$   VMI$CALLBACK MESSAGE I CREATE "Creating directory ''GDB_GCCLIB'"
$   VMI$CALLBACK CREATE_DIRECTORY USER 'GDB_GCCLIB'
$ ENDIF
$ !
$ VMI$CALLBACK MESSAGE I RESTORE "Installing new files, this may take several minutes"
$ BACKUP VMI$KWD:GDB.BCK/SAVE/select=[gnatmail.build-gdb.gcclib...] 'GDB_GCCLIBDOTDOTDOT'/NEW_VERSION
$ PURGE 'GDB_GCCLIBDOTDOTDOT'
$ !
$ !       Specifiy IVP to run
$ !
$ VMI$CALLBACK SET IVP YES
$ !
$ !       Move the help file
$ !
$ VMI$CALLBACK PROVIDE_DCL_HELP GDB.HLP
$ !  
$ !       Create IVP dirctory
$ !
$ VMI$CALLBACK CREATE_DIRECTORY COMMON SYSTEST.GDB /PROTECTION=(S:RWE,O:RWE,G:RWE,W:RWE)
$ !
$ !       Move IVP file
$ !
$ VMI$CALLBACK PROVIDE_FILE GDB_IVP GDB_IVP.COM VMI$ROOT:[SYSTEST]
$ !
$ !       Identify the startup command file
$ !
$ IF GDB_DEST.NES."SYS$COMMON:[SYSLIB]"
$ THEN
$   VMI$CALLBACK SET STARTUP GNAT$STARTUP.COM "''GDB_GCCLIB'"
$ ELSE
$   VMI$CALLBACK SET STARTUP GNAT$STARTUP.COM
$ ENDIF
$ !
$ !       Installation completed, exit
$ !
$ EXIT VMI$_SUCCESS
$ !
$ ! Post-install phase
$ !
$ GDB_POSTINSTALL:
$ !
$ !      No postinstall work
$ !
$ EXIT VMS$_SUCCESS
$ !
$ ! Verify installation
$ !
$ GDB_IVP:
$ !
$ !       run the ivp
$ !
$ @SYS$TEST:GDB_IVP
$ !
$ !       ivp completed, indicate results
$ !
$ EXIT $STATUS
$ !
$ ERR_EXIT:
$ EXIT VMI$_FAILURE
