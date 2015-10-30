//-----------------------------------------------------------------------------
// File: grp_Mode.cc
//
// Purpose: Member functions of class SAT_Mode. Used for identifying default
//          mode options.
//
// Remarks: --
//
// History: 03/15/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "grp_Mode.hh"


//-----------------------------------------------------------------------------
// Constructor/destructor.
//-----------------------------------------------------------------------------
    
SAT_Mode::SAT_Mode() : _mode (0) { }

SAT_Mode::~SAT_Mode() { _mode.resize (0); }


//-----------------------------------------------------------------------------
// Function: setup()
//
// Purpose: 
//
// Side-effects: Internal structures are resized.
//
// Notes: This is necessary since it's the only way to overload the
//        function call. At construction time it was not possible.
//-----------------------------------------------------------------------------

void SAT_Mode::setup()
{
    alloc_mode();
    init_mode();
}


//-----------------------------------------------------------------------------
// Function: alloc_mode()
//
// Purpose: Allocates space for the _mode array.
//-----------------------------------------------------------------------------

void SAT_Mode::alloc_mode()
{
    DBG1(cout<<"Allocating space for SAT options"<<endl;);
    _mode.resize (SAT_OPTION_NUMBER);
}


//-----------------------------------------------------------------------------
// Function: init_mode()
//
// Purpose: Specifies default values.
//-----------------------------------------------------------------------------

void SAT_Mode::init_mode()                             // Default configuration
{
    DBG1(cout<<"Initializing for SAT with mode array of size ";
	 cout<<_mode.size()<<endl;);
    _mode[_DEBUG_] = FALSE;
    _mode[_VERBOSE_] = FALSE;
    _mode[_BACKTRACK_LIMIT_] = 100000;
    _mode[_CONFLICT_LIMIT_] = 200000;
    _mode[_TIME_LIMIT_] = 1000;
    _mode[_SPACE_LIMIT_] = 10;

    _mode[_PREPROC_LEVEL_] = NONE;                          // NO Preprocessing
    _mode[_PREPROC_MODE_] = _WO_RELAXATION_;                   // NO Relaxation
    _mode[_DEDUCTION_LEVEL_] = 0;                 // BCP-based deduction engine
    _mode[_DEDUCTION_MODE_] = NONE;                             // Not used yet
    _mode[_DIAGNOSIS_LEVEL_] = 0;                 // Plain BCA diagnosis engine
    _mode[_DIAGNOSIS_MODE_] = NONE;                             // Not used yet
    _mode[_DECISION_LEVEL_] = _FIXED_;           // Fixed order decision making
    _mode[_DECISION_MODE_] = _STATIC_ORD_;              // Static ordering used

    _mode[_MULTIPLE_CONFLICTS_] = FALSE;               // NO multiple conflicts
    _mode[_MULTI_CONF_HANDLING_] = _SEPARATE_;   // Handle separately each conf
    _mode[_IDENTIFY_UIPS_] = TRUE;                             // Identify UIPs

    _mode[_BACKTRACKING_STRATEGY_] = _DYNAMIC_B_;       // Dynamic backtracking
    _mode[_DB_GROWTH_] = _EXPONENTIAL_DB_;                // Exponential growth
    _mode[_CONF_CLAUSE_SIZE_] = INFINITY;                 // Accept all clauses
    _mode[_TRIM_SOLUTIONS_] = FALSE;            // NO trim solutions by default
    _mode[_CONF_CLAUSE_REPACKING_] = FALSE;    // NO repack conflicting clauses
}

/*****************************************************************************/
