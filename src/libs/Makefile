#------------------------------------------------------------------------------
# Makefile for all libraries.
#------------------------------------------------------------------------------

include $(GRASP_PATH)/Make_defs.include

LIB_DIRS	= utils grasp parse/cnf_parse

#-----------------------------------------------------------------------------

all: libs_o

install: all


libs_o:
	for dir in $(LIB_DIRS); do\
		(cd $$dir;\
		 echo Making $$dir ...;\
		 $(MAKE) $(MFLAGS) lib)\
	done


libs_chk:
	for dir in $(LIB_DIRS); do\
		(cd $$dir;\
		 echo Making $$dir ...;\
		 $(MAKE) $(MFLAGS) lib_chk)\
	done

libs_dbg:
	for dir in $(LIB_DIRS); do\
		(cd $$dir;\
		 echo Making $$dir ...;\
		 $(MAKE) $(MFLAGS) lib_dbg)\
	done


libs_prof:
	for dir in $(LIB_DIRS); do\
		(cd $$dir;\
		 echo Making $$dir ...;\
		 $(MAKE) $(MFLAGS) lib_prof)\
	done


depend:
	for dir in $(LIB_DIRS); do\
		(cd $$dir;\
		 echo Making $$dir dependencies...;\
		 $(MAKE) $(MFLAGS) depend)\
	done

strip_depend:
	for dir in $(LIB_DIRS); do\
		(cd $$dir;\
		 echo Removing $$dir dependencies...;\
		 $(MAKE) $(MFLAGS) strip_depend)\
	done

clean:
	for dir in $(LIB_DIRS); do\
		(cd $$dir;\
		 echo Cleaning $$dir...;\
		 $(MAKE) $(MFLAGS) clean)\
	done

clean_obj:
	for dir in $(LIB_DIRS); do\
		(cd $$dir;\
		 echo Cleaning objs in $$dir...;\
		 $(MAKE) $(MFLAGS) clean_obj)\
	done

touch:
	for dir in $(LIB_DIRS); do\
		(cd $$dir;\
		 echo Touching files in $$dir...;\
		 $(MAKE) $(MFLAGS) touch)\
	done

#------------------------------------------------------------------------------
