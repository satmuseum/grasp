//-----------------------------------------------------------------------------
// File: grp_CDB.hh
//
// Purpose: CNF formula definition within a search context.
//
// Remarks: --
//
// History: 03/10/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef __GRP_CDB__
#define __GRP_CDB__

#include <iostream.h>

#include "defs.hh"
#include "memory.hh"
#include "list.hh"

#include "grp_CNF.hh"
#include "grp_Mode.hh"


//-----------------------------------------------------------------------------
// Class names and typedefs used for simplying interface with clause DBs.
//-----------------------------------------------------------------------------

class SAT_Variable;
class SAT_Clause;
class Decision;

typedef SAT_Variable *SAT_VariablePtr;
typedef List<SAT_Variable*> SAT_VariableList;
typedef ListItem<SAT_Variable*> *SAT_VariableListPtr;

typedef SAT_Clause *SAT_ClausePtr;
typedef List<SAT_Clause*> SAT_ClauseList;
typedef ListItem<SAT_Clause*> *SAT_ClauseListPtr;

typedef Decision *DecisionPtr;
typedef List<Decision*> DecisionList;
typedef ListItem<Decision*> *DecisionListPtr;



//-----------------------------------------------------------------------------
// Defines for clause databases in a search context.
//-----------------------------------------------------------------------------

#define UNKNOWN    2

enum ClauseState {
    UNRESOLVED = 0x10,       // Clause that can become SATISFIED or UNSATISFIED
    UNIT,                    // UNRESOLVED clause BUT with 1 free literal
    SATISFIED,               // Clause is SATISFIED by some literal
    UNSATISFIED              // All literals assigned and clause is UNSATISFIED
    };


//-----------------------------------------------------------------------------
// Class: SAT_Variable
//
// Purpose: Class declaration of variables of a CNF formula in a search
//          context.
//-----------------------------------------------------------------------------

class SAT_Variable : public Variable {
  public:

    //-------------------------------------------------------------------------
    // Constructor / destructor.
    //-------------------------------------------------------------------------

    SAT_Variable (char *Nname, int nID) : Variable (Nname, nID) {
	_value = UNKNOWN;
	_DLevel = NONE;
	_tag = WHITE;
	_antec = NULL;

	_assign_ref = new ListItem<SAT_Variable*> (this);
	_traverse_ref = new ListItem<SAT_Variable*> (this);
    }
    virtual ~SAT_Variable () {
	delete _assign_ref;
	delete _traverse_ref;
    }

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    inline int &value() { return _value; }
    inline int &DLevel() { return _DLevel; }
    inline SAT_ClausePtr antecedent() { return _antec; }

    inline int set_state (int val, int dlevel, SAT_ClausePtr ant);
    inline int unset_state();

    inline SAT_VariableListPtr traverse_ref() { return _traverse_ref; }
    inline SAT_VariableListPtr assign_ref() { return _assign_ref; }

    //-------------------------------------------------------------------------
    // Tag management.
    //-------------------------------------------------------------------------

    inline int test_tag (int ntag) { return _tag & ntag; }
    inline int sole_tag (int ntag) { return (_tag ^ ntag == 0); }
    inline int set_tag (int ntag) { _tag |= ntag; return _tag; }
    inline int unset_tag (int ntag) { _tag &= ~ntag; return _tag; }
    inline int &tag() { return _tag; }

    //-------------------------------------------------------------------------
    // Dumping variable information
    //-------------------------------------------------------------------------

    virtual void dump (ostream &outs);

    //-------------------------------------------------------------------------
    // Memory management of objects of class SAT_Variable.
    //-------------------------------------------------------------------------

    MEM_MNG_DECL(SAT_Variable, "SAT_Variable")

  private:
    int _value;            // Value assumed by variable
    int _DLevel;           // Decision level at which variable is assigned
    SAT_ClausePtr _antec;  // Clause causing assignment of variable

    int _tag;              // General purpose tag associated with each variable

    SAT_VariableListPtr _assign_ref;  // Var ref when it is implied or assigned
    SAT_VariableListPtr _traverse_ref;           // Var ref when for traversals
};


//-----------------------------------------------------------------------------
// Class: SAT_Clause
//
// Purpose: Clause declaration in the context of search.
//-----------------------------------------------------------------------------

class SAT_Clause : public Clause {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    SAT_Clause () : Clause() {
	_thres = 1;
	_lowerB = _upperB = 0;
	_state = UNSATISFIED;

	_tag = WHITE;

	_implied_var = NULL;

	_traverse_ref = new ListItem<SAT_Clause*> (this);
	_consist_ref = new ListItem<SAT_Clause*> (this);
    }
    virtual ~SAT_Clause() {
	delete _traverse_ref;
	delete _consist_ref;
    }

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    inline int state() { return _state; }

    inline SAT_VariablePtr &implied_var() { return _implied_var; }

    inline int free_literals() { return (_upperB - _lowerB); }
    inline int assigned_literals()
	{ return (_literals.size() - free_literals()); }

    inline int update_state () {
	if (_lowerB >= _thres) {
	    _state = SATISFIED;
	}
	else if (_upperB < _thres) {
	    _state = UNSATISFIED;
	}
	else if (_upperB - _lowerB == 1) {
	    _state = UNIT;
	}
	else {
	    _state = UNRESOLVED;
	}
	return _state;
    }
    inline int set_state (LiteralPtr lit) {
	SAT_VariablePtr var = (SAT_VariablePtr) lit->variable();
	int flag = (var->value() ^ lit->sign());
	_lowerB += flag;
	_upperB -= 1-flag;
	return update_state();
    }
    inline int unset_state (LiteralPtr lit) {
	SAT_VariablePtr var = (SAT_VariablePtr) lit->variable();
	CHECK(if (var->value() != TRUE && var->value() != FALSE)
	      {Warn("Invalid literal for clause state updating!");
	       cout<<"Variable literal: ";var->dump(cout);cout<<endl;});
	int flag = (var->value() ^ lit->sign());
	_lowerB -= flag;
	_upperB += 1-flag;
	return update_state();
    }
    inline int unset_state() {
	_lowerB = _thres - 1;
	_upperB = literals().size() + _lowerB;
	return update_state();
    }
    inline void set_bounds (LiteralPtr lit) {
	if (lit->sign()) {
	    _lowerB--;
	    _thres--;
	}
	else { _upperB++; }
    }
    inline void unset_bounds (LiteralPtr lit) {
	if (lit->sign()) {
	    _lowerB++;
	    _thres++;
	}
	else { _upperB--; }
    }

    inline int size() { return literals().size(); }

    inline SAT_ClauseListPtr traverse_ref() { return _traverse_ref; }
    inline SAT_ClauseListPtr consist_ref() { return _consist_ref; }

    //-------------------------------------------------------------------------
    // Tag management.
    //-------------------------------------------------------------------------

    inline int test_tag (int ntag) { return _tag & ntag; }
    inline int sole_tag (int ntag) { return (_tag ^ ntag == 0); }
    inline int set_tag (int ntag) { _tag |= ntag; return _tag; }
    inline int unset_tag (int ntag) { _tag &= ~ntag; return _tag; }
    inline int &tag() { return _tag; }

    //-------------------------------------------------------------------------
    // Debugging and dumping clause contents. Consistency checks.
    //-------------------------------------------------------------------------
    
    virtual void dump (ostream &outs);
    virtual int test_valid() { return _upperB >= _lowerB; }

    virtual int check_empty();
    virtual int check_consistency();

    //-------------------------------------------------------------------------
    // Memory management of objects of class SAT_Clause.
    //-------------------------------------------------------------------------

    MEM_MNG_DECL(SAT_Clause, "SAT_Clause")

  private:
    int _state;       // Whether clause is satisfied, unresolved or unsatisfied

    int _lowerB;      // Lower estimate on the sum of variable values
    int _upperB;      // Upper estimate on the sum of variable values
    int _thres;       // Threshold that for clause to be satisfied

    int _tag;         // General purpose tag associated with each clause

    SAT_VariablePtr _implied_var;       // Var implied due to clause being unit

    SAT_ClauseListPtr _consist_ref;  // Clause ref requiring consistency action
    SAT_ClauseListPtr _traverse_ref;               // Clause ref for traversals
};


//-----------------------------------------------------------------------------
// Class: Decision
//
// Purpose: Keeps information regarding a given decision assignment.
//-----------------------------------------------------------------------------

class Decision {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    Decision (int level) : _assignedVariables() {
	_DLevel = level;_used = FALSE;
	_avail_ref = new ListItem<Decision*> (this);
	_tag = WHITE;
    }
    virtual ~Decision() { delete _avail_ref; }

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    inline int DLevel() { return _DLevel; }
    inline int &used() { return _used; }
    inline DecisionListPtr avail_ref() { return _avail_ref; }
    inline SAT_VariableList &assignedVariables() { return _assignedVariables; }

    //-------------------------------------------------------------------------
    // Tag management.
    //-------------------------------------------------------------------------

    inline int test_tag (int ntag) { return _tag & ntag; }
    inline int sole_tag (int ntag) { return (_tag ^ ntag == 0); }
    inline int set_tag (int ntag) { _tag |= ntag; return _tag; }
    inline int unset_tag (int ntag) { _tag &= ~ntag; return _tag; }
    inline int &tag() { return _tag; }

  protected:

    int _DLevel;                             // DLevel associated with Decision
    int _used;                 // Indicates whether this decision is being used
    DecisionListPtr _avail_ref;    // Reference to this decision when available
    SAT_VariableList _assignedVariables;                  // Assigned variables

    int _tag;              // General purpose tag associated with each decision

  private:

};


//-----------------------------------------------------------------------------
// Class: SAT_ClauseDatabase
//
// Purpose: A clause database for the purpose of search for SAT must maintain
//          structural information as well as information regarding consistency
//          and constraining conditions.
//-----------------------------------------------------------------------------

class SAT_ClauseDatabase : public ClauseDatabase {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    SAT_ClauseDatabase (SAT_Mode &nmode) :
    ClauseDatabase(), _unitClauses(), _unsatClauses(),
    _impliedVariables(), _unassignedVariables(), _taggedClauses(),
    _decision(0), _mode(nmode) {
	_satisfied_clauses = 0;_DLevel = NONE;
	_initial_var_number = 0;
	_initial_cl_number = 0;
	_initial_lit_number = 0;
	_lit_number = 0;
	_dec_number = 0;
	_max_tree_depth = 0;
    }
    virtual ~SAT_ClauseDatabase() {
	_decision.resize(0);
	_avail_decisions.erase();
	_impliedVariables.erase();
	_unassignedVariables.erase();
    }

    //-------------------------------------------------------------------------
    // Interface contract.
    // **A** Functions for modifying the internal structure of the database.
    //-------------------------------------------------------------------------

    inline SAT_VariablePtr add_variable (char *vname = NULL) {
	SAT_VariablePtr var = new SAT_Variable (vname, _varIDs++);
	_variables.append (var->cnf_ref());
	return var;
    }
    inline void del_variable (SAT_VariablePtr var) {
	if (var->value() != UNKNOWN) {
	    _decision[var->DLevel()]->assignedVariables()
		.extract (var->assign_ref());
	}
	// Extract and delete all literals if any
	LiteralListPtr plit;
	while (plit = var->literals().first()) {
	    del_literal (plit->data());
	}
	_variables.extract (var->cnf_ref());
	delete var;
    }

    inline SAT_ClausePtr add_clause() {
	SAT_ClausePtr cl = new SAT_Clause();
	_clauses.append (cl->cnf_ref());
	_unsatClauses.append (cl->consist_ref());
	return cl;
    }
    inline void del_clause (SAT_ClausePtr cl) {
	// Extract and delete all literals
	DBG1(cout<<"DELETING ";cl->dump(cout);cout<<endl;);
	CHECK(cl->check_consistency(););
	LiteralListPtr plit;
	while (plit = cl->literals().first()) {
	    del_literal (plit->data());
	}
	CHECK(if (!cl->check_empty())
	      {Warn("Inconsistent status of clause");
	       cl->dump(cout);cout<<endl;});
	_clauses.extract (cl->cnf_ref());
	_unsatClauses.extract (cl->consist_ref());
	delete cl;
    }
    inline LiteralPtr add_literal (SAT_ClausePtr cl,
				   SAT_VariablePtr var, int sign) {
	int c_state = cl->state();

	LiteralPtr lit = new Literal (cl, var, sign);
	var->add_literal (lit->variable_ref());
	cl->add_literal (lit->clause_ref());
	cl->set_bounds (lit);

	int n_state = (var->value() != UNKNOWN) ?
	    cl->set_state(lit) : cl->update_state();
	update_state (cl, c_state, n_state);

	_lit_number++;
	return lit;
    }
    inline void del_literal (LiteralPtr lit) {
	SAT_VariablePtr var = (SAT_VariablePtr) lit->variable();
	SAT_ClausePtr cl = (SAT_ClausePtr) lit->clause();

	int c_state = cl->state();

	var->del_literal (lit->variable_ref());
	cl->del_literal (lit->clause_ref());
	cl->unset_bounds (lit);

	int n_state = (var->value() != UNKNOWN) ? 
	    cl->unset_state(lit) : cl->update_state();
	update_state (cl, c_state, n_state);

	_lit_number--;
	CHECK(cl->check_consistency(););
	delete lit;
    }

    //-------------------------------------------------------------------------
    // **B** Functions used for conducting the search, by keeping internal
    //       information updated.
    //-------------------------------------------------------------------------

    virtual void init();               // Prepare DB for search, ie alloc space
    virtual void clear();              // Prepare variables for instance of SAT
    virtual void reset();                 // Search-independent reset procedure

    virtual int solution_found()
	{ return (_clauses.size() == _satisfied_clauses); }

    inline int &DLevel() { return _DLevel; }        // Decision Level of search
    inline int MLevel() { return _decision.size()-1; }            // Max DLevel

    virtual int push_decision();                  // Enter a new decision level
    virtual int pop_decision (int level);              // Free available DLevel
    virtual int active_decisions()
	{ return _decision.size()-_avail_decisions.size(); }

    // Elected assignment. DLevel used is top decision level.

    inline void set_assignment (SAT_VariablePtr var, int nval) {
	DBG1(cout<<"CONSTRAINING "<<var->name()<<" = "<<nval<<endl;);
	CHECK(if (var->value() != UNKNOWN)
	      Warn("Assigning variable already assigned?"););
	var->set_state (nval, _DLevel, NULL);
	_impliedVariables.append (var->assign_ref());
    }

    //-------------------------------------------------------------------------
    // Functions for statistics gathering and accessing, for dumping data, and
    // for consistency checking.
    //-------------------------------------------------------------------------

    virtual void output_stats();
    virtual void output_assignments (ostream &outs, int assigned_only = TRUE);

    virtual int resources_exceeded (SAT_ClauseDatabase &clDB);

    virtual void dump (ostream &outs = cout) { dump (FALSE, outs); }
    virtual void dump (int incremental, ostream &outs = cout);

    virtual int can_trim_solution (int &total_dec, int &required_dec);

    virtual int check_solution();
    virtual int check_consistency (int consistent_state = TRUE);
    virtual int check_final_consistency ();

    //-------------------------------------------------------------------------
    // Functions for updating the assignment state of variables. A variable
    // can have an implied assignment, can be assigned or can be required to
    // become unassigned.
    //-------------------------------------------------------------------------

    inline void set_implied_variable (SAT_VariablePtr var,
				      int nval, int nDLevel,
				      SAT_ClausePtr antec) {
	CHECK(if (var->value() != UNKNOWN)
	      Warn("Implying variable already assigned?"););
	var->set_state (nval, nDLevel, antec);
	_impliedVariables.append (var->assign_ref());
    }
    inline void set_assigned_variable (SAT_VariablePtr var) {
	_decision[var->DLevel()]->assignedVariables()
	    .append (var->assign_ref());
    }
    inline void unset_assigned_variable (SAT_VariablePtr var) {
	_decision[var->DLevel()]->assignedVariables()
	    .extract (var->assign_ref());
    }

    inline void set_unassigned_variable (SAT_VariablePtr var) {
	_unassignedVariables.append (var);
    }

    //-------------------------------------------------------------------------
    // Accessing assigned variables at a given decision level.
    //-------------------------------------------------------------------------

    inline int exists_assigned_variable (int level) {
	return _decision[level]->assignedVariables().size();
    }
    inline SAT_VariablePtr first_assigned_variable (int level) {
	return get_var (_decision[level]->assignedVariables(),
			_decision[level]->assignedVariables().first());
    }
    inline SAT_VariablePtr last_assigned_variable (int level) {
	return get_var (_decision[level]->assignedVariables(),
			_decision[level]->assignedVariables().last());
    }

    //-------------------------------------------------------------------------
    // Accessing variables with implied assignments not yet assigned.
    //-------------------------------------------------------------------------

    inline int exists_implied_variable () {
	return _impliedVariables.size();
    }
    inline SAT_VariablePtr first_implied_variable () {
	return get_var (_impliedVariables, _impliedVariables.first());
    }
    inline SAT_VariablePtr last_implied_variable () {
	return get_var (_impliedVariables, _impliedVariables.last());
    }

    //-------------------------------------------------------------------------
    // Accessing variables required to become unassigned.
    //-------------------------------------------------------------------------

    inline int exists_unassigned_variable () {
	return _unassignedVariables.size();
    }
    inline SAT_VariablePtr first_unassigned_variable () {
	return get_var (_unassignedVariables,
			_unassignedVariables.first(), TRUE);
    }
    inline SAT_VariablePtr last_unassigned_variable () {
	return get_var (_unassignedVariables,
			_unassignedVariables.last(), TRUE);
    }
    //-------------------------------------------------------------------------
    // Accessing unit and unsatisfied clauses.
    //-------------------------------------------------------------------------

    inline int exists_unit_clause() {
	return _unitClauses.size();
    }
    inline SAT_ClausePtr next_unit_clause (SAT_ClausePtr cl) {
	return next_clause (_unitClauses, cl);
    }
    inline int exists_unsat_clause() {
	return _unsatClauses.size();
    }
    inline SAT_ClausePtr next_unsat_clause (SAT_ClausePtr cl) {
	return next_clause (_unsatClauses, cl);
    }

    //-------------------------------------------------------------------------
    // Setting and deleting tagged clauses.
    //-------------------------------------------------------------------------

    inline void set_tagged_clause (SAT_ClausePtr cl) {
	CHECK(if (cl->test_tag (GRAY))
	      Warn("Tagging an already GRAY clause??"););
	_taggedClauses.append (cl);
    }
    inline void del_tagged_clauses (int tag_ref) {
	SAT_ClauseListPtr pcl;
	while (pcl = _taggedClauses.first()) {
	    SAT_ClausePtr cl = pcl->data();
	    CHECK(if(cl->implied_var())
		  {cl->dump(cout);cout<<endl;
		   Warn("Deleting GRAY clause with implied var??");}
		  if(cl->state()!=UNRESOLVED)
		  {cl->dump(cout);cout<<endl;
		   Warn("Clause state is NOT unresolved??");});
	    DBG0(cout<<"DELETING LARGE ";cl->dump(cout);cout<<endl;);
	    _taggedClauses.remove (pcl);

	    cl->unset_tag (tag_ref);
	    del_clause (cl);
	}
	DBG1(if (_mode[_DEBUG_]) check_consistency (TRUE););
    }

    //-------------------------------------------------------------------------
    // Function for updating the state of a clause.
    //-------------------------------------------------------------------------

    inline int update_state (SAT_ClausePtr cl, int c_state, int n_state) {
	if (c_state != n_state) {
	    if (c_state == UNSATISFIED) {
		_unsatClauses.extract (cl->consist_ref());
	    }
	    else if (c_state == UNIT) {
		_unitClauses.extract (cl->consist_ref());
	    }
	    else if (c_state == SATISFIED) {
		_satisfied_clauses--;
	    }
	    if (n_state == UNSATISFIED) {
		_unsatClauses.append (cl->consist_ref());
	    }
	    else if (n_state == UNIT) {
		_unitClauses.append (cl->consist_ref());
	    }
	    else if (n_state == SATISFIED) {
		_satisfied_clauses++;
	    }
	}
	DBGn(cout<<"CSTATE: "<<c_state<<" -- NSTATE: "<<n_state<<endl;
	     if(!cl->test_valid())Warn("Invalid clause state");
	     cl->dump(cout);cout<<endl<<endl;);
	return n_state;
    }

  protected:

    //-------------------------------------------------------------------------
    // Functions for extracting variables from lists and visiting clauses in
    // lists.
    //-------------------------------------------------------------------------

    inline SAT_VariablePtr get_var (SAT_VariableList &var_list,
				    SAT_VariableListPtr pvar,
				    int delete_ref = FALSE) { 
	SAT_VariablePtr var = NULL;
	if (pvar) {
	    var = (SAT_VariablePtr) pvar->data();
	    if (delete_ref) { var_list.remove (pvar); }
	    else            { var_list.extract (pvar); }
	}
	CHECK(else if (var_list.size())
	      {Warn("Found list with no pointer access and non-null size?");});
	return var;
    }
    inline SAT_ClausePtr next_clause (SAT_ClauseList &cl_list,
				      SAT_ClausePtr cl) {
	SAT_ClauseListPtr pcl;
	SAT_ClausePtr ret_cl = NULL;
	pcl = (cl) ? cl->consist_ref()->next() : cl_list.first();
	if (pcl) {
	    ret_cl = (SAT_ClausePtr) pcl->data();
	}
	return ret_cl;
    }

    //-------------------------------------------------------------------------
    // Variable definitions.
    //-------------------------------------------------------------------------

    int _DLevel;                    // Decision level associated with decisions

    SAT_ClauseList _unitClauses;                        // List of unit clauses
    SAT_ClauseList _unsatClauses;                      // List of unsat clauses

    int _satisfied_clauses;

    Array<DecisionPtr> _decision;
    List<DecisionPtr> _avail_decisions;           // Decisions that can be used

    SAT_Mode &_mode;

    //-------------------------------------------------------------------------
    // List of variables that are required to become assigned.
    //-------------------------------------------------------------------------

    SAT_VariableList _impliedVariables;

    //-------------------------------------------------------------------------
    // List of variables that are required to become unassigned. List of
    // clauses that are tagged.
    //-------------------------------------------------------------------------

    SAT_VariableList _unassignedVariables;
    SAT_ClauseList _taggedClauses;                    // List of tagged clauses

  protected:                                   // Variables for stats gathering
    int _dec_number;
    int _max_tree_depth;
    int _initial_var_number;
    int _initial_cl_number;
    int _initial_lit_number;
    int _lit_number;

  private:

};

//-----------------------------------------------------------------------------
//
// Inlined member functions that required the complete declaration of another
// class.
//
//-----------------------------------------------------------------------------

inline int SAT_Variable::set_state (int val, int dlevel, SAT_ClausePtr ant) {
    _value = val;
    _DLevel = dlevel;
    if (_antec = ant) {
	_antec->implied_var() = this;      // Notify clause that it implies var
    }
}

inline int SAT_Variable::unset_state() {
    if (_antec) {
	_antec->implied_var() = NULL;           // Clause no longer implies var
    }
    _value = UNKNOWN; _DLevel = NONE; _antec = NULL;
}

#endif // __GRP_CDB__

/*****************************************************************************/
