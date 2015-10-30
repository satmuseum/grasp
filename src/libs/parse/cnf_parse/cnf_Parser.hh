//-----------------------------------------------------------------------------
// File: cnf_Parser.hh
//
// Purpose: Class declaration of a parser for SAT instances in cnf format. 
//
// Remarks: --
//
// History: 6/21/95 - JPMS - created.
//          3/16/96 - JPMS - repackaged for new version of GRASP.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include "defs.hh"
#include "array.hh"
#include "grp_SAT.hh"        // Only requires ability to create clause database


class CNF_Parser {

public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    CNF_Parser() : variables(0) {}
    virtual ~CNF_Parser() {
	variables.resize(0);
    }

    //-------------------------------------------------------------------------
    // Interface contract: create clause DB from a file in CNF format.
    //-------------------------------------------------------------------------

    int load_CDB (char *name, SAT_ClauseDatabase &clDB);

private:
    Array<SAT_VariablePtr> variables;     // Array for ID to pointer conversion

};

/*****************************************************************************/

