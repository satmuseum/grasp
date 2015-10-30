//-----------------------------------------------------------------------------
// File: grp_CDB.cc
//
// Purpose: Definition of the member functions of the classes associated
//          with the implementation of the search for SAT.
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

#include "grp_CDB.hh"


extern void dump_clause_list (ClauseList &cl_list);
extern void dump_clause_literals (SAT_ClausePtr cl);


//-----------------------------------------------------------------------------
// Macro expansions for memory management of SAT_Variables and SAT_Clauses.
//-----------------------------------------------------------------------------

MEM_MNG_DEF(SAT_Variable, "SAT_Variable")

MEM_MNG_DEF(SAT_Clause, "SAT_Clause")



//-----------------------------------------------------------------------------
// Function: push_decision()
//
// Purpose: Get next available decision.
//
// Side-effects: Changes current decision level accordingly.
//-----------------------------------------------------------------------------

int SAT_ClauseDatabase::push_decision()
{
    DBG1(cout<<"PUSHING ANOTHER DECISION"<<endl;);
    _DLevel = NONE;
    DecisionListPtr pdec = _avail_decisions.first();
    if (pdec) {
	_avail_decisions.extract (pdec);

	DecisionPtr dec = pdec->data();
	_DLevel = dec->DLevel();
	dec->used() = TRUE;

	if (active_decisions() > _max_tree_depth) {
	    _max_tree_depth = active_decisions();
	}
	_dec_number++;
    }
    CHECK(if(_DLevel==NONE)
	  {check_consistency();
	   for(int k=0;k<_decision.size();k++){
	       if(!_decision[k]->used())Warn("Available DLevel after all...");
	       else if (!_decision[k]->assignedVariables().size()){
		   cout<<"\nInvalid usage of DLevel: "<<k;
		   cout<<" NO ASSIGNMENTS"<<endl;
		   Warn("Invalid decision entry allocation");}}
	   Abort("Returning DLevel=NONE for decision??");});
    DBG0(cout<<"ALLOCATING DLEVEL: "<<_DLevel<<endl;);
    return _DLevel;
}


//-----------------------------------------------------------------------------
// Function: pop_decision()
//
// Purpose: Free a decision level.
//
// Side-effects: A new decision level becomes available.
//-----------------------------------------------------------------------------

int SAT_ClauseDatabase::pop_decision (int level)
{
    CHECK(if(level != _DLevel) Warn("Popping level other than DLevel?");
	  if(level == NONE) Warn("Popping NONE as a DLevel??"););
    if (_decision[level]->used()) {
	DBG0(cout<<"RELEASING DLEVEL: "<<level<<endl;);
	_decision[level]->used() = FALSE;

	if (_avail_decisions.size()) {
	    DecisionListPtr pdec;
	    for (pdec = _avail_decisions.first(); pdec; pdec = pdec->next()) {
		DecisionPtr dec = pdec->data();
		if (level < dec->DLevel()) {
		    _avail_decisions.insertBefore
			(_decision[level]->avail_ref(), pdec);
		    break;
		}
	    }
	}
    }
    _DLevel = NONE;
    DBG1(cout<<"TOP DECISION LEVEL: ";
	 cout<<_avail_decisions.first()->data()->DLevel()<<endl;);
    return _DLevel;
}


//-----------------------------------------------------------------------------
// Function: init()
//
// Purpose: Initiates the search after a CNF formula has been defined. This
//          function is required since it guarantees allocation of the data
//          structures to be used.
//
// Side-effects: Data structures are set up.
//
// Notes: The number of decisions needs to be the number of variables + 1,
//        because decision level 0 is not associated with any decision
//        assignment.
//-----------------------------------------------------------------------------

void SAT_ClauseDatabase::init()
{
    DBG1(cout<<"INITIALIZING CLAUSE DATABASE!"<<endl;);
    int orig_size = _decision.size();
    if (_variables.size() > orig_size) {
	_decision.resize (_variables.size() + 1);

	for (int k = orig_size; k < _decision.size(); k++) {
	    if (k >= orig_size) {
		_decision[k] = new Decision (k);
	    }
	    _avail_decisions.append (_decision[k]->avail_ref());
	}
    }
    CHECK(if (!_lit_number) {Warn("NO Literals in DATABASE??");});
    _initial_var_number = variables().size();
    _initial_cl_number = clauses().size();
    _initial_lit_number = _lit_number;
}


//-----------------------------------------------------------------------------
// Function: clear()
//
// Purpose: Clear stats and Dlevel info.
//-----------------------------------------------------------------------------

void SAT_ClauseDatabase::clear()
{
    _DLevel = 0;

    CHECK(if (_satisfied_clauses)
	  {Warn("STARTING WITH SAT CLAUSES??");_satisfied_clauses = 0;});

    // Stats:

    _dec_number = 0;
    _max_tree_depth = 0;

    // OTHER requirements to be defined.

}


//-----------------------------------------------------------------------------
// Function: reset()
//
// Purpose: Reset internal structures of DB.
//
// Notes: Lots of information get lost. Thus this is inly used after DB
//        information is properly accounted for.
//-----------------------------------------------------------------------------

void SAT_ClauseDatabase::reset()
{
    _impliedVariables.reset();
    _unassignedVariables.reset();

    for(register int k = 0; k < _decision.size(); k++) {
	if (_decision[k]->used()) {
	    _decision[k]->assignedVariables().reset();
	    _decision[k]->used() = FALSE;
	}
	else { _avail_decisions.extract (_decision[k]->avail_ref()); }

	_avail_decisions.append (_decision[k]->avail_ref());
    }
    _taggedClauses.reset();
}



//-----------------------------------------------------------------------------
// Function: trim_solution()
//
// Purpose: Reports stats on used and unused decision assignments.
//
// Side-effects: Tags of used decision assignments are marked. They should
//               be cleared by another member function.
//-----------------------------------------------------------------------------

int SAT_ClauseDatabase::can_trim_solution (int &total_dec, int &required_dec)
{
    total_dec = _decision.size() - _avail_decisions.size();
    required_dec = 0;

    SAT_ClauseListPtr pcl;
    for (pcl = (SAT_ClauseListPtr) _clauses.first(); pcl; pcl = pcl->next()) {
	SAT_ClausePtr cl = pcl->data();

	for_each(plit, cl->literals(), LiteralPtr) {
	    LiteralPtr lit = plit->data();
	    SAT_VariablePtr var = (SAT_VariablePtr) lit->variable();
	    if (var->value() != UNKNOWN &&
		var->value() ^ lit->sign()) {
		if (!_decision[var->DLevel()]->test_tag (GRAY)) {
		    _decision[var->DLevel()]->set_tag (GRAY);
		    required_dec++;
		}
		break;
	    }
	}
    }
    for (register int k = 0; k < _decision.size(); k++) {
	if (_decision[k]->test_tag (GRAY)) { _decision[k]->unset_tag (GRAY); }
    }
    return (total_dec != required_dec);
}


//-----------------------------------------------------------------------------
// Function: resources_exceeded()
//
// Purpose: Tests whether available computational resources have been
//          exceeded.
//-----------------------------------------------------------------------------

int SAT_ClauseDatabase::resources_exceeded (SAT_ClauseDatabase &clDB)
{
    CHECK(if (!_initial_lit_number) Abort("LIT NUMBER IS ZERO!?"););
    int growth = _lit_number / _initial_lit_number;
    return (growth > _mode[_SPACE_LIMIT_]);
}


//-----------------------------------------------------------------------------
// Function: output_stats()
//
// Purpose: Outputs stats of running SAT algorithm which are directly
//          associated with the clause database.
//-----------------------------------------------------------------------------

void SAT_ClauseDatabase::output_stats()
{
    printItem ("Initial number of variables", _initial_var_number);
    printItem ("Initial number of clauses", _initial_cl_number);
    printItem ("Initial number of literals", _initial_lit_number);
    printItem();
    // **** printItem ("Final number of variables", _initial_var_number);
    printItem ("Final number of clauses", clauses().size());
    printItem ("Final number of literals", _lit_number);
    printItem();
    printItem ("Total number of decisions", _dec_number);
    printItem ("Largest depth of decision tree", _max_tree_depth);
    printItem();
}

//-----------------------------------------------------------------------------
// Function: output_assignments()
//
// Purpose: Output value of variables in clause database.
//-----------------------------------------------------------------------------

void SAT_ClauseDatabase::output_assignments (ostream &outs, int assigned_only)
{
    cout << "\n    Variable Assignments Satisfying CNF Formula:\n\t";
    for_each(pvar,variables(),VariablePtr) {
	SAT_VariablePtr var = (SAT_VariablePtr) pvar->data();
	if (var->value() != UNKNOWN) {
	    if (!var->value()) {
		cout << "-" << var->name() << " ";
	    } else {
		cout << var->name() << " ";
	    }
	}
    }
    cout << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: dump()
//
// Purpose: Dumps clause database and current state of the search process.
//-----------------------------------------------------------------------------

void SAT_ClauseDatabase::dump (int incremental, ostream &outs)
{
    SAT_VariableListPtr pvar;
    SAT_ClauseListPtr pcl;
    SAT_VariablePtr var;

    if (!incremental) {
	outs << "\nDumping clause database\n";
	outs << "  Number of Variables: " << _variables.size() << endl;
	outs << "  Number of Clauses: " << _clauses.size() << endl;
	outs << "    Clause Database: " << endl;
	int k = 0;
	for (pcl = (SAT_ClauseListPtr)_clauses.first(); pcl;
	     pcl = (SAT_ClauseListPtr)pcl->next()) {
	    outs << "      ";
	    ((SAT_ClausePtr) pcl->data())->dump (outs);
	    outs << "\t-> [" << ++k << "]" << endl;
	}
	outs << endl;
	outs << "    End of Clause Database\n" << endl;
    }
    else {
	outs << "\nIncrementally dumping clause database\n";
    }
    //-------------------------------------------------------------------------
    // Output implied variables
    //-------------------------------------------------------------------------

    outs << "    Implied assignments: " << _impliedVariables.size() << endl;
    for (pvar = _impliedVariables.first(); pvar; pvar=pvar->next()) {
	outs << "      ";
	var = pvar->data();
	if (!var->value()) outs << "-";
	outs << var->name() << " ";
    }
    outs << endl;

    //-------------------------------------------------------------------------
    // Output assigned variables
    //-------------------------------------------------------------------------

    outs << "    Assignments: " <<endl;
    if (!incremental) {
	for (int k = 0; k < _decision.size(); k++) {
	    if (!_decision[k]->used()) { continue; }
	    outs << "      DLevel: " << k << " => ";
	    for (pvar = _decision[k]->assignedVariables().first(); pvar;
		 pvar = pvar->next()) {
		var = pvar->data();
		if (!var->value()) outs << "-";
		outs << var->name() << " ";
	    }
	    outs << endl;
	}
	outs << endl;
    }
    else if (_DLevel != NONE) {
	outs << "      DLevel: " << _DLevel << " => ";
	for (pvar = _decision[_DLevel]->assignedVariables().first(); pvar;
	     pvar = pvar->next()) {
	    var = pvar->data();
	    if (!var->value()) outs << "-";
	    outs << var->name() << " ";
	}
	outs << endl;
    }
    CHECK(else {Warn("Requesting assigned vars at DLevel NONE??");});

    //-------------------------------------------------------------------------
    // Output unassigned variables.
    //-------------------------------------------------------------------------

    outs << "    Unassigned variables: " << _unassignedVariables.size() <<endl;
    for (pvar = _unassignedVariables.first(); pvar; pvar=pvar->next()) {
	outs << "      ";
	var = pvar->data();
	if (!var->value()) outs << "-";
	outs << var->name() << " ";
    }
    outs << endl;

    //-------------------------------------------------------------------------
    // Output unit clauses.
    //-------------------------------------------------------------------------

    outs << "    Unit clauses: " << _unitClauses.size() << endl;
    for (pcl = _unitClauses.first(); pcl; pcl=pcl->next()) {
	outs << "      ";
	pcl->data()->dump (outs); cout << endl;
    }
    outs << endl;

    //-------------------------------------------------------------------------
    // Output unsat clauses.
    //-------------------------------------------------------------------------

    outs << "    Unsat clauses: " << _unsatClauses.size() << endl;
    for (pcl = _unsatClauses.first(); pcl; pcl=pcl->next()) {
	outs << "      ";
	pcl->data()->dump (outs); cout << endl;
    }
    outs << endl;
    outs << "    Done with Clause Database"<<endl<<endl;
}


//-----------------------------------------------------------------------------
// Function: check_solution()
//
// Purpose: Checks whether current assignments satisfy all clauses in the
//          clause database.
//-----------------------------------------------------------------------------

int SAT_ClauseDatabase::check_solution()
{
    int non_sat_clauses = 0;
    SAT_ClauseListPtr pcl;
    for (pcl = (SAT_ClauseListPtr) _clauses.first(); pcl; pcl = pcl->next()) {
	SAT_ClausePtr cl = pcl->data();
	int satisfied = FALSE;
	for_each(plit, cl->literals(), LiteralPtr) {
	    LiteralPtr lit = plit->data();
	    SAT_VariablePtr var = (SAT_VariablePtr) lit->variable();
	    if (var->value() != UNKNOWN &&
		var->value() ^ lit->sign()) {
		satisfied = TRUE;
	    }
	}
	if (!satisfied) {
	    cout << "NON SATISFIED ";cl->dump(cout);cout<<endl;
	    non_sat_clauses++;
	}
    }
    if (non_sat_clauses) {
	cout<<"\tTOTAL NON SATISFIED CLAUSES: "<<non_sat_clauses<<endl<<endl;
    }
    return (non_sat_clauses > 0);
}


//-----------------------------------------------------------------------------
// Function: check_consistency()
//
// Purpose: Checks clause consistency for all clauses in database.
//          Checks are different depending on whether a consistent search
//          state is assumed.
//-----------------------------------------------------------------------------

int SAT_ClauseDatabase::check_consistency (int consistent_state)
{
    DBG1(cout<<"Entering clDB check_consistency"<<endl;
	 dump (FALSE, cout););
    int unit_number = 0;
    int unsat_number = 0;

    int inconsistent = FALSE;
    for_each(pcl,clauses(),ClausePtr) {
	SAT_ClausePtr cl = (SAT_ClausePtr) pcl->data();
	inconsistent = cl->check_consistency() || inconsistent;
	if (cl->state() == UNIT) { unit_number++; }
	if (cl->state() == UNSATISFIED) { unsat_number++; }
	if (cl->test_tag (GRAY)) {
	    cout << "Gray "; cl->dump(cout); cout<<endl;
	    Warn("In the presence of a GRAY tagged clause??");
	}
    }
    if (unit_number != _unitClauses.size()) {
	cout << "Counted UNIT clauses:  " << unit_number << endl;
	cout << "Existing UNIT clauses: " << _unitClauses.size() << endl;
	dump_clause_list ((ClauseList&)_unitClauses);
	Warn("Invalid number of unit clauses in list");
    }
    if (unsat_number != _unsatClauses.size()) {
	cout << "Counted UNSAT clauses:  " << unsat_number << endl;
	cout << "Existing UNSAT clauses: " << _unsatClauses.size() << endl;
	dump_clause_list ((ClauseList&)_unsatClauses);
	Warn("Invalid number of unsat clauses in list");
    }
    if (consistent_state && (_unsatClauses.size() || _unitClauses.size())) {
	Info("Unsat and/or unit clauses. Invalid consistency check place?");
    }
    DBG1(cout<<"Exiting clDB check_consistency"<<endl;);
    return inconsistent;
}


//-----------------------------------------------------------------------------
// Function: check_final_consistency()
//
// Purpose: Checks clause consistency for all clauses in database after
//          search terminates.
//          Checks are different depending on whether a consistent search
//          state is assumed.
//-----------------------------------------------------------------------------

int SAT_ClauseDatabase::check_final_consistency()
{
    int unit_number = 0;
    int unsat_number = 0;

    int inconsistent = FALSE;
    for_each(pcl,clauses(),ClausePtr) {
	SAT_ClausePtr cl = (SAT_ClausePtr) pcl->data();
	inconsistent = cl->check_consistency() || inconsistent;
	if (cl->state() == UNIT) { unit_number++; }
	if (cl->state() == UNSATISFIED) { unsat_number++; }
	if (!cl->sole_tag (WHITE)) {
	    cl->dump(cout);cout<<endl;
	    Warn("Invalid tag on clause");
	}
    }
    if (unit_number != _unitClauses.size()) {
	cout << "Counted UNIT clauses:  " << unit_number << endl;
	cout << "Existing UNIT clauses: " << _unitClauses.size() << endl;
	Warn("Invalid number of unit clauses in list");
    }
    if (unsat_number != _unsatClauses.size()) {
	cout << "Counted UNSAT clauses:  " << unsat_number << endl;
	cout << "Existing UNSAT clauses: " << _unsatClauses.size() << endl;
	Warn("Invalid number of unsat clauses in list");
    }
    for_each(pvar,variables(),VariablePtr) {
	SAT_VariablePtr var = (SAT_VariablePtr) pvar->data();
	if (var->value() != UNKNOWN || var->DLevel() != NONE) {
	    var->dump(cout);cout<<endl;
	    Warn("Invalid Value or DLevel on variable?");
	}
	if (!var->sole_tag (WHITE)) {
	    var->dump(cout);cout<<endl;
	    Warn("Invalid tag on variable");
	}
    }
    for (int k = 0; k < _decision.size(); k++) {
	if (_decision[k]->assignedVariables().first()) {
	    cout << "    DLevel = " << k << endl;
	    Warn("DLevel with assigned variables??");
	    if (!_decision[k]->assignedVariables().last()) {
		Warn("BUT with no last var pointer??");
	    }
	}
    }
    return inconsistent;
}


//-----------------------------------------------------------------------------
// Member functions for class SAT_Clause and SAT_Variable.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function: dump()
//
// Purpose: Dump clause information.
//-----------------------------------------------------------------------------

void SAT_Clause::dump (ostream &outs)
{
    Clause::dump (outs);
    outs << "\t#LB: " << _lowerB << " #UB: " << _upperB;
    outs << " #TH: " << _thres << " #ST: " << _state;
    outs << " #TAG: " << _tag << " #";
}


//-----------------------------------------------------------------------------
// Function: check_empty()
//
// Purpose: Checks if clause is properly empty.
//-----------------------------------------------------------------------------

int SAT_Clause::check_empty()
{
    int inconsistent = ((_upperB) || (_lowerB) || (_thres != 1) ||
			(_state != UNSATISFIED));
    return !inconsistent;
}


//-----------------------------------------------------------------------------
// Function: check_consistency()
//
// Purpose: Checks whether bounds are consistent.
//-----------------------------------------------------------------------------

int SAT_Clause::check_consistency()
{
    int LB = 0, UB = 0, TH = 1;
    int v_state;

    for_each(plit,literals(),LiteralPtr) {
	LiteralPtr lit = plit->data();
	SAT_VariablePtr var = (SAT_VariablePtr) lit->variable();

	if (lit->sign()) {
	    LB--;
	    TH--;
	} else {
	    UB++;
	}
	if (var->value() != UNKNOWN) {
	    int flag = (var->value() ^ lit->sign());
	    LB += flag;
	    UB -= 1-flag;
	}
    }
    if (_lowerB >= _thres)           { v_state = SATISFIED; }
    else if (_upperB < _thres)       { v_state = UNSATISFIED; }
    else if (_upperB - _lowerB == 1) { v_state = UNIT; }
    else                             { v_state = UNRESOLVED; }

    if (UB != _upperB || LB != _lowerB || TH != _thres || v_state != _state) {
	Warn("Invalid clause maintenance.");
	cout << "COMPUTED #LB: "<<LB<<" #UB: "<<UB<<" #TH: "<<TH;
	cout << "#ST: " << v_state <<endl;
	dump(cout);cout<<endl<<endl;
	return FALSE;
    }
    return TRUE;
}


//-----------------------------------------------------------------------------
// Function: dump()
//
// Purpose: Dump variable information.
//-----------------------------------------------------------------------------

void SAT_Variable::dump (ostream &outs)
{
    cout << "Variable: " << name();

    outs << "\t#value: " << _value << " #DL: " << _DLevel;
    outs << " #antec: " << _antec << " #TAG: " << _tag;
}


//-----------------------------------------------------------------------------
// Function: dump_clause_list()
//
// Purpose: Print out list of clauses.
//-----------------------------------------------------------------------------

void dump_clause_list (ClauseList &cl_list)
{
    for_each(pcl,cl_list,ClausePtr) {
	SAT_ClausePtr cl = (SAT_ClausePtr) pcl->data();
	cl->dump (cout); cout << endl;
	dump_clause_literals (cl);
    }
}

//-----------------------------------------------------------------------------
// Function: dump_clause_literals()
//
// Purpose: Print out list literals of a clause.
//-----------------------------------------------------------------------------

void dump_clause_literals (SAT_ClausePtr cl)
{
    for_each(plit,cl->literals(),LiteralPtr) {
	LiteralPtr lit = plit->data();
	cout << "\t"; ((SAT_VariablePtr)lit->variable())->dump (cout);
	cout << endl;
    }
}

/*****************************************************************************/

