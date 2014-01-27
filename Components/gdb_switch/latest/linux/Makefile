#----- Include the PSDK's WIN32.MAK to pick up defines ------------------------------------

cc = gcc
link = gcc
SRC = ../src
MAINFILE = netx10_gdb_switch
OBJDIR = obj
OUTDIR = bin

#---------------------------------------------

# possible debug flags:
# -DGDB_SWITCH_DEBUG

myflags =	-DGDB_SWITCH_LINUX \
			-DGDB_SWITCH_DEBUG

cflags =	-pthread -c -g
conflags = -pthread
#---------------------------------------------


#----- OUTDIR is defined in WIN32.MAK This is the name of the destination directory-------
all: $(OUTDIR)/$(MAINFILE)

#----- If OUTDIR does not exist, then create directory
$(OUTDIR):
	mkdir $(OUTDIR)

$(OBJDIR):
	mkdir $(OBJDIR)


OBJ =	$(OBJDIR)/netx10gs_main.o \
		$(OBJDIR)/netx10gs_threads.o \
		$(OBJDIR)/netx10gs_sockets.o \
		$(OBJDIR)/netx10gs_helper.o \
		$(OBJDIR)/netx10gs_handlegdbproto.o \
		$(OBJDIR)/netx10gs_handlexpic.o \
		$(OBJDIR)/netx10gs_handlearm.o

$(OUTDIR)/$(MAINFILE):  $(OUTDIR) $(OBJ)
	$(link) $(ldebug) $(conflags) -o$(OUTDIR)/$(MAINFILE)  $(OBJ) $(conlibs)

$CPU == i386
$(OUTDIR)/$(MAINFILE):  $(OUTDIR) $(OBJ)
	$(link) $(ldebug) $(conflags) -o$(OUTDIR)/$(MAINFILE)  $(OBJ) $(conlibs)

	
# list ok
$(OBJDIR)/netx10gs_threads.o :	$(SRC)/netx10gs_threads.c \
									$(SRC)/netx10gs_globals.h \
									$(SRC)/netx10gs_sockets.c \
									$(SRC)/netx10gs_sockets.h \
									$(SRC)/netx10gs_handlegdbproto.c \
									$(SRC)/netx10gs_handlegdbproto.h \
									$(SRC)/netx10gs_helper.c \
									$(SRC)/netx10gs_helper.h \
									$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) $(SRC)/netx10gs_threads.c -o $(OBJDIR)/netx10gs_threads.o

# list ok
$(OBJDIR)/netx10gs_handlearm.o :	$(SRC)/netx10gs_handlearm.c \
									$(SRC)/netx10gs_globals.h \
									$(SRC)/netx10gs_gdbproto.h \
									$(SRC)/netx10gs_sockets.h \
									$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) $(SRC)/netx10gs_handlearm.c -o $(OBJDIR)/netx10gs_handlearm.o

# list ok
$(OBJDIR)/netx10gs_handlexpic.o :	$(SRC)/netx10gs_handlexpic.c \
									$(SRC)/netx10gs_globals.h \
									$(SRC)/netx10gs_gdbproto.h \
									$(SRC)/netx10gs_sockets.h \
									$(SRC)/netx10gs_handlearm.h \
									$(SRC)/netx10gs_helper.h \
									$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) $(SRC)/netx10gs_handlexpic.c -o $(OBJDIR)/netx10gs_handlexpic.o

$(OBJDIR)/netx10gs_helper.o :	$(SRC)/netx10gs_helper.c \
								$(SRC)/netx10gs_handlegdbproto.h \
								$(SRC)/netx10gs_globals.h \
								$(SRC)/netx10gs_sockets.h \
								$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) $(SRC)/netx10gs_helper.c -o $(OBJDIR)/netx10gs_helper.o

$(OBJDIR)/netx10gs_sockets.o :	$(SRC)/netx10gs_sockets.c \
									$(SRC)/netx10gs_sockets.h \
									$(SRC)/netx10gs_globals.h \
									$(SRC)/netx10gs_gdbproto.h \
									$(SRC)/netx10gs_handlegdbproto.c \
									$(SRC)/netx10gs_handlegdbproto.h \
									$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) $(SRC)/netx10gs_sockets.c -o $(OBJDIR)/netx10gs_sockets.o

$(OBJDIR)/netx10gs_handlegdbproto.o :	$(SRC)/netx10gs_handlegdbproto.c \
										$(SRC)/netx10gs_handlegdbproto.h \
										$(SRC)/netx10gs_globals.h \
										$(SRC)/netx10gs_helper.h \
										$(SRC)/netx10gs_sockets.h \
										$(SRC)/netx10gs_gdbproto.h \
										$(SRC)/netx10gs_handlearm.h \
										$(SRC)/netx10gs_handlexpic.h \
										$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) $(SRC)/netx10gs_handlegdbproto.c -o $(OBJDIR)/netx10gs_handlegdbproto.o
	
$(OBJDIR)/netx10gs_main.o :	$(SRC)/netx10gs_main.c \
								$(SRC)/netx10gs_globals.h \
								$(SRC)/netx10gs_threads.c \
								$(SRC)/netx10gs_threads.h \
								$(SRC)/netx10gs_helper.c \
								$(SRC)/netx10gs_helper.h \
								$(SRC)/netx10gs_sockets.c \
								$(SRC)/netx10gs_sockets.h \
								$(SRC)/netx10gs_handlexpic.c \
								$(SRC)/netx10gs_handlexpic.h \
								$(OBJDIR)
	$(cc) $(cdebug) $(myflags) $(cflags) $(SRC)/netx10gs_main.c -o $(OBJDIR)/netx10gs_main.o

clean:
	rm -r $(OBJDIR)