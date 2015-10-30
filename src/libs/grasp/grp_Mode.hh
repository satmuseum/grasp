//-----------------------------------------------------------------------------
// File: grp_Mode.hh
//
// Purpose: Mode
//
// Remarks: 
//
// History: 03/15/96 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef __GRP_MODE__
#define __GRP_MODE__

#include "array.hh"


//-----------------------------------------------------------------------------
// Defines common to the interface of the SAT algorithm and the clause
// database.
//-----------------------------------------------------------------------------

enum SatModes {
    _DEBUG_ = 0,                                    // Print debugging messages
    _VERBOSE_,                   // Print verbose info regarding search process
    _BACKTRACK_LIMIT_,                  // Maximum number of backtracks allowed
    _CONFLICT_LIMIT_,                    // Maximum number of conflicts allowed
    _TIME_LIMIT_,                                   // Maximum CPU time allowed
    _SPACE_LIMIT_,                 // Largest factor of growth of the clause DB
    _PREPROC_LEVEL_,                           // Level of preprocessing chosen
    _PREPROC_MODE_,                // Mode in which preprocessing is to be done
    _DEDUCTION_LEVEL_,                             // Level of deduction chosen
    _DEDUCTION_MODE_,                                      // Yet to be defined
    _DIAGNOSIS_LEVEL_,                         // Level of preprocessing chosen
    _DIAGNOSIS_MODE_,                                      // Yet to be defined
    _DECISION_LEVEL_,                          // Level of preprocessing chosen
    _DECISION_MODE_,            // Whether decision making is static or dynamic
    _IDENTIFY_UIPS_,                       // Whether UIPs are to be identified
    _MULTIPLE_CONFLICTS_,    // Whether multiple conflicts should be identified
    _MULTI_CONF_HANDLING_,          // Strategy for handling multiple conflicts
    _BACKTRACKING_STRATEGY_,              // Which backtracking strategy to use
    _DB_GROWTH_,                        // Chosen growth of teh clause database
    _CONF_CLAUSE_SIZE_,          // Largest clause size to add to the clause DB
    _TRIM_SOLUTIONS_,       // Prune computed solutions of irrelevant decisions
    _CONF_CLAUSE_REPACKING_     // Retrace depdendencies to decisions if useful
    };

enum { SAT_OPTION_NUMBER = (_CONF_CLAUSE_REPACKING_+1) };


enum BackStrategies {
    _CHRONOLOGICAL_B_,           // Standard chronological backtracking is used
    _NON_CHRONOLOGICAL_B_,            // Non-chronological backtracking is used
    _DYNAMIC_B_             // Unordered backtracking/conflict handling is used
    };

enum DBGrowths {
    _EXPONENTIAL_DB_,       // Growth of the clause database can be exponential
    _POLYNOMIAL_DB_            // Growth of the clause database *is* polynomial
    };

enum MultiConfHandling {
    _MERGED_,                   // Handle all conflicts simultaneously (faster)
    _SEPARATE_                      // Handle each conflict separately (slower)
    };

enum PreprocessingMode {
    _W_RELAXATION_,                  // Preprocess while "useful" info is found
    _WO_RELAXATION_                   // Preprocess based on a single iteration
    };

enum DecisionModes {
    _STATIC_ORD_,                 // Decision ordering chosen before teh search
    _DYNAMIC_ORD_                // Best assignment chosen before each decision
    };

enum DecisionLevels {                             // Decision making procedures
    _FIXED_, _LCS_, _LIS_, _CA_, _JW_, _CAJW_, _PARTITION_,
    _MSOS_, _MSTS_, _MSMM_, _BOHM_, _DLIS_, _DLCS_, _DJW_
    };



//-----------------------------------------------------------------------------
// Additional defines for GRASP_SAT interfacing with different engines.
//-----------------------------------------------------------------------------

enum DeduceResult { CONFLICT = 0x40, NO_CONFLICT = 0x41 };

enum DecideResult { DECISION = 0x50, SOLUTION = 0x51  };



//-----------------------------------------------------------------------------
// Class: SAT_Mode
//
// Purpose: Specified operating modes for the SAT algorithm.
//-----------------------------------------------------------------------------

class SAT_Mode {
public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------
    
    SAT_Mode();
    virtual ~SAT_Mode();

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    virtual void setup();
    
    inline int& operator[] (int idx) {
	return _mode[idx];
    }

protected:

    virtual void alloc_mode();
    virtual void init_mode();

    Array<int> _mode;

};

#endif // __GRP_MODE__

/*****************************************************************************/
