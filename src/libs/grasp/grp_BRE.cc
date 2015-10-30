//-----------------------------------------------------------------------------
// File: grp_BRE.cc
//
// Purpose: Implementation of the backward reasoning engine. It can
//          be used for conflict analysis and for state resetting.
//
// Remarks: A set of color tags is used to mark variables. Tag BROWN means
//          that the variable is going to be examined, since it has been
//          involved in a conflict. Tag TAN means a dependency represented
//          as a literal in a conflicting clause. Tag GRAY means a variable
//          that must become unassigned. In the end all tags are cleared.
//
// History: 03/15/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "grp_BRE.hh"


//-----------------------------------------------------------------------------
// Function: reset()
//
// Purpose: Functions for state resetting. Second fuction resets search
//          between two decision levels: a higher and a lower decision level.
//
// Side-effects: Search gets reset.
//-----------------------------------------------------------------------------

void BRE::reset (SAT_ClauseDatabase &clDB) {  reset (clDB, 0, clDB.MLevel()); }

void BRE::reset (SAT_ClauseDatabase &clDB, int l_level, int h_level)
{
    for (register int k = h_level; k >= l_level; k--)  {
	clDB.DLevel() = k;                      // Update DLevel internal to DB
	unset_state (clDB, k);
	clDB.pop_decision (k);
    }
}


//-----------------------------------------------------------------------------
// Function: clear()
//
// Purpose: Clear structure for subsequent search.
//-----------------------------------------------------------------------------

void BRE::clear (SAT_ClauseDatabase &clDB)
{
    _CLevel = NONE;
    _conf_clause = NULL;
    _tagged_vars = NONE;
    _skip_UIP = FALSE;

    _tot_conf_number = 0;
    _large_conf_diff = 0;
    _tot_back_number = 0;
    _tot_nonback_number = 0;
    _large_skip = 0;
    _tot_conf_cl_number = 0;
    _large_cl_size = 0;
    _small_cl_size = 0;
    _tot_uip_number = 0;
    _max_uip_number = 0;
}


//-----------------------------------------------------------------------------
// Functions for backward reasoning, i.e. for identifying the causes of
// conflicts or simply for backtracking.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function: unset_state()
//
// Purpose: Recovers the search from a given decision assignment without
//          conflict analysis.
//
// Side-effects: The state of variables and clauses is unset.
//-----------------------------------------------------------------------------

void BRE::unset_state (SAT_ClauseDatabase &clDB, int level)
{
    DBG1(Info("INSIDE unsat_state"););
    unset_implied_variables (clDB);

    register SAT_VariablePtr var;
    while (var = clDB.last_assigned_variable (level)) {
	unset_clauses_state (clDB, var);
	var->unset_state();
    }
    unset_unassigned_variables (clDB);
}


//-----------------------------------------------------------------------------
// Function: BCA()
//
// Purpose: Identifies the causes of conflicts. This function also clears
//          the previous sequence of implied assignments. Also, only one
//          unsatisfied clause is (explicitly) considered.
//
// Side-effects: New clauses can be added to the clause database. Some
//               variables become unassigned and clauses unresolved.
//
// Notes: BROWN flag indicates variable whose assignment is involved in a
//        conflict.
//-----------------------------------------------------------------------------

int BRE::BCA (SAT_ClauseDatabase &clDB)
{
    DBG0(cout<<"Starting BCA"<<endl<<endl;);
    _tot_conf_number++;

    int minDLevel = clDB.MLevel();            // Set to highest DLevel possible
    int num_depends = 0;
    SAT_ClausePtr cl;

    while (cl = clDB.next_unsat_clause (NULL)) {
	_CLevel = NONE;            // Current working DLevel must be recomputed

	for_each(plit,cl->literals(),LiteralPtr) {
	    LiteralPtr lit = plit->data();
	    SAT_VariablePtr var = (SAT_VariablePtr) lit->variable();
	    if (var->DLevel() > _CLevel) {
		_CLevel = var->DLevel();
		num_depends = 1;
	    }
	    else if (var->DLevel() == _CLevel) { num_depends++; }

 	    DBG0(cout<"Clause var";var->dump(cout);cout<<endl;);
	}
	if (minDLevel > _CLevel) { minDLevel = _CLevel; }
	if ((clDB.DLevel() = _CLevel) == 0) { break; }  // Update search DLevel
	if (num_depends == 1) {
	    unset_state (clDB, _CLevel);
	}
	else {
	    DBG0(cout<<"Conflicting DLevel: "<<_CLevel<<endl;
		 cout<<"  Unsat ";cl->dump(cout);cout<<endl<<endl;);

	    _conf_clause = clDB.add_clause();    // Start building confl clause
	    _skip_UIP = FALSE;
	    _tagged_vars = 0;

	    // Tag variables of conflict and unset set of clauses and vars

	    tag_clause_vars (clDB, NULL, cl, _CLevel);
	    unset_state_w_conflict (clDB, _CLevel);
	}
	_tagged_vars = NONE;
	_conf_clause = NULL;
	clDB.pop_decision (_CLevel);              // Free currently used DLevel
    }
    if (_mode[_VERBOSE_]) { output_conf_stats (clDB, minDLevel); }

    DBG0(cout<<"Done with BCA"<<endl<<endl;);
    return minDLevel;                     // Return lowest DLevel with conflict
}


//-----------------------------------------------------------------------------
// Function: unset_state_w_conflict()
//
// Purpose: Unsets the implication sequence at the current WORKING decision
//          level in the presence of a conflict. Hence, a conflicting clause
//          is created while unsetting the state.
//
// Side-effects: A clause is created and the state of variables and other
//               clauses is unset.
//-----------------------------------------------------------------------------

void BRE::unset_state_w_conflict (SAT_ClauseDatabase &clDB, int level)
{
    DBG0(cout<<"Unsetting @ DLevel: "<<level<<endl<<endl;);
    unset_implied_variables (clDB);

    register int uip_cnt = 0;
    register SAT_VariablePtr var;
    while (var = clDB.last_assigned_variable (level)) {
	DBG1(var->dump(cout);cout<<endl;);

	if (var->test_tag (BROWN)) {       // Variable associated with conflict
	    if (_tagged_vars == 1) {
		if (!_skip_UIP && (_mode[_IDENTIFY_UIPS_] ||
				   !clDB.exists_assigned_variable(level))) {
		    uip_cnt++;
		    add_clause_literal (clDB, var, var->value());
		    untag_conf_clause_vars();

		    _conf_clause = NULL;
		    if (clDB.exists_assigned_variable(level)) {
			_conf_clause = clDB.add_clause();
			_skip_UIP = TRUE;
		    }
		    _tot_uip_number++;
		}
	    }
	    var->unset_tag (BROWN);
	    _tagged_vars--;
	    if (var->antecedent()) {
		tag_clause_vars (clDB, var, var->antecedent(), level);
	    }
	    if (_skip_UIP) {
		if (_tagged_vars <= 1) {
		    del_conf_literals (clDB);
		} else {
		    add_clause_literal (clDB, var, 1 - var->value());
		    _skip_UIP = FALSE;
		}
	    }
	    DBGn(cout<<"TAGGED VARS: "<<_tagged_vars<<endl;);
	}
	unset_clauses_state (clDB, var);
	var->unset_state();
    }
    if (uip_cnt > _max_uip_number) {
	_max_uip_number = uip_cnt;
    }
    del_conf_clause (clDB);                 // Last conf clause must be deleted
    unset_unassigned_variables (clDB);
    clDB.del_tagged_clauses (GRAY);                  // Delete all GRAY clauses
}


//-----------------------------------------------------------------------------
// Function: unset_clauses_state()
//
// Purpose: Unsets changes to the clauses associated with a given variable.
//
// Side-effects: Some clauses can become unresolved and others become unit.
//               These unit clauses can imply assignments at other DLevels.
//-----------------------------------------------------------------------------

void BRE::unset_clauses_state (SAT_ClauseDatabase &clDB, SAT_VariablePtr var)
{
    for_each(plit,var->literals(),LiteralPtr) {
	register SAT_ClausePtr cl = (SAT_ClausePtr) plit->data()->clause();

	SAT_VariablePtr ivar = cl->implied_var();
	if (cl->state() == SATISFIED && ivar && ivar != var &&
	    !ivar->test_tag (GRAY)) {
	    clDB.set_unassigned_variable (ivar);
	    ivar->set_tag (GRAY);            // Variable will become unassigned
	}
	register int c_state = cl->state();
	register int n_state = cl->unset_state (plit->data());
	clDB.update_state (cl, c_state, n_state);
    }
}


//-----------------------------------------------------------------------------
// Function: resources_exceeded()
//
// Purpose: Tests whether available computational resources have been
//          exceeded.
//-----------------------------------------------------------------------------

int BRE::resources_exceeded (SAT_ClauseDatabase &clDB)
{
    return (_tot_conf_number >= _mode[_CONFLICT_LIMIT_] ||
	    _tot_back_number >= _mode[_BACKTRACK_LIMIT_]);
}


//-----------------------------------------------------------------------------
// Function: output_stats()
//
// Purpose: Output stats associated with running GRASP on a clause database.
//-----------------------------------------------------------------------------

void BRE::output_stats (SAT_ClauseDatabase &clDB)
{
    printItem ("Number of conflicts", _tot_conf_number);
    printItem ("Number of conflicting clauses", _tot_conf_cl_number);
    printItem ("Largest level difference in conflict", _large_conf_diff);
    if (_mode[_BACKTRACKING_STRATEGY_] != _DYNAMIC_B_) {
	printItem ("Number of backtracks", _tot_back_number);
	if (_mode[_BACKTRACKING_STRATEGY_] == _NON_CHRONOLOGICAL_B_) {
	    printItem ("Number of non-chronological backtracks",
		       _tot_nonback_number);
	    printItem ("Largest skip in decision tree", _large_skip);
	}
    }
    printItem();
    printItem ("Largest identified conflicting clause", _large_cl_size);
    printItem ("Smallest identified conflicting clause", _small_cl_size);
    printItem();
    printItem ("Number of UIPs", _tot_uip_number);
    printItem ("Maximum number of UIPs", _max_uip_number);
    printItem();
}

/*****************************************************************************/

