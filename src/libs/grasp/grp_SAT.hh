//-----------------------------------------------------------------------------
// File: grp_SAT.hh
//
// Purpose: Front-end class for the GRASP algorithmic framework for SAT.
//
// Remarks: --
//
// History: 03/10/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef __GRP_SAT__
#define __GRP_SAT__

#include <iostream.h>

#include "time.hh"
#include "grp_Mode.hh"
#include "grp_CDB.hh"
#include "grp_BRE.hh"
#include "grp_BRE_P.hh"
#include "grp_FRE.hh"
#include "grp_Preproc.hh"
#include "grp_Deduce.hh"
#include "grp_Diagnose.hh"
#include "grp_Decide.hh"



//-----------------------------------------------------------------------------
// Defines for GRASP_SAT interfacing.
//-----------------------------------------------------------------------------

enum SearchOutcomes { SATISFIABLE = 0x30, UNSATISFIABLE, ABORTED };



//-----------------------------------------------------------------------------
// Class: GRASP_SAT
//
// Purpose: This class is the front-end that must be considered for solving
//          instances of SAT. Even though different algorithms can be used,
//          it is up to this class to uniformize configurations.
//-----------------------------------------------------------------------------

class GRASP_SAT {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    GRASP_SAT (SAT_Mode &nmode) : _mode(nmode), _time() {
	_preprocEng = NULL;
	_deduceEng = NULL;
	_diagnoseEng = NULL;
	_decideEng = NULL;
    }
    virtual ~GRASP_SAT() {
	delete _BRE;
	delete _FRE;
	if (_preprocEng) { delete _preprocEng; }
	delete _deduceEng;
	delete _diagnoseEng;
	delete _decideEng;
    }

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    virtual void setup (SAT_ClauseDatabase &clDB);  // Setup engines to be used

    virtual void init (SAT_ClauseDatabase &clDB);
    virtual void reset (SAT_ClauseDatabase &clDB);
    virtual void clear (SAT_ClauseDatabase &clDB);

    virtual int solve (SAT_ClauseDatabase &clDB);          // Solve CNF formula

    //-------------------------------------------------------------------------
    // Access key data structures and engines used by the search algorithm.
    //-------------------------------------------------------------------------

    inline PreprocessEngine &preprocess_engine() { return *_preprocEng; }
    inline DeductionEngine &deduction_engine() { return *_deduceEng; }
    inline DiagnosisEngine &diagnosis_engine() { return *_diagnoseEng; }
    inline DecisionEngine &decision_engine() { return *_decideEng; }

    //-------------------------------------------------------------------------
    // Functions for mode setting and for accessing stats of running the SAT
    // algorithm.
    //-------------------------------------------------------------------------

    virtual void output_stats (SAT_ClauseDatabase &clDB);

    virtual void output_assignments (SAT_ClauseDatabase &clDB,
				     ostream &outs, int assigned_only) {
	clDB.output_assignments (outs, assigned_only);
    }

  protected:

    //-------------------------------------------------------------------------
    // Functions used for implementing the search process and for evaluating
    // used resources.
    //-------------------------------------------------------------------------

    virtual int backtrack (SAT_ClauseDatabase &clDB, int back_level);

    virtual int resources_exceeded (SAT_ClauseDatabase &clDB);

    //-------------------------------------------------------------------------
    // Data structures used by the search process.
    //-------------------------------------------------------------------------

    BRE *_BRE;     // Backward reasoning engine
    FRE *_FRE;     // Forward reasoning engine

    PreprocessEngine *_preprocEng;
    DeductionEngine *_deduceEng;
    DiagnosisEngine *_diagnoseEng;
    DecisionEngine *_decideEng;

    SAT_Mode &_mode;

    Timer _time;

  private:

};

#endif // __GRP_SAT__

/*****************************************************************************/
