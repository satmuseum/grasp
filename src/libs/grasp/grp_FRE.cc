//-----------------------------------------------------------------------------
// File: grp_FRE.cc
//
// Purpose: Implementation of the forward reasoning engine, which just
//          implements Boolean Constraint Propagation.
//
// Remarks: This class *needs* to know in detail the operation of
//          consistency in the clause database.
//
// History: 03/15/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "grp_FRE.hh"


//-----------------------------------------------------------------------------
// Function: BCP()
//
// Purpose: Applies BCP to a clause database.
//
// Side-effects: State of clause database changes accordingly. It either
//               becomes inconsistent, due to a conflict, or remains
//               unresolved without implied assignments.s
//
// Notes: The use of variable lst_cl is a hack... It is used to avoiding
//        visiting unit clauses already visited. This is required
//        because of the interface provided by the clause database.
//        A more elaborated interface would become somewhat cumbersome.
//-----------------------------------------------------------------------------

int FRE::BCP (SAT_ClauseDatabase &clDB)
{
    DBG0(cout<<"Starting BCP"<<endl<<endl;);
    register SAT_VariablePtr var;
    register SAT_ClausePtr cl;
    register int consistent = TRUE;

    while ((consistent || _mode[_MULTIPLE_CONFLICTS_]) &&
	   (clDB.exists_implied_variable() ||
	    clDB.exists_unit_clause())) {

	while (var = clDB.first_implied_variable()) {
	    DBG0(cout << "Assigning ";var->dump(cout);cout << endl;);
	    clDB.set_assigned_variable (var);
	    consistent = set_clauses_state (clDB, var) && consistent;
	}
	DBG0(cout << "Done with implications"<<endl;
	     if (_mode[_DEBUG_])clDB.dump (TRUE, cout););
	if (!consistent && !_mode[_MULTIPLE_CONFLICTS_]) break;

	cl = NULL;
	while (cl = clDB.next_unit_clause (cl)) {
	    DBG0(cout << "Satisfying ";cl->dump(cout);cout << endl;);
	    consistent = imply_variable_assignment (clDB, cl) && consistent;
	}
	DBG0(cout << "Done with unit clauses"<<endl;
	     if (_mode[_DEBUG_])clDB.dump (TRUE, cout););
    }
    CHECK(if (consistent &&
	      (clDB.exists_unit_clause() || clDB.exists_implied_variable()))
	  Warn("Invalid implication sequence"););
    DBG0(cout<<"Done with BCP"<<endl<<endl;);

    consistent = (consistent) ? NO_CONFLICT : CONFLICT;
    return consistent;
}


//-----------------------------------------------------------------------------
// Function: set_clauses_state
//
// Purpose: Given assignment on a variable, this function identifies changes
//          to the state of clauses containing literals on that variable.
//          It identifies unsatisfied clauses.
//
// Side-effects: Some clauses can become unit and be added to the list of
//               unit clauses. In the presence of a conflict, the clause is
//               declared to be unsatisfied.
//-----------------------------------------------------------------------------

int FRE::set_clauses_state (SAT_ClauseDatabase &clDB, SAT_VariablePtr var)
{
    register int c_state, n_state;
    register SAT_ClausePtr cl;

    register int consistent = TRUE;
    for_each(plit,var->literals(),LiteralPtr) {
	cl = (SAT_ClausePtr) plit->data()->clause();

	c_state = cl->state();
	n_state = cl->set_state (plit->data());
	clDB.update_state (cl, c_state, n_state);
	consistent = consistent && (n_state != UNSATISFIED);
    }
    return consistent;
}


//-----------------------------------------------------------------------------
// Function: imply_variable_assignment
//
// Purpose: For a given unit clause, identifies a variable assignment required
//          to satisfy that clause. It may identify *no* assignments if the
//          clause already is satisfied or will be declared unsatisfied.
//
// Side-effects: One variable assignment can be identified. Conflict
//               identification has been moved to the function for state
//               setting.
//-----------------------------------------------------------------------------

int FRE::imply_variable_assignment (SAT_ClauseDatabase &clDB, SAT_ClausePtr cl)
{
    register int value = NONE;
    register int maxLevel = 0;               // Assignment at least at DLevel 0
    register SAT_VariablePtr tvar = NULL;
    for_each(plit,cl->literals(),LiteralPtr) {
	register LiteralPtr lit = plit->data();
	register SAT_VariablePtr var = (SAT_VariablePtr) lit->variable();

	if (var->value() == UNKNOWN) {
	    tvar = var;
	    value = 1 - lit->sign();
	}
	else if (var->DLevel() > maxLevel) {
	    maxLevel = var->DLevel();
	}
    }
    if (value != NONE) {                                // Can imply assignment
	clDB.set_implied_variable (tvar, value, maxLevel, cl);
	DBG0(cout << "Implying ";tvar->dump(cout);cout << endl;);
    }
    return TRUE;          // Conflicts are detected when updating clauses state
}

/*****************************************************************************/
