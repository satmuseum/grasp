//-----------------------------------------------------------------------------
// File: sortInt.hh
//
// Purpose: Implements a O(min(n,k.log.k)) sorting algorithm where k=O(n).
//          Keys are assumed to be integer, distributed between 0 and n-1,
//          and number of keys to sort is k (usually less than n). Algorithm
//          runs in O(n) if c.k.log.k > n, by using an array to distribute
//          keys (and remove duplicates). Otherwise, algorithm runs in
//          O(k.log.k) using a non-recursive implementation of MergeSort.
//          It also merges two sorted lists of ints into a new sorted list.
//
// Remarks: --
//
// History: 6/9/94 - JPMS - first version.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef _SORTER_ALG
#define _SORTER_ALG

#include <math.h>

#include "defs.hh"
#include "array.hh"
#include "list.hh"


#define MRG_SRT_CONST 4                   // Constant (assumed) for merge sort



//-----------------------------------------------------------------------------
// Class: Sorter
//
// Purpose: Sorter class for interger keys.
//-----------------------------------------------------------------------------

class Sorter {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    Sorter() : array_1(0), array_2(0), array_n(0)
	{ _size = 0; initialized = FALSE; }
    Sorter(int nsize) : array_1(nsize), array_2(nsize), array_n(nsize)
	{ _size = nsize; initialized = TRUE; }
    ~Sorter() { array_1.resize(0); array_2.resize(0); array_n.resize(0); }

    //-------------------------------------------------------------------------
    // Initialization/resizing.
    //-------------------------------------------------------------------------

    inline void initialize(int nsize) {
	array_1.resize(nsize);array_2.resize(nsize);array_n.resize(nsize);
	_size = nsize;
	initialized = TRUE;
    }
    inline void resize(int nsize) { initialize(nsize); }

    //-------------------------------------------------------------------------
    // Sorting methods: pick one that runs in less time.
    //-------------------------------------------------------------------------

    inline List<int>& sort(List<int>& keys, int remove_duplicates = TRUE) {
	DBG1(cout<<"-> Entering Sorter::sort()"<<endl;);
	CHECK(if(!initialized) Abort("Sorter not initialized!?"););
	rm_duplics = remove_duplicates;
	register double range = (double) keys.size();
	if(_size <= MRG_SRT_CONST * range * log(range))
	    return(linear_sort(keys));
	return(merge_sort(keys));
    }

    //-------------------------------------------------------------------------
    // Merge method. Entries are extracted from the original lists.
    //-------------------------------------------------------------------------

    inline List<int>& merge(
	List<int>& list_a, List<int>& list_b, List<int>& list_o) {
	while(list_a.first() || list_b.first()) {
	    register ListItem<int> *item_a = list_a.first();
	    register ListItem<int> *item_b = list_b.first();
	    if(item_a && item_b) {
		if(item_a->data() <= item_b->data()) {
		    list_a.extract(item_a);
		    list_o.append(item_a);
		} else {
		    list_b.extract(item_b);
		    list_o.append(item_b);
		}
	    } else {
		while(item_a = list_a.first()) {
		    list_a.extract(item_a);
		    list_o.append(item_a);
		}
		while(item_b = list_b.first()) {
		    list_b.extract(item_b);
		    list_o.append(item_b);
		}
	    }
	}
	DBG1(list_o.dump(););
	return list_o;
    }
  private:                                                   // Private methods

    //-------------------------------------------------------------------------
    // Linear time sort.
    //-------------------------------------------------------------------------

    inline List<int>& linear_sort(List<int>& keys) {
	DBG1(cout<<"-> Entering Sorter::linear_sort()"<<endl;);
	register ListItem<int> *item;
	register int key;
	while(item = keys.first()) {
	    key = item->data();
	    if(!rm_duplics || !array_n[key].size()) {
		keys.extract(item);
		array_n[key].append(item);
	    }
	    else keys.remove(item);
	}
	DBG1(cout<<"\t-> Filling sorted key list"<<endl;);
	for(key = 0; key < _size; key++) {
	    keys.append(array_n[key]);
	    array_n[key].loadList(NULL, NULL, 0);
	}
	DBG1(cout<<"\t-> Final key set size: "<<keys.size()<<endl;
	     keys.dump(););
	return keys;
    }

    //-------------------------------------------------------------------------
    // Merge sort (k.log.k)
    //-------------------------------------------------------------------------

    inline List<int>& merge_sort(List<int>& keys) {
	DBG1(cout<<"-> Entering Sorter::merge_sort()"<<endl;);
	DBGn(cout<<"\t-> Dumping keys"<<endl;keys.dump(););
	register int key_size = keys.size();
	register Array<ListItem<int>*> *array_a, *array_b, *array_tmp;

	// Load keys into first array and sort pairwise.

	register ListItem<int> *item = keys.first();
	keys.extract(item);
	array_1[0] = item;
	register int index = 0;

	while(item = keys.first()) {
	    keys.extract(item);
	    if(EVEN(index) && item->data() < array_1[index]->data()) {
		array_1[index+1] = array_1[index];
		array_1[index++] = item;
	    }
	    else array_1[++index] = item;
	}
	// Main loop(s): merge (into the array) current partition of array,
	// NOTE: We ought to avoid the final merge until the list is filled.

	array_a = &array_1;
	array_b = &array_2;
	DBGn(cout<<"    -> Dumping keys"<<endl<<"      ";
	     for(int k=0;k<key_size;k++)
	     cout<<(*array_a)[k]->data()<<" ";cout<<endl;);
	for(register int mrg_size = 2; mrg_size < key_size;
	    mrg_size = 2*mrg_size) {
	    register int refb = 0;

	    for(index = 0; index < key_size; index += 2*mrg_size) {
		register int ref1 = index;
		register int ref2 = index+mrg_size;

		register int end1 = MIN(ref2,key_size);
		register int end2 = MIN(ref2+mrg_size,key_size);

		while(ref1 < end1 || ref2 < end2) {
		    if(ref1 < end1 && ref2 < end2) {
			if((*array_a)[ref1]->data()<=(*array_a)[ref2]->data())
			    (*array_b)[refb++] = (*array_a)[ref1++];
			else
			    (*array_b)[refb++] = (*array_a)[ref2++];
		    } else {
			while(ref1 < end1)
			    (*array_b)[refb++] = (*array_a)[ref1++];
			while(ref2 < end2)
			    (*array_b)[refb++] = (*array_a)[ref2++];
		    }
		}
		CHECK(if(refb < end2) Warn("NOT all keys moved!?"););
	    }
	    array_tmp = array_a;
	    array_a = array_b;
	    array_b = array_tmp;
	    DBGn(cout<<"    -> Dumping keys"<<endl<<"      ";
		 for(int k=0;k<key_size;k++)
		 cout<<(*array_a)[k]->data()<<" ";cout<<endl;);
	    CHECK(if(refb < key_size) Warn("NOT all keys moved!?"););
	}
	// Final merge, that loads directly to the keys array. If required
	// duplicates are removed.
	// NOTE: Above loop could have one less iteration.

	DBG1(cout<<"\t-> Filling sorted key list"<<endl;);
	keys.append((*array_a)[0]);
	if(!rm_duplics) {
	    for(index = 1; index < key_size; index++)
		keys.append((*array_a)[index]);
	} else {
	    for(index = 1; index < key_size; index++)
		if(keys.last()->data() != (*array_a)[index]->data())
		    keys.append((*array_a)[index]);
		else
		    delete (*array_a)[index];       // Remove duplicate entries
	}
	CHECK(int lastI=NONE;for(item=keys.first();item;item=item->next())
	      {if(item->data()<lastI)Warn("Sorted keys out of order!?");
	       lastI=item->data();});
	DBG1(cout<<"\t-> Dumping keys"<<endl;keys.dump(););
	return keys;
    }
  private:
    Array<ListItem<int>*> array_1, array_2;    // Arrays required for mergesort
    Array<List<int> > array_n;                 // Array to sort in O(n)
    int _size;

    int rm_duplics;                            // Whether to remove duplicates
    int initialized;
};

#endif    // SORTER_ALG

/*****************************************************************************/

