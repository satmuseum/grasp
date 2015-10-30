#------------------------------------------------------------------------------
# utils - utilities for the different software packages.
#------------------------------------------------------------------------------

include $(GRASP_PATH)/Make_defs.include

TG	= utils
TGDIR	= $(SRCDIR)/libs/$(TG)

TGSRC	= memory.cc sort.cc cmdArg.cc cmdLine.cc utils.cc

TGOBJ	= $(TGSRC:.cc=.o)

THDR	= array.hh cmdArg.hh cmdLine.hh defs.hh list.hh macros.hh \
	  memory.hh sort.hh sortInt.hh symbTable.hh time.hh traverse.hh

MISC	= $(TG).doc Makefile

include $(SRCDIR)/Make_templates.include
include $(SRCDIR)/libs/Make_lib.include

#------------------------------------------------------------------------------

#--DO NOT CHANGE ANYTHING AFTER THIS LINE
memory.o: memory.cc /usr/include/stdlib.h /usr/include/features.h \
 /usr/include/sys/cdefs.h \
 /usr/lib/gcc-lib/i586-unknown-linux/2.7.2.1/include/stddef.h \
 /usr/include/errno.h /usr/include/linux/errno.h \
 /usr/include/asm/errno.h /usr/include/alloca.h \
 /usr/lib/g++-include/iostream.h /usr/lib/g++-include/streambuf.h \
 /usr/include/libio.h /usr/include/_G_config.h defs.hh macros.hh \
 memory.hh
sort.o: sort.cc /usr/include/math.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/huge_val.h \
 /usr/include/endian.h /usr/include/bytesex.h \
 /usr/include/linux/version.h /usr/include/asm/byteorder.h \
 /usr/include/nan.h \
 /usr/lib/gcc-lib/i586-unknown-linux/2.7.2.1/include/float.h \
 /usr/include/values.h defs.hh macros.hh array.hh list.hh \
 /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/stdlib.h \
 /usr/lib/gcc-lib/i586-unknown-linux/2.7.2.1/include/stddef.h \
 /usr/include/errno.h /usr/include/linux/errno.h \
 /usr/include/asm/errno.h /usr/include/alloca.h \
 /usr/lib/g++-include/iostream.h /usr/lib/g++-include/streambuf.h \
 memory.hh sort.hh
cmdArg.o: cmdArg.cc /usr/include/ctype.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/endian.h /usr/include/bytesex.h \
 /usr/include/linux/version.h /usr/include/asm/byteorder.h \
 /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/string.h \
 /usr/lib/gcc-lib/i586-unknown-linux/2.7.2.1/include/stddef.h \
 /usr/lib/g++-include/iostream.h /usr/lib/g++-include/streambuf.h \
 defs.hh macros.hh cmdArg.hh
cmdLine.o: cmdLine.cc /usr/lib/g++-include/iostream.h \
 /usr/lib/g++-include/streambuf.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/string.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/lib/gcc-lib/i586-unknown-linux/2.7.2.1/include/stddef.h defs.hh \
 macros.hh cmdLine.hh array.hh cmdArg.hh
utils.o: utils.cc /usr/include/stdlib.h /usr/include/features.h \
 /usr/include/sys/cdefs.h \
 /usr/lib/gcc-lib/i586-unknown-linux/2.7.2.1/include/stddef.h \
 /usr/include/errno.h /usr/include/linux/errno.h \
 /usr/include/asm/errno.h /usr/include/alloca.h /usr/include/stdio.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/lib/g++-include/iostream.h /usr/lib/g++-include/streambuf.h
