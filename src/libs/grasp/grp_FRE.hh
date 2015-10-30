//-----------------------------------------------------------------------------
// File: grp_FRE.hh
//
// Purpose: Implementation of the core forward reasoning engine (FRE) for SAT.
//          For SAT it just implements Boolean Constraint Propagation (BCP).
//
// Remarks: --
//
// History: 03/15/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef __GRP_FRE__
#define __GRP_FRE__

#include "grp_Mode.hh"
#include "grp_CDB.hh"


//-----------------------------------------------------------------------------
// Class: FRE
//
// Purpose: Implementation of Boolean Constraint Propagation.
//-----------------------------------------------------------------------------

class FRE {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    FRE (SAT_Mode &nmode) : _mode(nmode) {}
    virtual ~FRE() {}

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    virtual int BCP (SAT_ClauseDatabase &clDB);

  protected:

    virtual int set_clauses_state (SAT_ClauseDatabase &clDB,
				   SAT_VariablePtr var);
    virtual int imply_variable_assignment (SAT_ClauseDatabase &clDB,
					   SAT_ClausePtr cl);

    SAT_Mode &_mode;

  private:

};


#endif // __GRP_FRE__

/*****************************************************************************/
