//-----------------------------------------------------------------------------
// File: array.hh
//
// Purpose: Template definition of a one-dimensional array.
//
// Remarks: --
//
// History: 5/18/94 - JPMS - created, but it is heavily based on the Vector
//                           class in MooCAF (developed by Tim Burks). 
//          1/18/96 - JPMS - modified: proper inlining of inlined functions
//                           requires joint declaration and definition
//                           (in gcc 2.7.2)
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef _ARRAY_DEF_
#define _ARRAY_DEF_

#include "defs.hh"



//-----------------------------------------------------------------------------
// Class: Array
//
// Purpose: One dimensional array of abstract objects. No specific size
//          is required at construction time.
//-----------------------------------------------------------------------------

template<class T>
class Array {

public:
    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    Array(int size=0) : _size(size) { _array = (size==0) ? 0 : new T [size]; }
    ~Array(void) { if(_array) delete[] _array; }

    //-------------------------------------------------------------------------
    // array information accessors/modifiers
    //-------------------------------------------------------------------------

    inline T& operator[] (int index) {
	CHECK(if(index < 0 || index >= _size)
	      {cout<<"Index: "<<index<<" in Size: "<<_size<<endl;
	       Abort("Invalid array range specification");});
	return _array[index];
    }
    inline const T& operator[] (int index) const {
	CHECK(if(index < 0 || index >= _size)
	      Abort("Invalid array range specification"););
	return _array[index];
    }
    inline int size(void) const { return _size; }
    inline int resize(int newsize) {          // newsize MUST be > current size
	if(newsize == 0) {          // if newsize is zero, throw away the array
	    if(_array) delete[] _array;
	    _array = 0;
	    _size = 0;
	} else {
	    T *newarray = new T [newsize];
	    if(newarray) {	                               // copy old data
		for(register int i = 0; i < _size; i++)
		    newarray [i] = _array [i];
		// delete old array, update internal status and return

		if(_array) delete[] _array;
		_array = newarray;
		_size = newsize;
	    }
	    CHECK(else Abort("ARRAY MEMORY ALLOCATION FAILURE"););
	}
	return(TRUE);
    }
protected:
    T *_array;		// points to array storage
    int _size;		// size of array
};


// NOTE: Future versions will define member functions after class declaration
//       (see example below).

//
// function:     Array<T>::operator[] ()
// purpose:      indexing operator
// parameters:   int index
// returns:      reference to array entry
//
//template<class T>
//T& Array<T>::operator[] (int index)
//{
//    CHECK(if(index < 0 || index >= _size)
//	  {cout<<"Index: "<<index<<" in Size: "<<_size<<endl;
//	   Abort("Invalid array range specification");});
//    return _array[index];
//}

#endif    // _ARRAY_DEF_

/*****************************************************************************/

