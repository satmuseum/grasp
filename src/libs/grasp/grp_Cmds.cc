//-----------------------------------------------------------------------------
// File: grp_Cmds.cc
//
// Purpose: Sets up configuration options for running the GRASP SAT algorithm.
//
// Remarks: --
//
// History: 6/21/95 - JPMS - created.
//          3/16/96 - JPMS - repackaged for new version of GRASP.
//-----------------------------------------------------------------------------

#include "grp_Cmds.hh"


//-----------------------------------------------------------------------------
// Constructor/destructor.
//-----------------------------------------------------------------------------

SAT_Cmds::SAT_Cmds (SAT_Mode &nmode) : _mode (nmode) {}

SAT_Cmds::~SAT_Cmds() {}


//-----------------------------------------------------------------------------
// General purpose methods for setting configuration options.
//-----------------------------------------------------------------------------

void SAT_Cmds::setBacktrackMode (CmdArg &arg)
{
    if (arg.matches ("C")) {
	_mode[_BACKTRACKING_STRATEGY_] = _CHRONOLOGICAL_B_;
    } else if (arg.matches("N")) {
	_mode[_BACKTRACKING_STRATEGY_] = _NON_CHRONOLOGICAL_B_;
    } else if (arg.matches("D")) {
	_mode[_BACKTRACKING_STRATEGY_] = _DYNAMIC_B_;
    } else {
	Warn("Invalid option for backtracking mode -- using default mode");
    }
}

void SAT_Cmds::setDecideMode (CmdArg &arg)
{
    if (arg.matches("F")) {
	_mode[_DECISION_LEVEL_] = _FIXED_;
    }
    else if (arg.matches("LCS")) {
	_mode[_DECISION_LEVEL_] = _LCS_;
    }
    else if (arg.matches("LIS")) {
	_mode[_DECISION_LEVEL_] = _LIS_;
    }
    else if (arg.matches("CA")) {
	_mode[_DECISION_LEVEL_] = _CA_;
    }
    else if (arg.matches("JW")) {
	_mode[_DECISION_LEVEL_] = _JW_;
    }
    else if (arg.matches("CAJW")) {
	_mode[_DECISION_LEVEL_] = _CAJW_;
    }
    else if( arg.matches( "P" ) ) {
	_mode[_DECISION_LEVEL_] = _PARTITION_;
    } 
    else if (arg.matches("MSOS")) {
	_mode[_DECISION_LEVEL_] = _MSOS_;
	_mode[_DECISION_MODE_] = _DYNAMIC_ORD_;
    }
    else if (arg.matches("MSTS")) {
	_mode[_DECISION_LEVEL_] = _MSTS_;
	_mode[_DECISION_MODE_] = _DYNAMIC_ORD_;
    }
    else if (arg.matches("MSMM")) {
	_mode[_DECISION_LEVEL_] = _MSMM_;
	_mode[_DECISION_MODE_] = _DYNAMIC_ORD_;
    }
    else if (arg.matches("BOHM")) {
	_mode[_DECISION_LEVEL_] = _BOHM_;
	_mode[_DECISION_MODE_] = _DYNAMIC_ORD_;
    }
    else if (arg.matches("DLIS")) {
	_mode[_DECISION_LEVEL_] = _DLIS_;
	_mode[_DECISION_MODE_] = _DYNAMIC_ORD_;
    }
    else if (arg.matches("DLCS")) {
	_mode[_DECISION_LEVEL_] = _DLCS_;
	_mode[_DECISION_MODE_] = _DYNAMIC_ORD_;
    }
    else if (arg.matches("DJW")) {
	_mode[_DECISION_LEVEL_] = _DJW_;
	_mode[_DECISION_MODE_] = _DYNAMIC_ORD_;
    }
    else {
	Warn("Invalid option for selection mode -- using default mode");
    }
}

void SAT_Cmds::setMultConfs (CmdArg &arg)
{
    _mode[_MULTIPLE_CONFLICTS_] = TRUE;
    if (arg.matches("M")) {
	_mode[_MULTI_CONF_HANDLING_] = _MERGED_;
    } else if (arg.matches("S")) {
	_mode[_MULTI_CONF_HANDLING_] = _SEPARATE_;
    } else {
	Warn("Invalid option for multiple conflicts -- using default mode");
    }
}

void SAT_Cmds::setPreprocessMode (CmdArg &arg)
{
    if (arg.matches("R")) {
	_mode[_PREPROC_MODE_] = _W_RELAXATION_;
	_mode[_PREPROC_LEVEL_] = (int) (arg >> 1);
    } else {
	_mode[_PREPROC_LEVEL_] = (int) arg;
    }
}

//-----------------------------------------------------------------------------
// Parse for the input command line arguments (for batch execution).
//-----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Function: cmdLineParse
//
// Purpose: Parses the initial command line arguments and uses them for
//          configuring GRASP.
//
// Notes: See GRASP description for an overview of the default values.
//        The file name, if any, has to be the last argument.
//
// History: 6/21/95 - JPMS - created.
// ----------------------------------------------------------------------------

char *SAT_Cmds::cmdLineParse (int argc, char *argv[])
{
    CmdLine cmd_line (argc, argv);
    
    for (int k = 1; k < argc; k++) {

	if (cmd_line[k].matches("+")) {
	    handlePlusOption (cmd_line[k] >> 1);
	}
	else if (cmd_line[k].matches("-")) {
	    handleMinusOption (cmd_line[k] >> 1);
	}
	else {                                             // File name assumed
	    DBG1(cout<<"Filename: "<<argv[k]<<endl;);
	    return argv[k];                                 // Return file name
	}
    }
    return NULL;
}

//-----------------------------------------------------------------------------
// Parses command line options of the form +Zy.
//-----------------------------------------------------------------------------

int SAT_Cmds::handlePlusOption (CmdArg &cmd)
{
    if (cmd.matches("b")) {
	setBacktrackMode (cmd >> 1);
    }
    else if (cmd.matches("d")) {
	setDecideMode (cmd >> 1);
    }
    else if (cmd.matches("g")) {                          // DB growth is bound
	_mode[_DB_GROWTH_] = _POLYNOMIAL_DB_;
	_mode[_CONF_CLAUSE_SIZE_] = (int) (cmd >> 1);
    }
    else if (cmd.matches("i")) {
	_mode[_DEDUCTION_LEVEL_] = (cmd >> 1);
    }
    else if (cmd.matches("m")) {
	setMultConfs (cmd >> 1);
    }
    else if (cmd.matches("p")) {
	setPreprocessMode (cmd >> 1);
    }
    else if( cmd.matches("r") ) {
	_mode[_CONF_CLAUSE_REPACKING_] = TRUE;
    }
    else if( cmd.matches("t") ) {
	_mode[_TRIM_SOLUTIONS_] = TRUE;
    }
    else if( cmd.matches("u") ) {
	_mode[_IDENTIFY_UIPS_] = TRUE;
    }
    else if (cmd.matches("B")) {
	_mode[_BACKTRACK_LIMIT_] = (int) (cmd >> 1);
    }
    else if (cmd.matches("C")) {
	_mode[_CONFLICT_LIMIT_] = (int) (cmd >> 1);
    }
    else if (cmd.matches("T")) {
	_mode[_TIME_LIMIT_] = (int) (cmd >> 1);
    }
    else if (cmd.matches("S")) {
	_mode[_SPACE_LIMIT_] = (int) (cmd >> 1);
    }
    else if (cmd.matches("F")) {
	Abort("Cannot read configuration file yet");
    }
    else if (cmd.matches("D")) {
	_mode[_DEBUG_] = TRUE;
    }
    else if (cmd.matches("V")) {
	_mode[_VERBOSE_] = TRUE;
    }
    else {
	cout << "\n    Unsupported plus (+) option: " << (char*) cmd << endl;
	cout << endl;
	return FALSE;
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// Parses command line options of the form +Zy.
//-----------------------------------------------------------------------------

int SAT_Cmds::handleMinusOption (CmdArg &cmd)
{
    if( cmd.matches("r") ) {
	_mode[_CONF_CLAUSE_REPACKING_] = FALSE;
    }
    else if( cmd.matches("t") ) {
	_mode[_TRIM_SOLUTIONS_] = FALSE;
    }
    else if (cmd.matches("u")) {
	_mode[_IDENTIFY_UIPS_] = FALSE;
    }
    else if (cmd.matches("D")) {
	_mode[_DEBUG_] = FALSE;
    }
    else if (cmd.matches("V")) {
	_mode[_VERBOSE_] = FALSE;
    }
    else {
	cout << "\n    Unsupported minus (-) option: " << (char*) cmd << endl;
	cout << endl;
	return FALSE;
    }
    return TRUE;
}

// ----------------------------------------------------------------------------
// Function: cmdParse
//
// Purpose: Parses commands in on-line mode.
//
// Notes: It may return a file name to be parsed.
//
// History: 6/21/95 - JPMS - created.
// ----------------------------------------------------------------------------

char *SAT_Cmds::cmdParse( char *str )
{
    CmdLine command( str );

    //-------------------------------------------------------------------------
    // Under construction.
    //-------------------------------------------------------------------------

    return NULL;
}

// ----------------------------------------------------------------------------
// Function: fileParse
//
// Purpose: Parses a configuration file for GRASP.
//
// Notes: --
//
// History: 6/21/95 - JPMS - created.
// ----------------------------------------------------------------------------

int SAT_Cmds::fileParse( char *name )
{
    //-------------------------------------------------------------------------
    // Under construction.
    //-------------------------------------------------------------------------

    return TRUE;
}

/*****************************************************************************/

