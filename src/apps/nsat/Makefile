#------------------------------------------------------------------------------
# nsat - GRASP SAT front-end for the CNF format (proposed at DIMACS).
#------------------------------------------------------------------------------

include $(GRASP_PATH)/Make_defs.include

TG	= nsat
TGDIR	= $(SRCDIR)/apps/$(TG)

TGSRC	= nsat.cc

TGOBJ	= $(TGSRC:.cc=.o)

THDR	= 


LIB_DIRS	= grasp parse/cnf_parse utils


TGLIBS	= $(LIBDIR)/libgrasp.a $(LIBDIR)/libcnf_parse.a \
	  $(LIBDIR)/libutils.a
TGLIBDEF = -lgrasp -lcnf_parse -lutils


TGLIBSG	= $(LIBDIR)/libgrasp_g.a $(LIBDIR)/libcnf_parse_g.a \
	  $(LIBDIR)/libutils_g.a
TGLIBGDEF = -lgrasp_g -lcnf_parse_g -lutils_g


TGLIBSFG= $(LIBDIR)/libgrasp_fg.a $(LIBDIR)/libcnf_parse_fg.a \
	  $(LIBDIR)/libutils_fg.a
TGLIBFGDEF = -lgrasp_fg -lcnf_parse_fg -lutils_fg


TGLIBSPG= $(LIBDIR)/libgrasp_pg.a $(LIBDIR)/libcnf_parse_pg.a \
	  $(LIBDIR)/libutils_pg.a
TGLIBPGDEF = -lgrasp_pg -lcnf_parse_pg -lutils_pg


MISC	= $(TG).doc Makefile

include $(SRCDIR)/Make_templates.include
include $(SRCDIR)/apps/Make_app.include

#------------------------------------------------------------------------------

#--DO NOT CHANGE ANYTHING AFTER THIS LINE
nsat.o: nsat.cc /usr/include/stdio.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/stdlib.h \
 /usr/lib/gcc-lib/i586-unknown-linux/2.7.2.1/include/stddef.h \
 /usr/include/errno.h /usr/include/linux/errno.h \
 /usr/include/asm/errno.h /usr/include/alloca.h \
 /usr/lib/g++-include/iostream.h /usr/lib/g++-include/streambuf.h \
 /home/jpms/research/.tmp/soft/include/time.hh \
 /usr/include/sys/types.h /usr/include/linux/types.h \
 /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h \
 /usr/include/asm/types.h /usr/include/sys/time.h \
 /usr/include/linux/time.h /usr/include/time.h \
 /usr/include/sys/resource.h /usr/include/limits.h \
 /usr/include/posix1_lim.h /usr/include/linux/limits.h \
 /usr/include/posix2_lim.h \
 /usr/lib/gcc-lib/i586-unknown-linux/2.7.2.1/include/limits.h \
 /usr/include/linux/resource.h /usr/include/asm/resource.h \
 /home/jpms/research/.tmp/soft/include/memory.hh \
 /home/jpms/research/.tmp/soft/include/defs.hh \
 /home/jpms/research/.tmp/soft/include/macros.hh \
 /home/jpms/research/.tmp/soft/include/cnf_Parser.hh \
 /home/jpms/research/.tmp/soft/include/array.hh \
 /home/jpms/research/.tmp/soft/include/grp_SAT.hh \
 /home/jpms/research/.tmp/soft/include/grp_Mode.hh \
 /home/jpms/research/.tmp/soft/include/grp_CDB.hh \
 /home/jpms/research/.tmp/soft/include/list.hh \
 /home/jpms/research/.tmp/soft/include/grp_CNF.hh \
 /home/jpms/research/.tmp/soft/include/grp_BRE.hh \
 /home/jpms/research/.tmp/soft/include/grp_BRE_P.hh \
 /home/jpms/research/.tmp/soft/include/grp_FRE.hh \
 /home/jpms/research/.tmp/soft/include/grp_Preproc.hh \
 /home/jpms/research/.tmp/soft/include/grp_Deduce.hh \
 /home/jpms/research/.tmp/soft/include/grp_Diagnose.hh \
 /home/jpms/research/.tmp/soft/include/grp_Decide.hh \
 /home/jpms/research/.tmp/soft/include/grp_Cmds.hh \
 /home/jpms/research/.tmp/soft/include/cmdArg.hh \
 /home/jpms/research/.tmp/soft/include/cmdLine.hh
