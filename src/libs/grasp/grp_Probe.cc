//-----------------------------------------------------------------------------
// File: grp_Probe.cc
//
// Purpose: Definition of the member functions implementing K-depth probing.
//
// Remarks: --
//
// History: 07/13/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include "grp_Probe.hh"



//-----------------------------------------------------------------------------
// Function: apply()
//
// Purpose: Given a set of variables (size K) applies K-depth probing to the
//          variables in order to evaluate all combinations of assignments.
//
// Side-effects: The clause database can be completed with additional
//               implicates
//-----------------------------------------------------------------------------

int ProbeEngine::apply (SAT_VariableList &var_list)
{
    
    //-------------------------------------------------------------------------
    // Define a stack of variables and their assignments, that will specify
    // how probing evolves. Creation of stack also leads to the definition of
    // the first assignment.
    //-------------------------------------------------------------------------

    int consistent = TRUE;
    int cons_index = 0;
    SAT_VariableListPtr pvar;
    for (pvar = var_list.first(); pvar; pvar = pvar->next()) {
	SAT_VariablePtr var = pvar->next();
	ProbeEntryPtr pr_item = new ProbeEntry (var, FALSE);
	probe_entries.append (pr_item);

	// If still consistent: push decision level; define assignment;
	// apply BCP; update cons_index

	if (consistent) {
	    cons_index++;
	    clDB.push_decision();
	    clDB.set_assignment (var, FALSE);  // Default value is always FALSE
	    if (deductionEng.deduce (clDB) == CONFLICT) {
		consistent = FALSE;
	    }
	}
    }
    //-------------------------------------------------------------------------
    // Second phase, iterate assignments over all K variables.
    //-------------------------------------------------------------------------




}


//-----------------------------------------------------------------------------
// Function: become_consistent()
//
// Purpose: Given an inconsistent set of assignments, enters a consistent
//          state.
//
// Side-effects: Updates clause database with implicates learnt from
//               conflicts(s).
//-----------------------------------------------------------------------------

int ProbeEngine::become_consistent()
{
    diagnosisEng.diagnose();


}

/*****************************************************************************/
