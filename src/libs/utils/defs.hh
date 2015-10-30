//-----------------------------------------------------------------------------
// File: defs.hh
//
// Purpose: Definition of the basic constants and macros common to most code
//          being developed. 
//
// Remarks: --
//
// History: 6/16/95 - JPMS - created, using definitions commont other programs.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef _COMMON_DEFS
#define _COMMON_DEFS

#include "macros.hh"    // Macros used for consistency checks and debugging


//-----------------------------------------------------------------------------
// Useful typedef definitions.
//-----------------------------------------------------------------------------

typedef unsigned int Boolean;


//-----------------------------------------------------------------------------
//
// Declaration of general-purpose utility functions. See utils.C for
// definitions.
//
//-----------------------------------------------------------------------------

extern void setTrailString(char*), Abort(char*), Warn(char*), Info(char*);
extern void printItem(), printItem(char*, int), printItem(char*, double);
extern void printTime(char *msg, double time, char *tmsg);


//-----------------------------------------------------------------------------
//
// Declaration of general-purpose constants, macros and enumerated types.
//
//-----------------------------------------------------------------------------

#ifndef _MODDDEF
#define _MODDDEF
#define ODD(x)   ((x % 2) != 0)
#define EVEN(x)  ((x % 2) == 0)
#endif

#ifndef _MMAXDEF
#define _MMAXDEF
#define MAX(a,b) (((a)<(b))?(b):(a))
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif


#ifndef NULL
#define NULL        ((void*)0)
#endif

#ifndef FALSE
#define FALSE       0
#define TRUE        1
#endif

#ifndef NONE
#define NONE        (-1)
#endif

#ifndef ANY
#define ANY         (-2)
#endif

#ifndef FORWARD
#define FORWARD     1
#define BACKWARD    2
#endif

#ifndef ON
#define ON          1
#define OFF         2
#endif

#ifndef INFINITY
#define INFINITY    1000000000
#endif

#ifndef ABORT
#define ABORT       (-2)
#endif


enum Tags { WHITE = 0x0,
	    BLACK = 0x1, GRAY = 0x2, TAN = 0x4, BROWN = 0x8,
	    RED = 0x10, ROSE = 0x20, SALMON = 0x40, ORANGE = 0x80,
	    OLIVE = 0x100, KHAKI = 0x200, GREEN = 0x400, MAZE = 0x800,
	    CYAN = 0x1000, BLUE = 0x2000, VIOLET = 0x4000,  PURPLE = 0x8000
	    };                  // By definition, WHITE means no associated tag

#endif  // COMMON_DEFS

/*****************************************************************************/

