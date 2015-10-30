//-----------------------------------------------------------------------------
// File: grp_Preproc.hh
//
// Purpose: Declaration of class PreprocessEngine, which identifies
//          necessary assignments prior to searching for a solution
//          In addition, clauses of size two are identified.
//
// Remarks: --
//
// History: 03/14/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef __GRP_PREPROC__
#define __GRP_PREPROC__

#include "grp_Mode.hh"
#include "grp_CDB.hh"
#include "grp_BRE.hh"
#include "grp_FRE.hh"


//-----------------------------------------------------------------------------
// Class: PreprocessEngine
//
// Purpose: The basic preprocessing engine permits two operation options:
//          preprocessing of level 0 (test the two assignments for every
//          variable) or preprocessing of level 0 with relaxation (apply
//          preeprocessing of level 0 while more clauses or necessary
//          assignments are identified).
//-----------------------------------------------------------------------------

class PreprocessEngine {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    PreprocessEngine (SAT_Mode &nmode, FRE &nFRE, BRE &nBRE) :
    _mode(nmode), _FRE(nFRE), _BRE(nBRE) {}
    virtual ~PreprocessEngine() {}

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    int init (SAT_ClauseDatabase &clauseDB) {}
    int clear (SAT_ClauseDatabase &clauseDB) {}
    int reset (SAT_ClauseDatabase &clauseDB) {}

    virtual int preprocess (SAT_ClauseDatabase &sat,
			    int with_relaxation = FALSE);

    int output_stats (SAT_ClauseDatabase &clauseDB) {}

  protected:

    FRE &_FRE;                 // Implementor of Boolean Constraint Propagation
    BRE &_BRE;                      // Implementor of Boolean Conflict Analysis
    SAT_Mode &_mode;                                   // Configuration options

  private:

};

#endif // __GRP_PREPROC__

/*****************************************************************************/
