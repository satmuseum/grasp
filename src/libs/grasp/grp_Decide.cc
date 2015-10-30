//-----------------------------------------------------------------------------
// File: grp_Decide.cc
//
// Purpose: Functions used for decision making.
//
// Remarks: --
//
// History: 6/23/95 - JPMS - created.
//          3/16/96 - JPMS - adapted for new version of GRASP.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "grp_Decide.hh"



//-----------------------------------------------------------------------------
// Local defines and variable definitions.
//-----------------------------------------------------------------------------

# define MAX_RELEVANT_SIZE  5

static int shifted_pivot[MAX_RELEVANT_SIZE+1];

# define Balpha 1
# define Bbeta  2



//-----------------------------------------------------------------------------
// Function: init()
//
// Purpose: Prepare engine for decision making.
//
// Notes: Array resizing is expected to be applied *solely* to arrays of size
//        0. Otherwise, allocated var ordering entries will be lost! Moreover,
//        orderings can be specified *ONLY* after resizing has taken place.
//-----------------------------------------------------------------------------

void DecisionEngine::init (SAT_ClauseDatabase &clDB)
{
    if (_mode[_DECISION_MODE_] == _STATIC_ORD_) {
	_dec_ptr = 0;
	if (clDB.variables().size() > _sdec_sched.size()) {
	    _sdec_sched.resize (clDB.variables().size());
	    _sdec_order.resize (clDB.variables().size());

	    SAT_VariableListPtr pvar =
		(SAT_VariableListPtr) clDB.variables().first();
	    for (; pvar; pvar = pvar->next()) {
		SAT_VariablePtr var = pvar->data();
		_sdec_order[var->ID()] = new VarOrdering (var);
		_sdec_sched[var->ID()] = var;
		DBG1(cout<<"SCHEDULING DEC VAR: "<<var->name()<<endl;);
	    }
	    _max_sched = _sdec_sched.size()-1;
	    DBG1(cout<<"TOTAL DECISION VARS: "<<_max_sched<<endl;);
	}
	else { reorder_variables (clDB); }      // Reorder only if not resizing
    }
    else {
        unsigned int pivot = 1;
        for (int k = 0; k < MAX_RELEVANT_SIZE; k++) {
            shifted_pivot[k] = pivot << k;
        }
    }
}


//-----------------------------------------------------------------------------
// Function: reset()
//
// Purpose: Clear internal structures after SAT instances has been solved.
//-----------------------------------------------------------------------------

void DecisionEngine::reset (SAT_ClauseDatabase &clauseDB)
{

}

//-----------------------------------------------------------------------------
// Function: clear()
//
// Purpose: Clear internal structures for subsequent SAT instance.
//-----------------------------------------------------------------------------

void DecisionEngine::clear (SAT_ClauseDatabase &clauseDB)
{

}

//-----------------------------------------------------------------------------
// Function: backtrack()
//
// Purpose: Set decision pointer at the backtracking level. When decide() is
//          called this pointer is then set to an unassigned variable.
//-----------------------------------------------------------------------------

void DecisionEngine::backtrack (SAT_ClauseDatabase &clDB, int back_level)
{
//    register int k;
//    for (k = 0; k < back_level; k++) {
//	if (_sdec_sched[k]->value() == UNKNOWN) { break; }
//    }
//    _dec_ptr = k;
    CHECK(for (int k = 0; k < back_level-1; k++)
	  {if (_sdec_sched[k] && _sdec_sched[k]->value() == UNKNOWN)
	       {cout<<"UNASSIGNED DECISION VARIABLE: "<<_sdec_sched[k]->name();
		cout<<endl;Warn("PREVIOUS var with UNKNOWN value???");}});
    if (_dec_ptr > back_level-1) { _dec_ptr = back_level-1; }
}


//-----------------------------------------------------------------------------
// Function: set_decision_order()
//
// Purpose: Specifies decision order for a variable.
//-----------------------------------------------------------------------------

void DecisionEngine::set_decision_order (SAT_VariablePtr var, int order)
{
    if (_mode[_DECISION_MODE_] == _STATIC_ORD_) {
	_sdec_order[var->ID()]->order() = order;
    }
}

//-----------------------------------------------------------------------------
// Function: set_preferred_value()
//
// Purpose: Chooses decision value for a variable.
//-----------------------------------------------------------------------------

void DecisionEngine::set_preferred_value (SAT_VariablePtr var, int value)
{
    if (_mode[_DECISION_MODE_] == _STATIC_ORD_) {
	_sdec_order[var->ID()]->pref_value() = value;
    }
}


//-----------------------------------------------------------------------------
// Function: reorder_variables()
//
// Purpose: Distributes variables according to chosen ordering.
//
// Notes: Number of variables with ANY order plus number of variables with
//        chosen order *must* match the total number of variables.
//-----------------------------------------------------------------------------

void DecisionEngine::reorder_variables (SAT_ClauseDatabase &clDB)
{
    SAT_VariableListPtr pvar = (SAT_VariableListPtr) clDB.variables().last();
    int _bott_idx = clDB.variables().size()-1;

    for (; pvar; pvar = pvar->prev()) {
	SAT_VariablePtr var = pvar->data();
	int var_order = _sdec_order[var->ID()]->order();
	if (var_order == ANY) { _sdec_sched[_bott_idx--] = var; }
	else                  { _sdec_sched[var_order] = var; }
	DBG1(cout<<"SCHEDULING DEC VAR: "<<var->name()<<endl;);
    }
}



//-----------------------------------------------------------------------------
// Function: output_stats()
//
// Purpose: Output information regarding decision making statistics.
//-----------------------------------------------------------------------------

void DecisionEngine::output_stats (SAT_ClauseDatabase &clauseDB)
{

}


//-----------------------------------------------------------------------------
// Static and dynamic assignment selection functions.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function: static_assignment_select()
//
// Purpose: Given a static ordering of the decision variables, it selects
//          the next assignment.
//
// Side-effects: If available, an assignment is made.
//-----------------------------------------------------------------------------

int DecisionEngine::static_assignment_select (SAT_ClauseDatabase &clDB)
{
    DBG1(cout<<"Making static decision assignment"<<endl;);
    while (_dec_ptr < _sdec_sched.size() &&
	   _sdec_sched[_dec_ptr]->value() != UNKNOWN) {
	_dec_ptr++;
    }
    if (_dec_ptr < _sdec_sched.size()) {
	CHECK(if(_dec_ptr > _max_sched)
	      Warn("Deciding above max number of decision vars?"););
	SAT_VariablePtr var = _sdec_sched[_dec_ptr];
	elect_assignment (clDB, var, _sdec_order[var->ID()]->pref_value());
	return DECISION;
    }
    DBG1(cout << "Solution found\n"<<endl);
    return SOLUTION;
}


//-----------------------------------------------------------------------------
// Function: dynamic_assignment_select()
//
// Purpose: For the current state of the search, and according to a chosen
//          ordering heuristic, a variable assignment is dynamically selected.
//
// Side-effects: A variable assignment is selected.
//-----------------------------------------------------------------------------

int DecisionEngine::dynamic_assignment_select (SAT_ClauseDatabase &clDB)
{
    DBG1(cout<<"Making dyamic decision assignment"<<endl;);
    int outcome;
    switch( _mode[_DECISION_LEVEL_] ) {
      case _MSOS_:
        outcome = select_MSOS (clDB);
        break;
      case _MSTS_:
        outcome = select_MSTS (clDB);
        break;
      case _MSMM_:
        outcome = select_MSMM (clDB);
        break;
      case _BOHM_:
        outcome = select_BOHM (clDB);
        break;
      case _DLIS_:
        outcome = select_DLIS (clDB);
        break;
      case _DLCS_:
        outcome = select_DLCS (clDB);
        break;
      case _DJW_:
      default:
        Abort( "Invalid dynamic selection option" );
        break;
    }
    return outcome;
}


//-----------------------------------------------------------------------------
// Dynamic ordering for decision variables.
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Ordering procedures for dynamic decision making.
//
// Key: MSOS - my own version of the one-sided JW-based heuristic.
//      MSTS - my own version of the two-sided JW-based heuristic.
//      MSMM - my own version of the MOM's heuristic.
//      BOHM - Bohm's heuristic.
//      DLIS - dynamic variation of LIS.
//      DLCS - dynamic variation of LCS.
//-----------------------------------------------------------------------------

int DecisionEngine::select_MSOS (SAT_ClauseDatabase &clDB)
{
    DBG1(cout<<"Making MSOS dynamic decision assignment"<<endl;);
    int target_figure = (-1);
    SAT_VariablePtr target_var = NULL;
    int target_value = NONE;

    for_each(pvar,clDB.variables(),VariablePtr) {
	register SAT_VariablePtr var = (SAT_VariablePtr) pvar->data();
	register int figureT = 0, figureF = 0;
	if (var->value() != UNKNOWN) { continue; }

	// Traverse literals of var and update figures.

	for_each(plit,var->literals(),LiteralPtr) {
	    LiteralPtr lit = plit->data();
	    SAT_ClausePtr cl = (SAT_ClausePtr) lit->clause();

	    if (cl->state() == SATISFIED ||
	        cl->free_literals() > MAX_RELEVANT_SIZE) { continue; }
	    int sp_val = shifted_pivot[MAX_RELEVANT_SIZE-cl->free_literals()];
	    if (!lit->sign()) {
		figureT++;              // Clause that is satisfied if var is T
		figureF += sp_val;
	    } else {
		figureF++;              // Clause that is satisfied if var is F
		figureT += sp_val;
	    }
	}
	if (figureT >= figureF && figureT > target_figure) {
	    target_var = var;
	    target_value = TRUE;
	    target_figure = figureT;
	}
	else if (figureF >= figureT && figureF > target_figure) {
	    target_var = var;
	    target_value = FALSE;
	    target_figure = figureF;
	}
    }
    if (!target_var) { return SOLUTION; }
    elect_assignment (clDB, target_var, target_value );
    return DECISION;
}

int DecisionEngine::select_MSTS (SAT_ClauseDatabase &clDB)
{
    DBG1(cout<<"Making MSTS dynamic decision assignment"<<endl;);
    int target_figure = (-1);
    SAT_VariablePtr target_var = NULL;
    int target_value = NONE;

    for_each(pvar,clDB.variables(),VariablePtr) {
	register SAT_VariablePtr var = (SAT_VariablePtr) pvar->data();
	register int figureT = 0, figureF = 0;
	if (var->value() != UNKNOWN) { continue; }

	// Traverse literals of var and update figures.

	for_each(plit,var->literals(),LiteralPtr) {
	    LiteralPtr lit = plit->data();
	    SAT_ClausePtr cl = (SAT_ClausePtr) lit->clause();

	    if (cl->state() == SATISFIED ||
	        cl->free_literals() > MAX_RELEVANT_SIZE) { continue; }
	    int sp_val = shifted_pivot[MAX_RELEVANT_SIZE-cl->free_literals()];
	    if (!lit->sign()) {
		figureT++;              // Clause that is satisfied if var is T
		figureF += sp_val;
	    } else {
		figureF++;              // Clause that is satisfied if var is F
		figureT += sp_val;
	    }
	}
	if( (figureT + figureF) > target_figure ) {
	    target_var = var;
	    target_value = (figureT >= figureF) ? TRUE : FALSE;
	    target_figure = figureT + figureF;
	}
    }
    if (!target_var) { return SOLUTION; }
    elect_assignment (clDB, target_var, target_value );
    return DECISION;
}

int DecisionEngine::select_MSMM (SAT_ClauseDatabase &clDB)
{
    //-------------------------------------------------------------------------
    // Compute H(x) and W(x). H(x) is defined as a figure of merit for a given
    // clause size, whereas W(x) is a figure of merit for x over a range of
    // clause sizes. These two figures are used for finding a 2-dimensional
    // maximum figure.
    //-------------------------------------------------------------------------
    DBG1(cout<<"Making MSMM dynamic decision assignment"<<endl;);

    int H_figure = (-1);
    int W_figure = (-1);
    // The highest figure of merit is defined by the smallest clause.
    int min_index = clDB.clauses().size();

    SAT_VariablePtr target_var = NULL;
    int target_value = NONE;

    for_each(pvar,clDB.variables(),VariablePtr) {
	register SAT_VariablePtr var = (SAT_VariablePtr) pvar->data();
	register int H_var_figure = 0, W_var_figure = 0;
	register int H_varb_figure = 0, W_varb_figure = 0;
	register int var_min_index = clDB.clauses().size();

	if (var->value() != UNKNOWN) { continue; }

	// Traverse literals of var and update figures.

	for_each(plit,var->literals(),LiteralPtr) {
	    LiteralPtr lit = plit->data();
	    SAT_ClausePtr cl = (SAT_ClausePtr) lit->clause();

	    if (cl->state() == SATISFIED) { continue; }

	    if (!lit->sign()) {
		if (cl->free_literals() < var_min_index) {
		    var_min_index = cl->free_literals();
		    H_var_figure = 1;     // Reset H(x) given the new min index
		}
		else if (cl->free_literals() == var_min_index) {
		    H_var_figure++;
		}
		W_var_figure += (cl->free_literals() <= MAX_RELEVANT_SIZE) ?
		    shifted_pivot[MAX_RELEVANT_SIZE - cl->free_literals()] : 0;
	    }
	    else {
		if (cl->free_literals() < var_min_index) {
		    var_min_index = cl->free_literals();
		    H_varb_figure = 1;
		    H_var_figure = 0;
		}
		else if (cl->free_literals() == var_min_index) {
		    H_varb_figure++;
		}
		W_varb_figure += (cl->free_literals() <= MAX_RELEVANT_SIZE) ?
		    shifted_pivot[MAX_RELEVANT_SIZE - cl->free_literals()] : 0;
	    }
	}
	int tmp_H_figure =
	    H_var_figure*H_varb_figure + H_var_figure + H_varb_figure;
	int tmp_W_figure = W_var_figure + W_varb_figure;

	if (var_min_index < min_index ||
	    var_min_index == min_index &&
	    (tmp_H_figure > H_figure ||
	     tmp_H_figure == H_figure && tmp_W_figure > W_figure) ) {
	    min_index = var_min_index;
	    H_figure = tmp_H_figure;
	    W_figure = tmp_W_figure;
	    target_var = var;
	    // Using H figure to decide selected value.
	    target_value = (H_var_figure > H_varb_figure) ? FALSE : TRUE;
	}
    }
    if (!target_var) { return SOLUTION; }
    elect_assignment (clDB, target_var, target_value );
    return DECISION;
}

int DecisionEngine::select_BOHM (SAT_ClauseDatabase &clDB)
{
    //-------------------------------------------------------------------------
    // Compute H(x) and W(x). H(x) is defined as a figure of merit for a given
    // clause size, whereas W(x) is a figure of merit for x over a range of
    // clause sizes. These two figures are used for finding a 2-dimensional
    // maximum figure.
    //-------------------------------------------------------------------------
    DBG1(cout<<"Making MSMM dynamic decision assignment"<<endl;);

    int H_figure = (-1);
    int W_figure = (-1);
    // The highest figure of merit is defined by the smallest clause.
    int min_index = clDB.clauses().size();

    SAT_VariablePtr target_var = NULL;
    int target_value = NONE;

    for_each(pvar,clDB.variables(),VariablePtr) {
	register SAT_VariablePtr var = (SAT_VariablePtr) pvar->data();
	register int H_var_figure = 0, W_var_figure = 0;
	register int H_varb_figure = 0, W_varb_figure = 0;
	register int var_min_index = clDB.clauses().size();

	if (var->value() != UNKNOWN) { continue; }

	// Traverse literals of var and update figures.

	for_each(plit,var->literals(),LiteralPtr) {
	    LiteralPtr lit = plit->data();
	    SAT_ClausePtr cl = (SAT_ClausePtr) lit->clause();

	    if (cl->state() == SATISFIED) { continue; }

	    if (!lit->sign()) {
		if (cl->free_literals() < var_min_index) {
		    var_min_index = cl->free_literals();
		    H_var_figure = 1;     // Reset H(x) given the new min index
		}
		else if (cl->free_literals() == var_min_index) {
		    H_var_figure++;
		}
		W_var_figure += (cl->free_literals() <= MAX_RELEVANT_SIZE) ?
		    shifted_pivot[MAX_RELEVANT_SIZE - cl->free_literals()] : 0;
	    }
	    else {
		if (cl->free_literals() < var_min_index) {
		    var_min_index = cl->free_literals();
		    H_varb_figure = 1;
		    H_var_figure = 0;
		}
		else if (cl->free_literals() == var_min_index) {
		    H_varb_figure++;
		}
		W_varb_figure += (cl->free_literals() <= MAX_RELEVANT_SIZE) ?
		    shifted_pivot[MAX_RELEVANT_SIZE - cl->free_literals()] : 0;
	    }
	}
	int minH = (H_var_figure<H_varb_figure) ? H_var_figure : H_varb_figure;
	int maxH = (H_var_figure>H_varb_figure) ? H_var_figure : H_varb_figure;
	int tmp_H_figure = Balpha*maxH + Bbeta*minH;
	int tmp_W_figure = W_var_figure + W_varb_figure;

	if( var_min_index < min_index ||
	    var_min_index == min_index &&
	    (tmp_H_figure > H_figure ||
	     tmp_H_figure == H_figure && tmp_W_figure > W_figure) ) {
	    min_index = var_min_index;
	    H_figure = tmp_H_figure;
	    W_figure = tmp_W_figure;
	    target_var = var;
	    // Using H figure to decide selected value.
	    target_value = (H_var_figure > H_varb_figure) ? FALSE : TRUE;
	}
    }
    if (!target_var) { return SOLUTION; }
    elect_assignment (clDB, target_var, target_value );
    return DECISION;
}

int DecisionEngine::select_DLIS (SAT_ClauseDatabase &clDB)
{
    DBG1(cout<<"Making DLIS dynamic decision assignment"<<endl;);
    int target_figure = (-1);
    SAT_VariablePtr target_var = NULL;
    int target_value = NONE;

    for_each(pvar,clDB.variables(),VariablePtr) {
	register SAT_VariablePtr var = (SAT_VariablePtr) pvar->data();
	register int figureT = 0, figureF = 0;
	if (var->value() != UNKNOWN) { continue; }

	// Traverse literals of var and update figures.

	for_each(plit,var->literals(),LiteralPtr) {
	    LiteralPtr lit = plit->data();
	    SAT_ClausePtr cl = (SAT_ClausePtr) lit->clause();

	    if (cl->state() == SATISFIED) continue;
	    if (!lit->sign()) {
		figureT++;              // Clause that is satisfied if var is T
	    } else {
		figureF++;              // Clause that is satisfied if var is F
	    }
	}
	if (figureT >= figureF && figureT > target_figure) {
	    target_var = var;
	    target_value = TRUE;
	    target_figure = figureT;
	}
	else if (figureF >= figureT && figureF > target_figure) {
	    target_var = var;
	    target_value = FALSE;
	    target_figure = figureF;
	}
    }
    if (!target_var) { return SOLUTION; }
    elect_assignment (clDB, target_var, target_value );
    return DECISION;
}

int DecisionEngine::select_DLCS (SAT_ClauseDatabase &clDB)
{
    DBG1(cout<<"Making DLCS dynamic decision assignment"<<endl;);
    int target_figure = (-1);
    SAT_VariablePtr target_var = NULL;
    int target_value = NONE;

    for_each(pvar,clDB.variables(),VariablePtr) {
	register SAT_VariablePtr var = (SAT_VariablePtr) pvar->data();
	register int figureT = 0, figureF = 0;
	if (var->value() != UNKNOWN) { continue; }

	// Traverse literals of var and update figures.

	for_each(plit,var->literals(),LiteralPtr) {
	    LiteralPtr lit = plit->data();
	    SAT_ClausePtr cl = (SAT_ClausePtr) lit->clause();

	    if (cl->state() == SATISFIED) continue;
	    if (!lit->sign()) {
		figureT++;              // Clause that is satisfied if var is T
	    } else {
		figureF++;              // Clause that is satisfied if var is F
	    }
	}
	if ((figureT + figureF) > target_figure) {
	    target_var = var;
	    target_value = (figureT > figureF) ? TRUE : FALSE;
	    target_figure = figureT + figureF;
	}
    }
    if (!target_var) { return SOLUTION; }
    elect_assignment (clDB, target_var, target_value );
    return DECISION;
}


//-----------------------------------------------------------------------------
// Static ordering for decision variables.
//-----------------------------------------------------------------------------


/*****************************************************************************/
