//-----------------------------------------------------------------------------
// File: memory.hh
//
// Purpose: Basic definitions for memory management in basic data types.
//          A global list of free storage is maintained, which is accessed
//          by the allocation methods of basic data types.
//
// Remarks: Several ideas for memory management are taken from an early
//          version of the LEDA software (developed by Stephan Naher).
//
// History: 7/2/94 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef _MEM_DEF
#define _MEM_DEF


#include "defs.hh"


//-----------------------------------------------------------------------------
// Memory management for classes derived from the template ListItem. Size
// of entries stored in each ListItem must be less then MEM_DIM_SIZE = 4096.
// I have not been able to compile a cleaner implementation of memory
// management (see macro definitions below), which does not use external
// variables.
//-----------------------------------------------------------------------------


#define ALLOC_BLOCK_SIZE  512
#define MEM_DIM_SIZE      4096


extern void *newList[];
extern unsigned long mem_defined;
extern unsigned long memblock_allocs[];
extern unsigned long new_count[];
extern unsigned long del_count[];


extern void init_memory();
extern void print_mem_stats();


//-----------------------------------------------------------------------------
// Calls memory initilization. ANY program using this software *MUST* call
// this macro before executing any other instructions.
//-----------------------------------------------------------------------------

#define MEM_MNG_INIT() \
if(!mem_defined) { \
   init_memory(); \
}


//-----------------------------------------------------------------------------
// Macro definitions for implementing memory management in classes of known
// fixed size. New and delete operators are redefined. Some statistics can
// optionally be recorded. Need to compile with option -DMEM for dedicated
// memory management and -DSTAT for accessing stats.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro to include include in class declarations with memory management.
//-----------------------------------------------------------------------------

#define MEM_MNG_DECL(ClassType, ClassName) \
DMEM( \
public: \
    inline void *operator new(size_t _size) { \
	if( !newList ) { \
	    STAT(memblock_allocs++;) \
	    newList = (ClassType*) new char[ALLOC_BLOCK_SIZE * _size]; \
	    register int k; \
	    for( k = 0; k < ALLOC_BLOCK_SIZE-1; k++ ) { \
		newList[k].freePtr = &(newList[k+1]); \
	    } \
	    newList[k].freePtr = NULL; \
	} \
	STAT(new_count++;) \
	ClassType *ret_ptr = newList; \
	newList = newList->freePtr; \
	return ret_ptr; \
    } \
    inline void operator delete(void *ptr) { \
	STAT(del_count++;) \
	ClassType *ref_ptr = (ClassType*) ptr; \
	ref_ptr->freePtr = newList; \
	newList = ref_ptr; \
    } \
    STAT(static void print_mem_stats();) \
private: \
    ClassType *freePtr; \
    static ClassType *newList; \
    STAT( \
	static unsigned long memblock_allocs; \
	static unsigned long new_count; \
	static unsigned long del_count;))


//-----------------------------------------------------------------------------
// Associated definitions for implementing memory management.
//-----------------------------------------------------------------------------

#define MEM_MNG_DEF(ClassType, ClassName) \
DMEM( \
ClassType *ClassType::newList = NULL; \
STAT( \
unsigned long ClassType::memblock_allocs = 0; \
unsigned long ClassType::new_count = 0; \
unsigned long ClassType::del_count = 0;) \
STAT( \
void ClassType::print_mem_stats() \
{ \
    cout << "\n  Memory stats for class " << ClassName << ":\n"; \
    cout << "    Size of class: " << sizeof(ClassType); \
    cout << "    Number of " << ALLOC_BLOCK_SIZE << " blocks allocated: "; \
    cout << memblock_allocs << endl; \
    cout << "    Number of NEW calls:    " << new_count << endl; \
    cout << "    Number of DELETE calls: " << del_count << endl; \
}))

#endif

/*****************************************************************************/

