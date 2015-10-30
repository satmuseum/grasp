//-----------------------------------------------------------------------------
// File: sort.hh
//
// Purpose: Definition of data types assumed by sort procedures.
//
// Remarks: --
//
// History: 6/22/95 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef _SORT_DEFS
#define _SORT_DEFS


#include "defs.hh"
#include "array.hh"
#include "list.hh"


class sortType;


//-----------------------------------------------------------------------------
//
// Available sorting functions
//
//-----------------------------------------------------------------------------

extern List<sortType*> &mergeSort( List<sortType*> &keys );



//-----------------------------------------------------------------------------
//
// Type definition for the objects to be sorted
//
//-----------------------------------------------------------------------------

class sortType {
public:

    // Constructor/destructor.

    sortType( void *Nptr, double Nvalue ) {
	_ptr = Nptr; _value = Nvalue;
    }
    sortType( void *Nptr, int Nvalue ) {
	_ptr = Nptr; _value = (double) Nvalue * 1.0;
    }
    ~sortType() {}

    // Acess methods.
    
    void *ptr() { return _ptr; }
    double value() { return _value; }

protected:
    void *_ptr;
    double _value;
};

#endif

/*****************************************************************************/

