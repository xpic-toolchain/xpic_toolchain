#----- Include the PSDK's WIN32.MAK to pick up defines ------------------------------------
!include <win32.mak>

!IF "$(TARGETOS)" == "WINNT"

SRC=src
SRCGDBS=..\..\src
MAINFILE=switchtester
GDBSFILE=netx10_gdb_switch_switchtester
OBJDIR=obj
OUTDIR=bin

myflags =	-DGDB_SWITCHTESTER \
			    -DGDB_SWITCH_DEBUG \
          -DGDB_SWITCH_WINDOWS /wd4305

#----- OUTDIR is defined in WIN32.MAK This is the name of the destination directory-------
all:	$(OUTDIR)\$(MAINFILE).exe \
		$(OUTDIR)\$(GDBSFILE).exe

#----- If OUTDIR does not exist, then create directory
$(OUTDIR) :
    if not exist "$(OUTDIR)/$(NULL)" mkdir $(OUTDIR)

$(OBJDIR) :
    if not exist "$(OBJDIR)/$(NULL)" mkdir $(OBJDIR)


OBJ =	$(OBJDIR)\switchtester.obj \
		$(OBJDIR)\tester_socket.obj \
		$(OBJDIR)\tester_threads.obj \
		$(OBJDIR)\tester_testcases.obj
		
OBJGSBS =	$(OBJDIR)\netx10gs_main.obj \
			$(OBJDIR)\netx10gs_threads.obj \
			$(OBJDIR)\netx10gs_sockets.obj \
			$(OBJDIR)\netx10gs_helper.obj \
			$(OBJDIR)\netx10gs_handlegdbproto.obj \
			$(OBJDIR)\netx10gs_handlexpic.obj \
			$(OBJDIR)\netx10gs_handlearm.obj \
			$(OBJDIR)\netx10gs_handleboard.obj


#--------------------- EXE ---------------------------------------------------------------
#PLEASE REFER TO WIN32.MAK for the different Linker options WIN32.MAK provides

# Build rule for DLL
$(OUTDIR)\$(MAINFILE).exe:  $(OUTDIR) $(OBJ)
	$(link) $(ldebug) $(conflags) /PDB:$(OUTDIR)\$(MAINFILE).pdb -out:$(OUTDIR)\$(MAINFILE).exe  $(OBJ) $(conlibs)

$(OUTDIR)\$(GDBSFILE).exe:  $(OUTDIR) $(OBJGSBS)
	$(link) $(ldebug) $(conflags) /PDB:$(OUTDIR)\$(GDBSFILE).pdb -out:$(OUTDIR)\$(GDBSFILE).exe  $(OBJGSBS) $(conlibs)

#-------------------------------------------------------------------------------
# TESTER
#-------------------------------------------------------------------------------
$(OBJDIR)\switchtester.obj :	$(SRC)\switchtester.c \
								$(SRC)\tester_socket.c \
								$(SRC)\tester_socket.h \
								$(SRC)\tester_threads.c \
								$(SRC)\tester_threads.h \
								$(SRC)\tester_testcases.c \
								$(SRC)\tester_testcases.h \
								$(OBJDIR)
    $(cc) $(cdebug) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" $(SRC)\switchtester.c
	
$(OBJDIR)\tester_socket.obj :	$(SRC)\tester_socket.c \
								$(SRC)\tester_socket.h \
								$(OBJDIR)
    $(cc) $(cdebug) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" $(SRC)\tester_socket.c

$(OBJDIR)\tester_threads.obj :	$(SRC)\tester_threads.c \
								$(SRC)\tester_threads.h \
								$(SRC)\tester_testcases.c \
								$(SRC)\tester_testcases.h \
								$(OBJDIR)
    $(cc) $(cdebug) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" $(SRC)\tester_threads.c

$(OBJDIR)\tester_testcases.obj :	$(SRC)\tester_testcases.c \
									$(SRC)\tester_testcases.h \
									$(SRC)\tester_socket.c \
									$(SRC)\tester_socket.h \
									$(OBJDIR)
    $(cc) $(cdebug) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" $(SRC)\tester_testcases.c



#-------------------------------------------------------------------------------
# GDB SWITCH
#-------------------------------------------------------------------------------
$(OBJDIR)\netx10gs_threads.obj :	$(SRCGDBS)\netx10gs_threads.c \
									$(SRCGDBS)\netx10gs_globals.h \
									$(SRCGDBS)\netx10gs_sockets.c \
									$(SRCGDBS)\netx10gs_sockets.h \
									$(SRCGDBS)\netx10gs_handlegdbproto.c \
									$(SRCGDBS)\netx10gs_handlegdbproto.h \
									$(SRCGDBS)\netx10gs_helper.c \
									$(SRCGDBS)\netx10gs_helper.h \
									$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRCGDBS)\netx10gs_threads.c

$(OBJDIR)\netx10gs_handleboard.obj :	$(SRCGDBS)\netx10gs_handleboard.c \
									$(SRCGDBS)\netx10gs_handleboard.h \
									$(SRCGDBS)\netx10gs_gdbproto.h \
									$(SRCGDBS)\netx10gs_globals.h \
									$(SRCGDBS)\netx10gs_sockets.c \
									$(SRCGDBS)\netx10gs_sockets.h \
									$(SRCGDBS)\netx10gs_helper.c \
									$(SRCGDBS)\netx10gs_helper.h \
									$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRCGDBS)\netx10gs_handleboard.c
	
# list ok
$(OBJDIR)\netx10gs_handlearm.obj :	$(SRCGDBS)\netx10gs_handlearm.c \
									$(SRCGDBS)\netx10gs_globals.h \
									$(SRCGDBS)\netx10gs_gdbproto.h \
									$(SRCGDBS)\netx10gs_sockets.h \
									$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRCGDBS)\netx10gs_handlearm.c

# list ok
$(OBJDIR)\netx10gs_handlexpic.obj :	$(SRCGDBS)\netx10gs_handlexpic.c \
									$(SRCGDBS)\netx10gs_globals.h \
									$(SRCGDBS)\netx10gs_gdbproto.h \
									$(SRCGDBS)\netx10gs_sockets.h \
									$(SRCGDBS)\netx10gs_handlearm.h \
									$(SRCGDBS)\netx10gs_helper.h \
									$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRCGDBS)\netx10gs_handlexpic.c

$(OBJDIR)\netx10gs_helper.obj :	$(SRCGDBS)\netx10gs_helper.c \
								$(SRCGDBS)\netx10gs_handlegdbproto.h \
								$(SRCGDBS)\netx10gs_globals.h \
								$(SRCGDBS)\netx10gs_sockets.h \
								$(OBJDIR)
    $(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRCGDBS)\netx10gs_helper.c

$(OBJDIR)\netx10gs_sockets.obj :	$(SRCGDBS)\netx10gs_sockets.c \
									$(SRCGDBS)\netx10gs_sockets.h \
									$(SRCGDBS)\netx10gs_globals.h \
									$(SRCGDBS)\netx10gs_gdbproto.h \
									$(SRCGDBS)\netx10gs_handlegdbproto.c \
									$(SRCGDBS)\netx10gs_handlegdbproto.h \
									$(OBJDIR)
    $(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRCGDBS)\netx10gs_sockets.c

$(OBJDIR)\netx10gs_handlegdbproto.obj :	$(SRCGDBS)\netx10gs_handlegdbproto.c \
										$(SRCGDBS)\netx10gs_handlegdbproto.h \
										$(SRCGDBS)\netx10gs_globals.h \
										$(SRCGDBS)\netx10gs_helper.h \
										$(SRCGDBS)\netx10gs_sockets.h \
										$(SRCGDBS)\netx10gs_gdbproto.h \
										$(SRCGDBS)\netx10gs_handlearm.h \
										$(SRCGDBS)\netx10gs_handlexpic.h \
										$(OBJDIR)
    $(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRCGDBS)\netx10gs_handlegdbproto.c
	
$(OBJDIR)\netx10gs_main.obj :	$(SRCGDBS)\netx10gs_main.c \
								$(SRCGDBS)\netx10gs_globals.h \
								$(SRCGDBS)\netx10gs_threads.c \
								$(SRCGDBS)\netx10gs_threads.h \
								$(SRCGDBS)\netx10gs_helper.c \
								$(SRCGDBS)\netx10gs_helper.h \
								$(SRCGDBS)\netx10gs_sockets.c \
								$(SRCGDBS)\netx10gs_sockets.h \
								$(SRCGDBS)\netx10gs_handlexpic.c \
								$(SRCGDBS)\netx10gs_handlexpic.h \
								$(OBJDIR)
    $(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRCGDBS)\netx10gs_main.c

clean:
	$(CLEANUP)
	if exist $(OBJDIR)/$(NULL) rd /s /q $(OBJDIR)



!ELSE
!MESSAGE Sorry this sample only builds for the Windows NT Platforms
!ENDIF

