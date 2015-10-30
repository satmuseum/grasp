//-----------------------------------------------------------------------------
// File: nsat.cc
//
// Purpose: Front-end for the GRASP SAT algorithmic framework. Instances of
//          CNF are specified in CNF format (from DIMACS).
//
// Remarks: Eventually interactive use will be allowed.
//
// History: 06/16/95 - JPMS - created.
//          03/17/96 - JPMS - repackaged for the new version of GRASP.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "time.hh"
#include "memory.hh"
#include "cnf_Parser.hh"
#include "grp_Cmds.hh"
#include "grp_SAT.hh"



//-----------------------------------------------------------------------------
// Function: main
//
// Purpose: Top-level actions of nsat.
//-----------------------------------------------------------------------------

main( int argc, char *argv[] )
{
    DBG1(cout<<"-> Entering ::main()."<<endl;);

    //-------------------------------------------------------------------------
    // Initilization of memory management -> depends on the compilation options
    //-------------------------------------------------------------------------

    MEM_MNG_INIT();

    //-------------------------------------------------------------------------
    // Next, definition of nsat.
    //-------------------------------------------------------------------------

    Timer time;
    SAT_Mode mode;                                    // Configuration of GRASP
    mode.setup();                                // Setup mode for future usage

    SAT_ClauseDatabase database (mode);               // Create clause database
    GRASP_SAT sat (mode);                                // Create a SAT solver
    SAT_Cmds scmd  (mode);
    CNF_Parser parser;

    char *fname;
    if( fname = scmd.cmdLineParse (argc, argv) ) {
	DBG1(cout<<"Filename: "<<fname<<endl;);
	if( !parser.load_CDB (fname, database) ) {
	    cout << "\n    File name: " << fname << endl;
	    Abort("Cannot open input file");
	}
	sat.setup (database);
	sat.init (database);
	printTime ("Done creating structures", time.elapsedTime(), "Elapsed");

	int status = sat.solve (database); 	          // Solve SAT instance

	if( status == SATISFIABLE ) {
	    printTime ("Done searching.... SATISFIABLE INSTANCE",
		       time.elapsedTime(), "Elapsed");
	    if (mode[_TRIM_SOLUTIONS_]) {
		int tot_dec, req_dec;
		if (database.can_trim_solution (tot_dec, req_dec)) {
		    cout << "\n\tSolution can be trimmed from ";
		    cout << tot_dec << " to " << req_dec << " decisions\n";
		    cout << endl;
		}
		else {
		    cout << "\n\t Solution cannot be trimmed\n" << endl;
		}
	    }
	    if (mode[_VERBOSE_]) {
		Info("Solution found");
		database.output_assignments (cout);
	    }
	} else if( status == UNSATISFIABLE ) {
	    printTime ("Done searching.... UNSATISFIABLE INSTANCE",
		       time.elapsedTime(), "Elapsed");
	    if (mode[_VERBOSE_]) {
		Info("NO solution found");
	    }
	} else {
	    printTime ("Done searching.... RESOURCES EXCEEDED",
		       time.elapsedTime(), "Elapsed");
	    if (mode[_VERBOSE_]) {
		Info("Resources exceeded... Aborted");
	    }
	}

	// Output relevant stats.

	sat.output_stats (database);
	sat.reset (database);
    } else {
	Abort( "Unable to enter on-line mode... Under construction" );
    }
    printTime ("Terminating NSAT", time.totalTime(), "Total");
    CHECK(database.check_final_consistency(););
    DBG1(cout<<"-> Exiting ::main()."<<endl;);
    exit(0);
}

/*****************************************************************************/

