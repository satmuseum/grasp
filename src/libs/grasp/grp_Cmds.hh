//-----------------------------------------------------------------------------
// File: grp_Cmds.hh
//
// Purpose: Handles command line arguments for configuring the GRASP SAT
//          algorithmci framework.
//
// Remarks: None.
//
// History: 6/21/95 - JPMS - created.
//          3/16/96 - JPMS - repackaged for new version.
//-----------------------------------------------------------------------------

#ifndef __GRP_CMDS
#define __GRP_CMDS

#include "cmdArg.hh"
#include "cmdLine.hh"

#include "grp_Mode.hh"    // Only needs to be aware of the configuration modes


//-----------------------------------------------------------------------------
// Class: SAT_Cmds
//
// Purpose: Parser for command line options for batch execution.
//-----------------------------------------------------------------------------

class SAT_Cmds {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------
    
    SAT_Cmds (SAT_Mode &nmode);
    virtual ~SAT_Cmds();

    //-------------------------------------------------------------------------
    // Interface methods.
    //-------------------------------------------------------------------------

    virtual char *cmdLineParse (int argc, char *argv[]);
    virtual char *cmdParse (char *str);
    virtual int fileParse (char *name);

  protected:

    //-------------------------------------------------------------------------
    // Protected methods for configuring some options of GRASP.
    //-------------------------------------------------------------------------

    virtual int handlePlusOption (CmdArg &cmd);
    virtual int handleMinusOption (CmdArg &cmd);

    void setBacktrackMode (CmdArg &arg);
    void setDecideMode (CmdArg &arg);
    void setMultConfs (CmdArg &arg);
    void setPreprocessMode (CmdArg &arg);

  private:

    //-------------------------------------------------------------------------
    // Data structures for configuring the search engine.
    //-------------------------------------------------------------------------

    SAT_Mode &_mode;
};

#endif

/*****************************************************************************/
