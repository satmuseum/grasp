#------------------------------------------------------------------------------
# Global Makefile for the software directory.
#
# Date:		03/20/96
# Author:	jpms.
#------------------------------------------------------------------------------

include $(GRASP_PATH)/Make_defs.include

HDRFILES = `find $(SRCDIR) -name ".*" -prune -o -name "*.hh" -print`
SRCFILES = `find $(SRCDIR) -name ".*" -prune -o -name "*.cc" -print`
LIBFILES = `find $(SRCDIR) -name ".*" -prune -o -name "*.a" -print`
MANFILES = `find $(SRCDIR) -name ".*" -prune -o -name "*.1" -print`
DOCFILES = `find $(SRCDIR) -name ".*" -prune -o -name "*.doc" -print`
TXTFILES = `find $(SRCDIR) -name ".*" -prune -o -name "*.txt" -print`
MKFILES = `find $(SOFTDIR) -name ".*" -prune -o -name "Make*" -print`

BCKFILES = $(HDRFILES) $(SRCFILES) $(MANFILES) $(DOCFILES) \
	   $(TXTFILES) $(MKFILES)


DATE	= `date '+%m.%d.%y'`
RANGE	= -1

#-----------------------------------------------------------------------------

all: hlinks all_libs llinks all_apps mlinks dlinks blinks

distr: tar_src

install: all


all_libs: clean_obj libs clean_obj libs_chk clean_obj libs_dbg \
	  clean_obj libs_prof clean_obj

all_apps: clean_obj apps clean_obj apps_chk clean_obj apps_dbg \
	  clean_obj apps_prof clean_obj


libs:
	(cd src; echo Making libs ...;\
	$(MAKE) $(MFLAGS) libs_o)

libs_chk:
	(cd src; echo Making libs_chk ...;\
	$(MAKE) $(MFLAGS) libs_chk)

libs_dbg:
	(cd src; echo Making libs_dbg ...;\
	$(MAKE) $(MFLAGS) libs_dbg)

libs_prof:
	(cd src; echo Making libs_prof ...;\
	$(MAKE) $(MFLAGS) libs_prof)


apps:
	(cd src; echo Making apps ...;\
	$(MAKE) $(MFLAGS) apps_o)

apps_chk:
	(cd src; echo Making apps_chk ...;\
	$(MAKE) $(MFLAGS) apps_chk)

apps_dbg:
	(cd src; echo Making apps_dbg ...;\
	$(MAKE) $(MFLAGS) apps_dbg)

apps_prof:
	(cd src; echo Making apps_prof ...;\
	$(MAKE) $(MFLAGS) apps_prof)



links: hlinks llinks mlinks dlinks blinks

hlinks:
	@/bin/rm -f $(INCDIR)/*
	-ln -s $(HDRFILES) $(INCDIR)

llinks:
	@/bin/rm -f $(LIBDIR)/*
	-ln -s $(LIBFILES) $(LIBDIR)

mlinks:
	@/bin/rm -f $(MANDIR)/*
	-ln -s $(MANFILES) $(MANDIR)

dlinks:
	@/bin/rm -f $(DOCDIR)/*
	-ln -s $(DOCFILES) $(MANDIR)

blinks:
	(cd $(SRCDIR); \
	$(MAKE) $(MFLAGS) link_apps)


depend:
	(cd src; echo Making depend ...;\
	$(MAKE) $(MFLAGS) depend)

strip_depend:
	(cd src; echo Making strip_depend ...;\
	$(MAKE) $(MFLAGS) strip_depend)

clean:
	(cd src; echo Cleaning up ...;\
	$(MAKE) $(MFLAGS) clean)

clean_obj:
	(cd src; echo Cleaning object files ...;\
	$(MAKE) $(MFLAGS) clean_obj)

touch:
	(cd src; echo Touching .hh and .cc files ...;\
	$(MAKE) $(MFLAGS) touch)


wc:
	wc -lc `find . \( -name "*.cc" -o -name "*.hh" \) -print`


backup:
	tar cvf .backup/src.backup.$(DATE).tar $(BCKFILES)
	gzip .backup/src.backup.$(DATE).tar


tar_tree:
	(cd ..; \
	 tar cvf SOFT.tar `find soft-curr -name ".*" -type d -prune -o -type l -prune -o \( -name "*.hh" -o -name "*.cc" -o -name "*.h" -o -name "*.c" -o -name "*.y" -o -name "*.l" -o -name "*.1" -o -name "GRASP*" -o -name "*.doc" -o -name "Make*" -o -name "INSTALL*" -o -name "README*" -o -name "ChangeLog*" -o -name "*.txt" -o -name ".*.txt" \) -print`; \
	gzip SOFT.tar;)


tar_src:
	(cd ..; \
	 tar cvf SOFT.tar `find soft-curr -name "\.*" -prune -o -type l -prune -o \( -name "*.hh" -o -name "*.cc" -o -name "*.h" -o -name "*.c" -o -name "*.y" -o -name "*.l" -o -name "*.1" -o -name "GRASP*" -o -name "*.doc" -o -name "Make*" -o -name "INSTALL*" -o -name "README*" -o -name "ChangeLog*" -print`; \
	gzip SOFT.tar;)


tar_incr:
	(cd ..; \
	tar cvf SOFT.tar `find soft-curr -name ".*" -type d -prune -o -type l -prune -o -mtime $(RANGE) \( -name "*.hh" -o -name "*.cc" -o -name "*.h" -o -name "*.c" -o -name "*.y" -o -name "*.l" -o -name "*.1" -o -name "GRASP*" -o -name "*.doc" -o -name "Make*" -o -name "INSTALL*" -o -name "README*" -o -name "ChangeLog*" -o -name "*.txt" -o -name ".*.txt" \) -print`; \
	gzip SOFT.tar;)

#------------------------------------------------------------------------------
