//-----------------------------------------------------------------------------
// File: grp_BRE_P.cc
//
// Purpose: Backward reasoning for a polynomially bounded clause database.
//
// Remarks: Some conflicting clauses are eventually deleted. This clauses
//          are taggeed BLACK to denote large size clauses. A GRAY tag
//          indicates that the clause can be deleted.
//
// History: 03/14/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "grp_BRE_P.hh"


//-----------------------------------------------------------------------------
// Functions for backward reasoning, i.e. for identifying the causes of
// conflicts or simply for backtracking.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function: clear()
//
// Purpose: Clear internal structures for subsequent SAT instance.
//-----------------------------------------------------------------------------

void BRE_P::clear (SAT_ClauseDatabase &clDB)
{
    BRE::clear (clDB);
    _tot_discard_cl_number = 0;
}


//-----------------------------------------------------------------------------
// Function: unset_state()
//
// Purpose: Recovers the search from a given decision assignment without
//          conflict analysis.
//
// Side-effects: The state of variables and clauses is unset.
//-----------------------------------------------------------------------------

void BRE_P::unset_state (SAT_ClauseDatabase &clDB, int level)
{
    DBG1(Info("INSIDE unsat_state"););
    unset_implied_variables (clDB);

    register SAT_VariablePtr var;
    while (var = clDB.last_assigned_variable (level)) {
	unset_clauses_state (clDB, var);
	var->unset_state();
    }
    unset_unassigned_variables (clDB);
    clDB.del_tagged_clauses (GRAY);                  // Delete all GRAY clauses
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

void BRE_P::unset_state_w_conflict (SAT_ClauseDatabase &clDB, int level)
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
		    eval_n_tag_clause_size (clDB);

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
//               Since literals can be removed from variables, organization
//               needs to be slightly modified from exponential version.
//-----------------------------------------------------------------------------

void BRE_P::unset_clauses_state (SAT_ClauseDatabase &clDB, SAT_VariablePtr var)
{
    register LiteralListPtr plit, nplit;

    for (plit = var->literals().first(); plit;) {
	nplit = plit->next();

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

	if (cl->state() == UNRESOLVED && cl->test_tag (BLACK)) {
	    clDB.set_tagged_clause (cl);
	    cl->unset_tag (BLACK);
	    cl->set_tag (GRAY);                // Clause is going to be deleted
	    DBG0(cout<<"GOING TO SCHEDULE DELETION OF LARGE ";
		 cl->dump(cout);cout<<endl;);
	    _tot_discard_cl_number++;
	}
	plit = nplit;
    }
}


//-----------------------------------------------------------------------------
// Function: conf_clause_repack()
//
// Purpose: Traces conflicting clause literals to the decision variables and
//          recreates the conflicting clause using decision variables. This
//          technique is applied whenever the created confliting clause has
//          more literals than there are decision assignments.
//
// Side-effects: Existing conflicting clause is deleted and a new one is
//               created. For efficiency purposes, the traverse reference
//               of variables is used. This reference *must* not be in use
//               for any other traversal.
//-----------------------------------------------------------------------------

void BRE_P::conf_clause_repack (SAT_ClauseDatabase &clDB)
{
    if (_conf_clause->size() <= _mode[_CONF_CLAUSE_SIZE_] ||
	_conf_clause->size() <= clDB.active_decisions() ||
//	clDB.active_decisions() > _mode[_CONF_CLAUSE_SIZE_] ||
	!_mode[_CONF_CLAUSE_REPACKING_]) {
	return;
    }
    List<SAT_VariablePtr> sched_list, visited_list;
    SAT_ClausePtr new_clause = clDB.add_clause();

    SAT_VariableListPtr pvar;
    SAT_VariablePtr var;
    LiteralPtr lit;

    for_each(plit,_conf_clause->literals(),LiteralPtr) {
	lit = plit->data();
	var = (SAT_VariablePtr) lit->variable();
	if (var->value() == UNKNOWN) {
	    clDB.add_literal (new_clause, var, lit->sign());
	}
	else {
	    if (!var->test_tag (BLACK)) {
		var->set_tag (BLACK);
		sched_list.append (var->traverse_ref());
	    }
	}
    }
    while (pvar = sched_list.first()) {
	var = pvar->data();
	sched_list.extract (pvar);
	if (!var->antecedent()) {
	    CHECK(if (var->value() == UNKNOWN)
		  {var->dump(cout);cout<<endl;
		   Warn("UNASSIGNED VARIABLE IN TRACE??");});
	    clDB.add_literal (new_clause, var, var->value());
	}
	else {
	    for_each(plit,var->antecedent()->literals(),LiteralPtr) {
		lit = plit->data();
		SAT_VariablePtr avar = (SAT_VariablePtr) lit->variable();
		if (!avar->test_tag (BLACK)) {
		    avar->set_tag (BLACK);
		    sched_list.append (avar->traverse_ref());
		}
	    }
	}
	visited_list.append (pvar);
    }
    while (pvar = visited_list.first()) {
	var = pvar->data();
	visited_list.extract (pvar);
	var->unset_tag (BLACK);
    }
    CHECK(if(_mode[_DEBUG_])
	  {cout<<"  Replacing ";_conf_clause->dump(cout);cout<<endl;
	  cout<<"  With new ";new_clause->dump(cout);cout<<endl;});

    clDB.del_clause (_conf_clause);
    _conf_clause = new_clause;
}


//-----------------------------------------------------------------------------
// Function: output_stats()
//
// Purpose: Output stats associated with running GRASP on a clause database.
//-----------------------------------------------------------------------------

void BRE_P::output_stats (SAT_ClauseDatabase &clDB)
{
    BRE::output_stats (clDB);
    printItem ("Number of discarded conflicting clauses",
	       _tot_discard_cl_number);
    printItem();
}

/*****************************************************************************/
