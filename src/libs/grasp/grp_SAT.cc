//-----------------------------------------------------------------------------
// File: grp_SAT.cc
//
// Purpose: Definition of the member functions of class GRASP_SAT.
//
// Remarks: --
//
// History: 03/10/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "time.hh"
#include "grp_SAT.hh"


//-----------------------------------------------------------------------------
// Non-inlined member functions of class GRASP_SAT.
// First, define functions for initiating the search and for resetting it.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function: setup()
//
// Purpose: Sets up engines and data structures used by the SAT algorithm.
//-----------------------------------------------------------------------------

void GRASP_SAT::setup (SAT_ClauseDatabase &clDB)
{
    DBG1(cout<<"Inside GRASP_SAT setup function"<<endl;);

    //-------------------------------------------------------------------------
    // Define the forward and backward reasoning engines.
    //-------------------------------------------------------------------------

    _FRE = new FRE (_mode);

    if (_mode[_DB_GROWTH_] == _POLYNOMIAL_DB_) {
	_BRE = (BRE*) new BRE_P (_mode);
    } else {
	_BRE = new BRE (_mode);
    }

    //-------------------------------------------------------------------------
    // Define the engines directly used by the search process.
    //-------------------------------------------------------------------------

    if (_mode[_PREPROC_LEVEL_] != NONE) {
	_preprocEng = new PreprocessEngine (_mode, *_FRE, *_BRE);
    }
    _deduceEng = new DeductionEngine (_mode, *_FRE, *_BRE);
    _diagnoseEng = new DiagnosisEngine (_mode, *_FRE, *_BRE);
    _decideEng = new DecisionEngine (_mode);
}


//-----------------------------------------------------------------------------
// Function: init()
//
// Purpose: Sets up data structures and engines for solving a pre-specified
//          instance of SAT.
//
// Side-effects: Prepares data structures and engines for a new SAT instance.
//-----------------------------------------------------------------------------

void GRASP_SAT::init(SAT_ClauseDatabase &clDB)
{
    DBG0(if (_mode[_DEBUG_])
	 {cout<<"CLAUSE DATABASE DUMP IN INIT"<<endl;clDB.dump(FALSE, cout);});
    clDB.init();

    if (_preprocEng) {
	_preprocEng->init (clDB);
    }
    _deduceEng->init (clDB);
    _diagnoseEng->init (clDB);
    _decideEng->init (clDB);
}


//-----------------------------------------------------------------------------
// Function: reset()
//
// Purpose: Resets the different engines and data structures from solving a
//          previous instance of SAT.
//
// Side-effects: All variables become unassigned. No clause is satisfied or
//               unsatisfied.
//
// Notes: I decided not to implement reset for FRE's.
//-----------------------------------------------------------------------------

void GRASP_SAT::reset (SAT_ClauseDatabase &clDB)
{
    _BRE->reset (clDB);

    if (_preprocEng) {
	_preprocEng->reset (clDB);
    }
    _deduceEng->reset (clDB);
    _diagnoseEng->reset (clDB);
    _decideEng->reset (clDB);
}


//-----------------------------------------------------------------------------
// Function: clear()
//
// Purpose: Clear necessary information to accept the definition of a SAT
//          instance.
//
// Side-effects: Clear stats and DLevel information.
//-----------------------------------------------------------------------------

void GRASP_SAT::clear (SAT_ClauseDatabase &clDB)
{
    clDB.clear();
    _BRE->clear (clDB);

    if (_preprocEng) {
	_preprocEng->clear (clDB);
    }
    _deduceEng->clear (clDB);
    _diagnoseEng->clear (clDB);
    _decideEng->clear (clDB);
}


//-----------------------------------------------------------------------------
// Fundamental function of GRASP-SAT: the solve() function that searches for
// a solution of a given SAT instance.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function: solve()
//
// Purpose: Conducts the search for finding a solution to an instance of SAT.
//
// Side-effects: Clause database can be completed with additional clauses.
//
// Notes: If the backtracking strategy is _DYNAMIC_B_ is set, then dynamic
//        organization of the search process is assumed and implemented.
//        In the end *no* reset to the state of the variables is done. This
//        should be done afterwards.
//-----------------------------------------------------------------------------

int GRASP_SAT::solve (SAT_ClauseDatabase &clDB)
{
    _time.presetTime();

    //-------------------------------------------------------------------------
    // Setup phase. Create DLevel = 0.
    //-------------------------------------------------------------------------

    DBG0(if (_mode[_DEBUG_])clDB.dump (FALSE, cout););
    clDB.push_decision();
    if (_preprocEng &&  (_preprocEng->preprocess (clDB) == CONFLICT) ||
	(_deduceEng->deduce (clDB) == CONFLICT)) {
	_BRE->reset (clDB, 0, 0);
	if (_mode[_VERBOSE_]) {
	    printTime ("Unsatisfiable instance",
		       _time.elapsedTime(), "SAT Elapsed");
	}
	return UNSATISFIABLE;
    }
    //-------------------------------------------------------------------------
    // Search phase.
    //-------------------------------------------------------------------------

    CHECK(if (clDB.exists_unsat_clause())Warn("UNSAT clause w/o conflict?");
	  if (_mode[_DEBUG_]) {clDB.check_consistency (TRUE);});
    DBG0(if (_mode[_DEBUG_]) clDB.dump (FALSE, cout););

    while (!clDB.solution_found() && _decideEng->decide (clDB) != SOLUTION) {
	DBG0(if (_mode[_DEBUG_])clDB.dump (TRUE, cout););

	while (_deduceEng->deduce (clDB) == CONFLICT) {
	    int backtrack_level = _diagnoseEng->diagnose (clDB);
	    if (!backtrack (clDB, backtrack_level)) {
		if (_mode[_VERBOSE_]) {
		    printTime ("Unsatisfiable instance",
			       _time.elapsedTime(), "SAT Elapsed");
		}
		return UNSATISFIABLE;
	    }
	    CHECK(else {if (_mode[_DEBUG_]) {clDB.check_consistency (TRUE);}});
	}
	if (resources_exceeded (clDB)) {
	    if (_mode[_VERBOSE_]) {
		printTime ("Aborted instance",
			   _time.elapsedTime(), "SAT Elapsed");
	    }
	    return ABORTED;
	}
	CHECK(if (clDB.exists_unsat_clause())
	      Warn("UNSAT clause w/o conflict?");
	      if (_mode[_DEBUG_]) {clDB.check_consistency (TRUE);});
	DBGn(clDB.dump (TRUE, cout););
    }
    CHECK(clDB.check_solution(););
    if (_mode[_VERBOSE_]) {
	printTime ("Satisfiable instance", _time.elapsedTime(), "SAT Elapsed");
    }
    return SATISFIABLE;
}


//-----------------------------------------------------------------------------
// Function: backtrack()
//
// Purpose: Causes search process to backtrack. Returns FALSE if problem is
//          found to be unsatisfiable.
//
// Side-effects: Search structures are updated accordingly.
//-----------------------------------------------------------------------------

int GRASP_SAT::backtrack (SAT_ClauseDatabase &clDB, int back_level)
{
    if (!back_level) {
	_BRE->reset (clDB, back_level, clDB.MLevel());    // *must* reset state
	return FALSE;
    }
    else if (_mode[_BACKTRACKING_STRATEGY_] != _DYNAMIC_B_) {
	_BRE->reset (clDB, back_level + 1, clDB.active_decisions());
    }
    _decideEng->backtrack (clDB, back_level);      // Recover decision schedule
    return TRUE;
}


//-----------------------------------------------------------------------------
// Function: resources_exceeded()
//
// Purpose: Evaluates whether the allowed computational resources have been
//          exceeded.
//-----------------------------------------------------------------------------

int GRASP_SAT::resources_exceeded (SAT_ClauseDatabase &clDB)
{
    int must_abort =
	(_time.readTime() > _mode[_TIME_LIMIT_]) ||
	    clDB.resources_exceeded (clDB) ||
		_BRE->resources_exceeded (clDB);
    return must_abort;
}


//-----------------------------------------------------------------------------
// Function: output_stats()
//
// Purpose: Outputs stats regarding running the SAT algorithm.
//
// Notes: Each engine and major data structure keeps their own stats.
//        This function just invokes the relevant function of each engine
//        or data structure.
//-----------------------------------------------------------------------------

void GRASP_SAT::output_stats (SAT_ClauseDatabase &clDB)
{
    cout << "\n\nOutput Statistics:\n" << endl;
    clDB.output_stats();

//    _FRE->output_stats (clDB);
    _BRE->output_stats (clDB);

    if (_preprocEng) {
	_preprocEng->output_stats (clDB);
    }
    _deduceEng->output_stats (clDB);
    _diagnoseEng->output_stats (clDB);
    _decideEng->output_stats (clDB);
    printItem();
}

/*****************************************************************************/
