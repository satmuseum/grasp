//-----------------------------------------------------------------------------
// File: time.hh
//
// Purpose: Routines for measuring the time spent in a given process.
//
// Remarks: --
//
// History: 04/21/92 - JPMS - first version.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef _TIMER_DEF_
#define _TIMER_DEF_

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>


extern "C" {
int getrusage(int, struct rusage*);
}


//-----------------------------------------------------------------------------
// Class: Timer
//
// Purpose: Time measurement class and member functions.
//-----------------------------------------------------------------------------

class Timer {
  public:

    //-------------------------------------------------------------------------
    // Constructor.
    //-------------------------------------------------------------------------

    Timer() { updateTime(); start = last = current; }

    //-------------------------------------------------------------------------
    // Presets time to current time.
    //-------------------------------------------------------------------------

    inline void presetTime() {
	updateTime();
	start = last = current;
    }

    //-------------------------------------------------------------------------
    // Returns elapsed CPU time since last call for this object.
    //-------------------------------------------------------------------------

    inline double elapsedTime() {
	updateTime();
	msec = (current - last) / 1000.0;
	last = current;
	return( msec );
    }

    //-------------------------------------------------------------------------
    // Returns total CPU time since construction of object.
    //-------------------------------------------------------------------------

    inline double totalTime() {
	updateTime();
	msec = (current - start) / 1000.0;
	last = current;
	return( msec );
    }

    //-------------------------------------------------------------------------
    // Reads total time, but does not change the information used to measure
    // the elapsed time (last).
    //-------------------------------------------------------------------------

    inline double readTime() {
	updateTime();
	msec = (current - start) / 1000.0;
	return( msec );
    }
  protected:

    //-------------------------------------------------------------------------
    // Auxiliar variables.
    //-------------------------------------------------------------------------

    double msec;

    //-------------------------------------------------------------------------
    // Auxiliary internal variables.
    //-------------------------------------------------------------------------

    long          start;    // Start time, when the constructor is invoked.
    long          last;     // Last time the print time method was made.
    long          current;  // Present time.
    struct rusage rusage;   // Resource variable.
    double        _time;    // Time in seconds.

    //-------------------------------------------------------------------------
    // Updates current time of object.
    //-------------------------------------------------------------------------

    inline void updateTime() {
	(void) getrusage(RUSAGE_SELF, &rusage);

	// User time.

	current = (long) rusage.ru_utime.tv_sec*1000 +
	                 rusage.ru_utime.tv_usec/1000;

	// System time => total time.
	// NOTE: Most packages and tools do not measure the system time.

	current += (long) rusage.ru_stime.tv_sec*1000+
	                  rusage.ru_stime.tv_usec/1000;
    }
};

#endif

/*****************************************************************************/

