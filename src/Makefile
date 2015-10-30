#------------------------------------------------------------------------------
# Makefile for tools and libraries.
#
# Date:		03/20/96
# Author:	jpms.
#------------------------------------------------------------------------------

include $(GRASP_PATH)/Make_defs.include

#-----------------------------------------------------------------------------

all: libs_o apps_o

install: all


libs_o:
	(cd libs; echo Making libs ...;\
	$(MAKE) $(MFLAGS) libs_o)

libs_chk:
	(cd libs; echo Making libs_chk ...;\
	$(MAKE) $(MFLAGS) libs_chk)

libs_dbg:
	(cd libs; echo Making libs_dbg ...;\
	$(MAKE) $(MFLAGS) libs_dbg)

libs_prof:
	(cd libs; echo Making libs_prof ...;\
	$(MAKE) $(MFLAGS) libs_prof)


apps_o:
	(cd apps; echo Making apps ...;\
	$(MAKE) $(MFLAGS) apps_o)

apps_chk:
	(cd apps; echo Making apps_chk ...;\
	$(MAKE) $(MFLAGS) apps_chk)

apps_dbg:
	(cd apps; echo Making apps_dbg ...;\
	$(MAKE) $(MFLAGS) apps_dbg)

apps_prof:
	(cd apps; echo Making apps_prof ...;\
	$(MAKE) $(MFLAGS) apps_prof)


link_apps:
	(cd apps; echo Making links for apps ...;\
	$(MAKE) $(MFLAGS) link_apps)


depend:
	(cd apps; echo Making depend on apps ...;\
	$(MAKE) $(MFLAGS) depend)
	(cd libs; echo Making depend on libs ...;\
	$(MAKE) $(MFLAGS) depend)

strip_depend:
	(cd apps; echo Making strip_depend on apps ...;\
	$(MAKE) $(MFLAGS) strip_depend)
	(cd libs; echo Making strip_depend on libs ...;\
	$(MAKE) $(MFLAGS) strip_depend)

clean:
	(cd apps; echo Cleaning on apps ...;\
	$(MAKE) $(MFLAGS) clean)
	(cd libs; echo Cleaning on libs ...;\
	$(MAKE) $(MFLAGS) clean)

clean_obj:
	(cd apps; echo Cleaning object files on apps ...;\
	$(MAKE) $(MFLAGS) clean_obj)
	(cd libs; echo Cleaning objet files on libs ...;\
	$(MAKE) $(MFLAGS) clean_obj)

touch:
	(cd apps; echo Touching apps ...;\
	$(MAKE) $(MFLAGS) touch)
	(cd libs; echo Touching libs ...;\
	$(MAKE) $(MFLAGS) touch)

#------------------------------------------------------------------------------
