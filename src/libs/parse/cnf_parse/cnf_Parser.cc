//-----------------------------------------------------------------------------
// File: cnf_Parser.cc
//
// Purpose: Realization of a parter that reads a SAT instance from a file
//          in CNF format and builds the initial clause database.
//
// Remarks: --
//
// History: 6/21/95 - JPMS - created.
//          3/16/96 - JPMS - repackaged for new version of GRASP.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream.h>

#include "cnf_Parser.hh"


// ----------------------------------------------------------------------------
// Function: parse
//
// Purpose: Given a file specification, in CNF format, it builds the
//          internal clause database.
//
// Side-effects: The database is built.
//
// Notes: ---
// ----------------------------------------------------------------------------

# define MAX_LINE_LENGTH    256
# define MAX_WORD_LENGTH    32

int CNF_Parser::load_CDB (char *fname, SAT_ClauseDatabase &clDB)
{
    DBGn(cout<<"Entering CNF_parser::parse()"<<endl;
	 cout<<"File name: "<<fname<<endl;);
    char line_buffer[MAX_LINE_LENGTH];
    char word_buffer[MAX_WORD_LENGTH];
    SAT_ClausePtr clause = NULL;

    ifstream inp (fname, ios::in|ios::nocreate);
    if (!inp) {
	return FALSE;
    }
    while (inp.getline(line_buffer, MAX_LINE_LENGTH)) {
	if (line_buffer[0] == 'c') {
	    continue;
	}
	else if (line_buffer[0] == 'p') {
	    int var_num;
	    int cl_num;

	    int arg = sscanf (line_buffer, "p cnf %d %d", &var_num, &cl_num);
	    if( arg < 2 ) {
		Abort("Unable to read number of variables and clauses");
	    }
	    DBGn(cout<<"total vars: "<<var_num<<endl;);
	    variables.resize (var_num);
	    for (int k = 0; k < var_num; k++) {
		sprintf( word_buffer, "%d%c", k+1, '\0' );
		int s_size = strlen( word_buffer );
		char *name = new char[s_size+1] ;
		strcpy( name, word_buffer );

		DBGn(cout<<"defining var: "<<name<<" for index "<<k<<endl;);
		variables[k] = clDB.add_variable (name);
	    }
	}
	else {                             // Clause definition or continuation
	    char *lp = line_buffer;
	    do {
		char *wp = word_buffer;
		while (*lp && ((*lp == ' ') || (*lp == '\t'))) {
		    lp++;
		}
		while (*lp && (*lp != ' ') && (*lp != '\t') && (*lp != '\n')) {
		    *(wp++) = *(lp++);
		}
		*wp = '\0';                                 // terminate string

		if (strlen(word_buffer) != 0) {     // check if number is there
		    if( clause == NULL ) {
			DBGn(cout<<"creating new clause\n";);
			clause = clDB.add_clause();
		    }
		    int vID = atoi (word_buffer);
		    int sign = FALSE;

		    if( vID != 0) {
			if( vID < 0)  { vID = -vID; sign = TRUE; }

			DBGn(cout<<"adding "<<vID<<" to clause w/ sign ";
			     cout<<sign<<"\n";);
			clDB.add_literal( clause, variables[vID-1], sign );
		    } else {
			if(clause->size() <= 0) {
			    clDB.del_clause (clause);       // size zero clause
			}
			clause = NULL;
		    }
		}
	    }
	    while (*lp);
	}
    }
    return TRUE;
}

/*****************************************************************************/

