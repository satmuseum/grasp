//-----------------------------------------------------------------------------
// File: grp_CNF.cc
//
// Purpose: Definition of some functions declared in module CNG.hh. Currently,
//          mostly used for memory management definitions.
//
// Remarks: --
//
// History: 03/15/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include "grp_CNF.hh"


//-----------------------------------------------------------------------------
// Macro expansions for memory management of Variables, Literals and Clauses.
//-----------------------------------------------------------------------------


MEM_MNG_DEF(Variable, "Variable")

MEM_MNG_DEF(Literal, "Literal")

MEM_MNG_DEF(Clause, "Clause")



//-----------------------------------------------------------------------------
// Function: dump()
//
// Purpose: 
//
//-----------------------------------------------------------------------------

void ClauseDatabase::dump (ostream &outs)
{
    outs << "\nDumping clause database\n";
    outs << "  Number of Variables: " << _variables.size() << endl;
    outs << "  Number of Clauses: " << _clauses.size() << endl;
    outs << "    Clause Database: " << endl;
    for_each(pcl,_clauses,ClausePtr) {
	outs << "      ";
	ClausePtr cl = pcl->data();
	for_each(plit,cl->literals(),LiteralPtr) {
	    LiteralPtr lit = plit->data();
	    if (lit->sign()) {
		outs << "-";
	    }
	    outs << lit->variable()->name() << " ";
	}
	outs << endl;
    }
    outs << "    End of Clause Database\n" << endl;
}

/*****************************************************************************/
