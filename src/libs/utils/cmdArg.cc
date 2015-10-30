//
// file:      CmdArg.cc
// purpose:   code declarations for command argument class
//

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "defs.hh"
#include "cmdArg.hh"


//
// Allocates space and fills a string.
//
void CmdArg::define_arg(char *str)
{
    int len = strlen(str);
    _string = new char [len+1];
    char *p = _string;
    char *q = str;
    while(*(p++) = *(q++)) ;
    _ref_ptr = _string;
    DBGn(cout<<"\t-> Input arg:  "<<str<<endl;
	 cout<<"\t-> Output arg: "<<_string<<endl;);
}


//
// assignment operator (from CmdArg)
//
CmdArg &CmdArg::operator= (CmdArg &arg) {
    DBGn(cout<<"\t-> Cmd oper = ARG"<<endl;);
    define_arg(arg._string);
    return *this;
}

//
// assignment operator (from char*)
//
CmdArg &CmdArg::operator= (char *arg) {
    DBGn(cout<<"\t-> Cmd oper = CHAR"<<endl;);
    define_arg(arg);
    return *this;
}


//-----------------------------------------------------------------------------
// Access complete string.
//-----------------------------------------------------------------------------

char *CmdArg::string() { return _ref_ptr; }


//-----------------------------------------------------------------------------
// access argument as a string
//-----------------------------------------------------------------------------

CmdArg::operator char*(void) {
    return _string;
}

//
// access argument as if it were an integer
//
CmdArg::operator int(void) {
    int value;
    if (sscanf (_string, "%d", &value) == 1)
	return value;
    else if (matches("TRUE"))
	return 1;
    else if (matches("FALSE"))
	return 0;
    else if (matches("ON"))
	return 1;
    else if (matches("OFF"))
	return 0;
    else // throw an exception?
	return 0;
}

//
// access argument as if it were a long integer
//
CmdArg::operator long(void) {
    long value;
    if (sscanf (_string, "%ld", &value) == 1)
	return value;
    else // throw an exception?
	return 0;
}

//
// access argument as if it were a float
//
CmdArg::operator float(void) {
    float value;
    if (sscanf (_string, "%f", &value) == 1)
	return value;
    else // throw an exception?
	return 0;
}

//
// access argument as if it were a double
//
CmdArg::operator double() {
    double value;
    if (sscanf (_string, "%lf", &value) == 1) {
	return value;
    } else { // throw an exception?
	return 0;
    }
}


//-----------------------------------------------------------------------------
// Shifts the string pointer by pos positions.
//-----------------------------------------------------------------------------

CmdArg &CmdArg::operator>> ( int pos )
{
    for( int k = 0; k < pos && *_string; k++ ) {
	_string++;
    }
    CHECK(if(!*_string)cout<<"Shifted argument out of bounds!?"<<endl;);
    return *this;
}


//
// Test argument to see if it matches a given key.
//
// The match rules for a key "THISkey" are:
//      1. All initial capitalized letters in the key must be matched
//      2. Any additional argument characters must match the remaining 
//         lowercase key
//
// Keys MUST begin with capital letters, and can optionally be 
// followed by lowercase letters.  No capitals can follow a lower
// case letter.
//
int CmdArg::matches (char *str) {
    DBGn(cout<<"\t-> CMD STRING: "<<_string<<endl;
	 cout<<"\t-> CMP STRING: "<<str<<endl;);
    char *c = str;
    char *p = _string;
    //
    // Match all characters of the input string.
    //
    while(*c) {
	if (*c++ != *p++)
	    return FALSE;
    }
    return TRUE;
}

