//-----------------------------------------------------------------------------
// File: memory.cc
//
// Purpose: Memory management functions for ListItems.
//
// Remarks: Required because templates do not permit static variables in
//          gcc 2.7.2
//
// History: 7/2/94 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <iostream.h>

#include "defs.hh"
#include "memory.hh"


void *newList[MEM_DIM_SIZE];
unsigned long mem_defined = 0;
unsigned long memblock_allocs[MEM_DIM_SIZE];
unsigned long new_count[MEM_DIM_SIZE];
unsigned long del_count[MEM_DIM_SIZE];


void init_memory()
{
    DBG1(cout << "Clearing memory" << endl;);
    for( register int k = 0; k < MEM_DIM_SIZE; k++ ) {
	newList[k] = (void*) NULL;
	memblock_allocs[k] = 0;
	new_count[k] = 0;
	del_count[k] = 0;
    }
    mem_defined = 1;
}

DMEM(
STAT(
void print_mem_stats()
{
    cout << "\n  UTILS memory allocation stats:\n";
    for( register int k = 0; k < MEM_DIM_SIZE; k++ ) {
	if( memblock_allocs[k] ) {

	    cout << "    Size of each new: " << k << "    Number of ";
	    cout << ALLOC_BLOCK_SIZE << " blocks allocated: ";
	    cout << memblock_allocs[k] << endl;

	    cout << "    Number of NEW calls:    " << new_count[k] << endl;
	    cout << "    Number of DELETE calls: " << del_count[k] << endl;
	}
    }
}))

/*****************************************************************************/

