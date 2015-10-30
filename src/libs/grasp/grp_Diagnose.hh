//-----------------------------------------------------------------------------
// File: grp_Diagnose.hh
//
// Purpose: Class declaration of a diagnosis engine, that establishes the
//          causes of a conflict. Only simple form of a diagnosis engine
//          is currently implemented. See dissertation for other more
//          elaborated diagnosis procedures.
//
// Remarks: --
//
// History: 03/10/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef __GRP_DIAGNOSE__
#define __GRP_DIAGNOSE__

#include "grp_Mode.hh"
#include "grp_CDB.hh"
#include "grp_FRE.hh"
#include "grp_BRE.hh"


//-----------------------------------------------------------------------------
// Class: DiagnosisEngine
//
// Purpose: Given an inconsistent clause database, the diagnosis engine
//          identifies the causes of a conflict.
//-----------------------------------------------------------------------------

class DiagnosisEngine {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    DiagnosisEngine (SAT_Mode &nmode, FRE &nFRE, BRE &nBRE) :
    _mode(nmode), _FRE(nFRE), _BRE(nBRE) {}
    virtual ~DiagnosisEngine() {}

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    void init (SAT_ClauseDatabase &clauseDB) {}
    void clear (SAT_ClauseDatabase &clauseDB) {}
    void reset (SAT_ClauseDatabase &clauseDB) {}

    int diagnose (SAT_ClauseDatabase &clauseDB) { return _BRE.BCA (clauseDB); }

    void output_stats (SAT_ClauseDatabase &clauseDB) {}

  protected:

    FRE &_FRE;                 // Implementor of Boolean Constraint Propagation
    BRE &_BRE;                      // Implementor of Boolean Conflict Analysis
    SAT_Mode &_mode;                                   // Configuration options

  private:

};

#endif // __GRP_DIAGNOSE__

/*****************************************************************************/
