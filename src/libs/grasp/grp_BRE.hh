//-----------------------------------------------------------------------------
// File: grp_BRE.hh
//
// Purpose: Implementation of the backward reasoning engine (BRE) for SAT.
//          This engine provides ability Boolean Conflict Analysis, and
//          for state resetting. In all cases the state of the search
//          is moved backwards (i.e. less assigned variables).
//
// Remarks: --
//
// History: 03/15/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef __GRP_BRE__
#define __GRP_BRE__

#include "grp_Mode.hh"
#include "grp_CDB.hh"


//-----------------------------------------------------------------------------
// Class: BRE
//
// Purpose: Implementation of BRE.
//-----------------------------------------------------------------------------

class BRE {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    BRE (SAT_Mode &nmode) : _mode(nmode) {
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
    virtual ~BRE() {}

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    virtual int BCA (SAT_ClauseDatabase &clDB);

    virtual void clear (SAT_ClauseDatabase &clDB); // Clear DB for SAT instance
    virtual void reset (SAT_ClauseDatabase &clDB);  // Clear DB for next search

    // Clear from levels H to L

    virtual void reset (SAT_ClauseDatabase &clDB, int l_level, int h_level);

    virtual int resources_exceeded (SAT_ClauseDatabase &clDB);

    virtual void output_stats (SAT_ClauseDatabase &clDB);

  protected:

    //-------------------------------------------------------------------------
    // Resetting the state of the clause database.
    //-------------------------------------------------------------------------

    virtual void unset_state (SAT_ClauseDatabase &clDB, int level);

    virtual void unset_clauses_state (SAT_ClauseDatabase &clDB,
				      SAT_VariablePtr var);
    virtual void unset_state_w_conflict (SAT_ClauseDatabase &clDB, int level);

    //-------------------------------------------------------------------------
    // Inlined functions for the sole purpose of facilitating readability of
    // the code.
    // **A - functions for variable state unsetting.
    //-------------------------------------------------------------------------

    inline void unset_implied_variables (SAT_ClauseDatabase &clDB) {
	register SAT_VariablePtr var;
	while (var = clDB.last_implied_variable()) {
	    DBG0(var->dump(cout);cout<<endl;);
	    CHECK(if(var->test_tag(BROWN))
		  Warn("Implied variable with BROWN tag??"););
	    var->unset_state();
	}
    }
    inline void unset_unassigned_variables (SAT_ClauseDatabase &clDB) {
	register SAT_VariablePtr var;
	while (var = clDB.first_unassigned_variable()) {
	    DBG0(var->dump(cout);cout<<endl;);
	    unset_clauses_state (clDB, var);
	    clDB.unset_assigned_variable (var);
	    var->unset_state();
	    var->unset_tag (GRAY);               // Tag of unassigned variables
	}
    }

    //-------------------------------------------------------------------------
    // **B - functions used in conflict analysis.
    //-------------------------------------------------------------------------

    inline LiteralPtr add_clause_literal (SAT_ClauseDatabase &clDB,
					  SAT_VariablePtr var, int sign) {
	LiteralPtr lit = NULL;
	if (!var->test_tag (TAN)) {
	    var->set_tag (TAN);
	    lit = clDB.add_literal (_conf_clause, var, sign);
	}
	DBG0(cout << "ADDING literal ";
	     if(sign)cout<<"-";cout << var->name()<<endl;
	     _conf_clause->check_consistency();cout<<endl;);
	return lit;
    }
    inline LiteralPtr del_clause_literal (SAT_ClauseDatabase &clDB,
					  LiteralPtr lit) {
	DBG0(cout << "DELETING literal ";
	     if(lit->sign())cout<<"-";cout << lit->variable()->name()<<endl;);
	((SAT_VariablePtr)lit->variable())->unset_tag (TAN);
	clDB.del_literal (lit);
    }
    inline void del_conf_literals (SAT_ClauseDatabase &clDB) {
	LiteralListPtr plit;
	while (plit = _conf_clause->literals().first()) {
	    LiteralPtr lit = plit->data();
	    SAT_VariablePtr var = (SAT_VariablePtr) lit->variable();
	    var->unset_tag (TAN);
	    clDB.del_literal (lit);
	}
    }
    inline void del_conf_clause (SAT_ClauseDatabase &clDB) {
	if (_conf_clause) {
	    del_conf_literals (clDB);
	    clDB.del_clause (_conf_clause);
	}
    }
    inline void untag_conf_clause_vars() {
	for_each(plit,_conf_clause->literals(),LiteralPtr) {
	    SAT_VariablePtr var = (SAT_VariablePtr) plit->data()->variable();
	    var->unset_tag (TAN);
	}
	if (_conf_clause->size() > _large_cl_size) {
	    _large_cl_size = _conf_clause->size();
	}
	else if (_conf_clause->size() < _small_cl_size || _small_cl_size==0) {
	    _small_cl_size = _conf_clause->size();
	}
	_tot_conf_cl_number++;
	CHECK(if(_mode[_DEBUG_])
	      {cout << "CONFLICTING ";_conf_clause->dump(cout);cout<<endl;});
    }
    inline void tag_clause_vars (SAT_ClauseDatabase &clDB,
				 SAT_VariablePtr nvar,
				 SAT_ClausePtr cl, int level) {
	for_each(plit,cl->literals(),LiteralPtr) {
	    LiteralPtr lit = plit->data();
	    SAT_VariablePtr var = (SAT_VariablePtr) lit->variable();
	    if (var->DLevel() == level) {
		if (var != nvar && !var->test_tag (BROWN)) {
		    var->set_tag (BROWN);
		    _tagged_vars++;
		}
	    }
	    else {
		CHECK(if (var->DLevel() > level)
		      {Warn("Too high DLevel in antecedent??");
		       cout<<"  Clause ";cl->dump(cout);cout<<endl;
		       cout<<"    Problem ";var->dump(cout);cout<<endl;});
		add_clause_literal (clDB, var, var->value());
	    }
	}
    }

    //-------------------------------------------------------------------------
    // Statistics gathering.
    //-------------------------------------------------------------------------

    inline void output_conf_stats (SAT_ClauseDatabase &clDB, int minDLevel) {
	int act_dec = clDB.active_decisions();              // Active decisions
	int conf_diff = act_dec - minDLevel;           // Size of tree and conf
	cout<<"CURRENT DLEVELs:  "<<act_dec<<endl;
	if (conf_diff > _large_conf_diff) {
	    _large_conf_diff = conf_diff;
	}
	if (_mode[_BACKTRACKING_STRATEGY_] == _DYNAMIC_B_) {
	    cout<<"CONFLICT LEVEL: "<<minDLevel<<endl;
	} else if (minDLevel != act_dec) {
	    _tot_back_number++;
	    cout<<"BACKTRACK LEVEL:  "<<minDLevel;
	    if (minDLevel < act_dec-1) {
		if (conf_diff > _large_skip) {
		    _large_skip = conf_diff;
		}
		cout << "    *"; _tot_nonback_number++;
	    }
	    cout << endl;
	} else if (_mode[_DEBUG_]) {
	    cout<<"DEFINING ASSERTION @ CURRENT DLEVEL: ";
	    cout<<act_dec<<endl;
	}
	CHECK(clDB.check_consistency (FALSE);
	      if (minDLevel && clDB.exists_unsat_clause())
	      {Warn("Still unsatisfied clauses in DB after BCA??");}
	      if (minDLevel && !clDB.exists_unit_clause())
	      {Warn("NO unit clauses in DB after BCA??");});
    }

    //-------------------------------------------------------------------------
    // Data structures used.
    //-------------------------------------------------------------------------

    int _CLevel;

    SAT_ClausePtr _conf_clause;
    int _skip_UIP;
    int _tagged_vars;

    SAT_Mode &_mode;

  protected:                                   // Variables for stats gathering
    int _tot_conf_number;
    int _large_conf_diff;
    int _tot_back_number;
    int _tot_nonback_number;
    int _large_skip;
    int _tot_conf_cl_number;
    int _large_cl_size;
    int _small_cl_size;
    int _tot_uip_number;
    int _max_uip_number;

  private:

};


#endif // __GRP_BRE__

/*****************************************************************************/
