#------------------------------------------------------------------------------
# Makefile for all applications
#------------------------------------------------------------------------------

include $(GRASP_PATH)/Make_defs.include

APP_DIRS	= nsat

#-----------------------------------------------------------------------------

all: apps_o

install: all


apps_o:
	@for dir in $(APP_DIRS); do\
		(cd $$dir;\
		 echo Making $$dir ...;\
		 $(MAKE) $(MFLAGS) app)\
	done


apps_chk:
	@for dir in $(APP_DIRS); do\
		(cd $$dir;\
		 echo Making $$dir ...;\
		 $(MAKE) $(MFLAGS) app_chk)\
	done

apps_dbg:
	@for dir in $(APP_DIRS); do\
		(cd $$dir;\
		 echo Making $$dir ...;\
		 $(MAKE) $(MFLAGS) app_dbg)\
	done


apps_prof:
	@for dir in $(APP_DIRS); do\
		(cd $$dir;\
		 echo Making $$dir ...;\
		 $(MAKE) $(MFLAGS) app_prof)\
	done



link_apps:
	@for dir in $(APP_DIRS); do\
		(cd $$dir;\
		 echo Linking in $$dir ...;\
		 $(MAKE) $(MFLAGS) link_app)\
	done



depend:
	@for dir in $(APP_DIRS); do\
		(cd $$dir;\
		 echo Making $$dir dependencies...;\
		 $(MAKE) $(MFLAGS) depend)\
	done

strip_depend:
	@for dir in $(APP_DIRS); do\
		(cd $$dir;\
		 echo Removing $$dir dependencies...;\
		 $(MAKE) $(MFLAGS) strip_depend)\
	done

clean:
	@for dir in $(APP_DIRS); do\
		(cd $$dir;\
		 echo Cleaning $$dir...;\
		 $(MAKE) $(MFLAGS) clean)\
	done

clean_obj:
	@for dir in $(APP_DIRS); do\
		(cd $$dir;\
		 echo Cleaning objs in $$dir...;\
		 $(MAKE) $(MFLAGS) clean_obj)\
	done

touch:
	@for dir in $(APP_DIRS); do\
		(cd $$dir;\
		 echo Touching files in $$dir...;\
		 $(MAKE) $(MFLAGS) touch)\
	done

#------------------------------------------------------------------------------
