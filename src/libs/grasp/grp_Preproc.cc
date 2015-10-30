//-----------------------------------------------------------------------------
// File: grp_Preproc.cc
//
// Purpose: Member functions of the preprocessing engine.
//
// Remarks: --
//
// History: 03/14/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "grp_Preproc.hh"


//-----------------------------------------------------------------------------
// Function: preprocess()
//
// Purpose: Level 0 preprocessing. For each variable apply both assignments
//          and identify necessary assignments and clauses of size two.
//
// Side-effects: Some variables can become assigned. In addition, some
//               implicates of the consistency function can be identified.
//
// Notes: Still far from the easily configurable preprocessing engine. The
//        same also holds true with the deduction engine.
//-----------------------------------------------------------------------------

int PreprocessEngine::preprocess (SAT_ClauseDatabase &sat, int with_relaxation)
{



}

/*****************************************************************************/
