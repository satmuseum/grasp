//-----------------------------------------------------------------------------
// File: utils.cc
//
// Purpose: General purpose utilities for printing out messages.
//
// Remarks: --
//
// History: 6/16/95 - JPMS - created -- adapted from other utils files.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>


//-----------------------------------------------------------------------------
// Functions for printing error/warn messages.
//-----------------------------------------------------------------------------

static char *trailer = NULL;

//
// Sets the traling keyword for each message. For example "GRASP"
//
void setTrailString( char *str )
{
    trailer = str;
}


//
// Abort: Exits the program if error conditions are detected.
//
void Abort( char *s )
{
    cout << "\n    " << trailer << " Error: " << s << endl;
    cout << "\tAborting Execution....\n\n" << endl;
    exit(1);
}


//
// Warn: Gives a warning on an unexpected condition.
//
void Warn( char *s )
{
    cout << "\n    " << trailer << " Warning: " << s << endl;
    cout << "\tContinuing Execution....\n\n" << endl;
}


//
// Warn: Gives a run-time information message.
//
void Info( char *s )
{
    cout << "\n    " << trailer << " Information: " << s << endl;
}


//-----------------------------------------------------------------------------
//
// Functions for I/O interfacing and for printing general data.
//
//-----------------------------------------------------------------------------

#define STR_COL_WIDTH 60
#define NUM_COL_WIDTH 10

//
// Formatted printing of string number pairs.
//
void printItem() { cout << endl; }


void printItem(char *str, int num)
{
    cout << "  ";
    cout.setf(ios::left,ios::adjustfield);
    cout.width(STR_COL_WIDTH);
    cout.fill(' ');
    cout << str;
    cout.setf(ios::right,ios::adjustfield);
    cout.width(NUM_COL_WIDTH);
    cout << num << endl;
}


void printItem(char *str, double rnum)
{
    cout << "  ";
    cout.setf(ios::left,ios::adjustfield);
    cout.width(STR_COL_WIDTH);
    cout.fill(' ');
    cout << str;
    cout.setf(ios::right,ios::adjustfield);
    cout.width(NUM_COL_WIDTH);
    cout.precision(3);
    cout << rnum << endl;
}


//
// Prints timing information, either elapsed time or (optionally) the total
// time.
//
void printTime( char *msg, double time, char *tmsg )
{
    cout.precision(5);
    cout << "\t" << msg << ". " << tmsg << " time: " << time << endl;
}

/*****************************************************************************/

