//-----------------------------------------------------------------------------
// File: grp_Decide.hh
//
// Purpose: Class declaration of a decision engine, that elects decision
//          assignments in search for SAT.
//
// Remarks: --
//
// History: 6/23/95 - JPMS - created.
//          3/10/96 - JPMS - adapted for new version of GRASP.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef __GRP_DECIDE__
#define __GRP_DECIDE__

#include "list.hh"
#include "grp_Mode.hh"
#include "grp_CDB.hh"


//-----------------------------------------------------------------------------
// Class names and typedefs used for simplying interface with CNF formulas.
//-----------------------------------------------------------------------------

class sortType;
class VarOrdering;

typedef VarOrdering *VarOrderingPtr;
typedef List<VarOrdering*> VarOrderingList;
typedef ListItem<VarOrdering*> *VarOrderingListPtr;



//-----------------------------------------------------------------------------
// Class: DecisionEngine
//
// Purpose: Elects decision assignments during the search for a solution of
//          an instance of SAT.
//-----------------------------------------------------------------------------

class DecisionEngine {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    DecisionEngine (SAT_Mode &nmode) : _mode(nmode), _sdec_sched(0) {
	_max_sched = NONE; _dec_ptr = 0;
    }
    virtual ~DecisionEngine() { _sdec_sched.resize(0); }

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    virtual void init (SAT_ClauseDatabase &clauseDB);
    virtual void clear (SAT_ClauseDatabase &clauseDB);
    virtual void reset (SAT_ClauseDatabase &clauseDB);

    virtual void backtrack (SAT_ClauseDatabase &clDB, int back_level);

    inline int decide (SAT_ClauseDatabase &clDB) {
	return ((_mode[_DECISION_MODE_] == _STATIC_ORD_) ?
		static_assignment_select (clDB) :
		dynamic_assignment_select (clDB));
    }

    //-------------------------------------------------------------------------
    // Decision making preparation -> for static decision making.
    //-------------------------------------------------------------------------

    virtual void set_decision_order (SAT_VariablePtr var, int order = ANY);
    virtual void set_preferred_value (SAT_VariablePtr var, int value = NONE);

    virtual void reorder_variables (SAT_ClauseDatabase &clDB);

    //-------------------------------------------------------------------------
    // Function for accessing stats of running the SAT algorithm.
    //-------------------------------------------------------------------------

    virtual void output_stats (SAT_ClauseDatabase &clauseDB);

  protected:

    //-------------------------------------------------------------------------
    // Internal member functions for decision making.
    //-------------------------------------------------------------------------

    virtual int static_assignment_select (SAT_ClauseDatabase &clDB);
    virtual int dynamic_assignment_select (SAT_ClauseDatabase &clDB);

    inline void elect_assignment (SAT_ClauseDatabase &clDB,
				  SAT_VariablePtr var, int value) {
	clDB.push_decision();
	clDB.set_assignment (var, value);
	if (_mode[_DEBUG_]) {
	    cout << "DECISION: " << var->name() <<" = " << value << " @ ";
	    cout << clDB.DLevel() << endl;
	}
    }

    //-------------------------------------------------------------------------
    // Functions for static variable ordering.
    //-------------------------------------------------------------------------


    //-------------------------------------------------------------------------
    // Functions for dynamic variable ordering.
    //-------------------------------------------------------------------------

    int select_MSOS (SAT_ClauseDatabase &clDB);
    int select_MSTS (SAT_ClauseDatabase &clDB);
    int select_MSMM (SAT_ClauseDatabase &clDB);
    int select_BOHM (SAT_ClauseDatabase &clDB);
    int select_DLIS (SAT_ClauseDatabase &clDB);
    int select_DLCS (SAT_ClauseDatabase &clDB);

    //-------------------------------------------------------------------------
    // Internal variable definition.
    //-------------------------------------------------------------------------

    SAT_Mode &_mode;                                   // Configuration options

    Array<SAT_VariablePtr> _sdec_sched;
    Array<VarOrderingPtr> _sdec_order;
    int _max_sched;
    int _dec_ptr;

  private:

};


//-----------------------------------------------------------------------------
// Class: VarOrdering
//
// Purpose: Class declaration for the ordering of a variable.
//-----------------------------------------------------------------------------

class VarOrdering {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    VarOrdering (SAT_VariablePtr Nvar) {
	_var = Nvar; _pref_value = TRUE; _order = Nvar->ID();
    }
    ~VarOrdering() {}

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------
    
    SAT_VariablePtr variable() { return _var; }
    
    inline int &pref_value() { return _pref_value; }
    
    inline int &order() { return _order; }

  protected:

    //-------------------------------------------------------------------------
    // Contains ordering information for each variable. _ordFig measures
    // how "useful" it is to assign on this variable. _rank indicates how
    // does this variable compara against others.
    //-------------------------------------------------------------------------

    SAT_VariablePtr _var;                            // Pointer to the variable
    int _pref_value;                   // Preferred value to assign to variable
    int _order;                         // Figure by which variables are sorted

  private:

};

#endif // __GRP_DECIDE__

/*****************************************************************************/
