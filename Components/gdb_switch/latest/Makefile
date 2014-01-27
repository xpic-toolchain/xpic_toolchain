#----- Include the PSDK's WIN32.MAK to pick up defines ------------------------------------
!include <win32.mak>

!IF "$(TARGETOS)" == "WINNT"

SRC=src
MAINFILE=netx10_gdb_switch
OBJDIR=obj
OUTDIR=bin

#---------------------------------------------

# possible debug flags:
# -DGDB_SWITCH_DEBUG
# -DGDB_SWITCH_WINDOWS
myflags =	-DGDB_SWITCH_WINDOWS \
			/wd4305


#---------------------------------------------


#----- OUTDIR is defined in WIN32.MAK This is the name of the destination directory-------
all: $(OUTDIR)\$(MAINFILE).exe

#----- If OUTDIR does not exist, then create directory
$(OUTDIR) :
    if not exist "$(OUTDIR)/$(NULL)" mkdir $(OUTDIR)

$(OBJDIR) :
    if not exist "$(OBJDIR)/$(NULL)" mkdir $(OBJDIR)


OBJ =	$(OBJDIR)\netx10gs_main.obj \
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

# list ok
$(OBJDIR)\netx10gs_threads.obj :	$(SRC)\netx10gs_threads.c \
									$(SRC)\netx10gs_globals.h \
									$(SRC)\netx10gs_sockets.c \
									$(SRC)\netx10gs_sockets.h \
									$(SRC)\netx10gs_handlegdbproto.c \
									$(SRC)\netx10gs_handlegdbproto.h \
									$(SRC)\netx10gs_helper.c \
									$(SRC)\netx10gs_helper.h \
									$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRC)\netx10gs_threads.c


$(OBJDIR)\netx10gs_handleboard.obj :$(SRC)\netx10gs_handleboard.c \
									$(SRC)\netx10gs_handleboard.h \
									$(SRC)\netx10gs_gdbproto.h \
									$(SRC)\netx10gs_globals.h \
									$(SRC)\netx10gs_sockets.c \
									$(SRC)\netx10gs_sockets.h \
									$(SRC)\netx10gs_helper.c \
									$(SRC)\netx10gs_helper.h \
									$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRC)\netx10gs_handleboard.c

# list ok
$(OBJDIR)\netx10gs_handlearm.obj :	$(SRC)\netx10gs_handlearm.c \
									$(SRC)\netx10gs_globals.h \
									$(SRC)\netx10gs_gdbproto.h \
									$(SRC)\netx10gs_sockets.h \
									$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRC)\netx10gs_handlearm.c

# list ok
$(OBJDIR)\netx10gs_handlexpic.obj :	$(SRC)\netx10gs_handlexpic.c \
									$(SRC)\netx10gs_globals.h \
									$(SRC)\netx10gs_gdbproto.h \
									$(SRC)\netx10gs_sockets.h \
									$(SRC)\netx10gs_handlearm.h \
									$(SRC)\netx10gs_helper.h \
									$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRC)\netx10gs_handlexpic.c

$(OBJDIR)\netx10gs_helper.obj :	$(SRC)\netx10gs_helper.c \
								$(SRC)\netx10gs_handlegdbproto.h \
								$(SRC)\netx10gs_globals.h \
								$(SRC)\netx10gs_sockets.h \
								$(OBJDIR)
    $(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRC)\netx10gs_helper.c

$(OBJDIR)\netx10gs_sockets.obj :	$(SRC)\netx10gs_sockets.c \
									$(SRC)\netx10gs_sockets.h \
									$(SRC)\netx10gs_globals.h \
									$(SRC)\netx10gs_gdbproto.h \
									$(SRC)\netx10gs_handlegdbproto.c \
									$(SRC)\netx10gs_handlegdbproto.h \
									$(OBJDIR)
    $(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRC)\netx10gs_sockets.c

$(OBJDIR)\netx10gs_handlegdbproto.obj :	$(SRC)\netx10gs_handlegdbproto.c \
										$(SRC)\netx10gs_handlegdbproto.h \
										$(SRC)\netx10gs_globals.h \
										$(SRC)\netx10gs_helper.h \
										$(SRC)\netx10gs_sockets.h \
										$(SRC)\netx10gs_gdbproto.h \
										$(SRC)\netx10gs_handlearm.h \
										$(SRC)\netx10gs_handlexpic.h \
										$(OBJDIR)
    $(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRC)\netx10gs_handlegdbproto.c
	
$(OBJDIR)\netx10gs_main.obj :	$(SRC)\netx10gs_main.c \
								$(SRC)\netx10gs_globals.h \
								$(SRC)\netx10gs_threads.c \
								$(SRC)\netx10gs_threads.h \
								$(SRC)\netx10gs_helper.c \
								$(SRC)\netx10gs_helper.h \
								$(SRC)\netx10gs_sockets.c \
								$(SRC)\netx10gs_sockets.h \
								$(SRC)\netx10gs_handlexpic.c \
								$(SRC)\netx10gs_handlexpic.h \
								$(OBJDIR)
    $(cc) $(cdebug) $(myflags) $(cflags) /WX /Fo"$(OBJDIR)\\" /Fd"$(OBJDIR)\\" .\$(SRC)\netx10gs_main.c

clean:
	$(CLEANUP)
	if exist $(OBJDIR)/$(NULL) rd /s /q $(OBJDIR)



!ELSE
!MESSAGE Sorry this sample only builds for the Windows NT Platforms
!ENDIF

