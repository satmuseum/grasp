//-----------------------------------------------------------------------------
// File: grp_BRE_P.hh
//
// Purpose: Implementation of backward reasnoning when the clause database
//          can only grow polynomially.
//
// Remarks: --
//
// History: 03/14/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef __GRP_BRE_P__
#define __GRP_BRE_P__

#include "grp_BRE.hh"


//-----------------------------------------------------------------------------
// Class: BRE_P
//
// Purpose: Polynomial conflict analysis procedure.
//
// Notes:   Clauses that are created of size greater than maximum
//          allowed are tagged BLACK. BLACK tagged clauses are deleted
//          whenever possible.
//-----------------------------------------------------------------------------

class BRE_P : public BRE {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    BRE_P(SAT_Mode &nmode) : BRE(nmode) {
	_tot_discard_cl_number = 0;
    }
    virtual ~BRE_P() {}

    //-------------------------------------------------------------------------
    // Interface contract -> defined in inherited class
    //-------------------------------------------------------------------------

    virtual void clear (SAT_ClauseDatabase &clDB); // Clear DB for SAT instance

    virtual void output_stats (SAT_ClauseDatabase &clDB);

  protected:

    //-------------------------------------------------------------------------
    // Functions for analyzing the causes of conflicts. Front-end function
    // is BCA(), that returns the highest conflicting decision level.
    //-------------------------------------------------------------------------

    virtual void unset_clauses_state (SAT_ClauseDatabase &clDB,
				      SAT_VariablePtr var);
    virtual void unset_state_w_conflict (SAT_ClauseDatabase &clDB, int level);

    virtual void unset_state (SAT_ClauseDatabase &clDB, int level);

    virtual void conf_clause_repack (SAT_ClauseDatabase &clDB);

    inline void eval_n_tag_clause_size (SAT_ClauseDatabase &clDB) {
	conf_clause_repack (clDB);

	if (_conf_clause->size() > _mode[_CONF_CLAUSE_SIZE_]) {
	    _conf_clause->set_tag (BLACK);
	    CHECK(if(_mode[_DEBUG_])
		  {if(_conf_clause->size() > clDB.active_decisions())
		       Info("Decision-directed clause is smaller??");});
	    DBG0(cout<<"CREATED TEMP LARGE CLAUSE"<<endl;);
	}
    }

  protected:                                   // Variables for stats gathering
    int _tot_discard_cl_number;

  private:

};

#endif // __GRP_BRE_P__

/*****************************************************************************/
