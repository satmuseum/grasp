//-----------------------------------------------------------------------------
// File: macros.hh
//
// Purpose: Macro definitions for optimizations, consistency maintenance
//          and debugging purposes.
//
// Remarks: Expanded version of the debugging macros in LEAP.
//
// History: 5/12/94 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef _MACROS_DEF_
#define _MACROS_DEF_


//-----------------------------------------------------------------------------
// Macros for generation of optimized code, which can be prevented for
// debugging purposes.
//-----------------------------------------------------------------------------

#ifdef MEMG
#define DMEM(x) x
#else
#define DMEM(x)
#endif


//-----------------------------------------------------------------------------
// Expansion macros that can be used to print warning/error/debugging
// messages as well as validating invalid run-time conditions.
//-----------------------------------------------------------------------------

#ifdef STATS
#define STAT(x) x
#else
#define STAT(x)
#endif


#ifdef CHCK
#define CHECK(x) x
#else
#define CHECK(x)
#endif


#ifdef DEBUG_0
#define DBG0(x) x
#else
#define DBG0(x)
#endif


#ifdef DEBUG_1
#define DBG1(x) x
#else
#define DBG1(x)
#endif


#define DBGn(x)

#endif    // _MACROS_DEF_

/*****************************************************************************/

