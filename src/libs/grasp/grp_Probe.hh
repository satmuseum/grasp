//-----------------------------------------------------------------------------
// File: grp_Probe.hh
//
// Purpose: Class declaration of an engine that implements probing on a set
//          of K variables, i.e. applies BCP all combinations of assignments
//          of K given variables.
//
// Remarks: Additional features include:
//            - If a variable assumes the same value for all 2^K
//              assignments, then the value of that variable can be fixed.
//
// History: 07/13/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef __GRP_PROBE__
#define __GRP_PROBE__

#include "defs.hh"
#include "memory.hh"
#include "list.hh"

#include "grp_Mode.hh"
#include "grp_CNF.hh"
#include "grp_CDB.hh"
#include "grp_Deduce.hh"
#include "grp_Diagnose.hh"


//-----------------------------------------------------------------------------
// Class names and typedefs used for simplying interface with probe entries.
//-----------------------------------------------------------------------------

typedef ProbeEntry *ProbeEntryPtr;
typedef List<ProbeEntry*> ProbeEntryList;
typedef ListItem<ProbeEntry*> *ProbeEntryListPtr;



//-----------------------------------------------------------------------------
// Class: ProbeEntry
//
// Purpose: Describes the current status of an entry used during probing.
//-----------------------------------------------------------------------------

class ProbeEntry {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    ProbeEntry (SAT_VariablePtr nvar, int ndef_val) {
	_var = nvar; _def_val = ndef_val; _toggled = FALSE;
    }
    virtual ~ProbeEntry();

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    SAT_VariablePtr var() { return _var; }
    int def_val() { return _def_val; }
    int toggled() { return _toggled; }

  protected:

    SAT_VariablePtr _var;
    int _def_val;
    int _toggled;

  private:

};


//-----------------------------------------------------------------------------
// Class: ProbeEngine
//
// Purpose: Class for implementing K-depth probing.
//-----------------------------------------------------------------------------

class ProbeEngine {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    ProbeEngine (SAT_ClauseDatabase &_clDB, DeductionEngine &_deductionEng,
		 DiagnosisEngine &_diagnosisEng) :
    clDB (_clDB), deductionEng (_deductionEng), diagnosisEng (_diagnosisEng),
    probe_entries () {}

    virtual ~ProbeEngine() {}

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    int apply (SAT_VariableList &var_list);

  protected:

    int become_consistent();

    //-------------------------------------------------------------------------
    // Clause database and Engines references.
    //-------------------------------------------------------------------------

    SAT_ClauseDatabase &clDB;
    DeductionEngine &deductionEng;
    DiagnosisEngine &diagnosisEng;

    //-------------------------------------------------------------------------
    // Internal data structures.
    //-------------------------------------------------------------------------

    ProbeEntryList probe_entries;

  private:

};

#endif // __GRP_PROBE__

/*****************************************************************************/
