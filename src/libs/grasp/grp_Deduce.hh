//-----------------------------------------------------------------------------
// File: grp_Deduce.hh
//
// Purpose: Class declaration of a deduction engine, which identifies
//          necessary assignments on a clause database. A deduction engine
//          assumes always the existence of a BCP object. Only simplest
//          form of a deduction engine (level 0) is currently implemented.
//
// Remarks: --
//
// History: 03/10/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef __GRP_DEDUCE__
#define __GRP_DEDUCE__

#include "grp_Mode.hh"
#include "grp_CDB.hh"
#include "grp_FRE.hh"
#include "grp_BRE.hh"


//-----------------------------------------------------------------------------
// Class: DeductionEngine
//
// Purpose: Identifies necessary assignments given other assignments of
//          contraints in a clause database.
//-----------------------------------------------------------------------------

class DeductionEngine {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    DeductionEngine (SAT_Mode &nmode, FRE &nFRE, BRE &nBRE) :
    _mode(nmode), _FRE(nFRE), _BRE(nBRE) {}
    virtual ~DeductionEngine() {}

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    void init (SAT_ClauseDatabase &clauseDB) {}
    void clear (SAT_ClauseDatabase &clauseDB) {}
    void reset (SAT_ClauseDatabase &clauseDB) {}

    int deduce (SAT_ClauseDatabase &clauseDB) { return _FRE.BCP (clauseDB); }

    void output_stats (SAT_ClauseDatabase &clauseDB) {}

  protected:

    FRE &_FRE;                 // Implementor of Boolean Constraint Propagation
    BRE &_BRE;                      // Implementor of Boolean Conflict Analysis
    SAT_Mode &_mode;                                   // Configuration options

  private:

};

#endif // __GRP_DEDUCE__

/*****************************************************************************/
