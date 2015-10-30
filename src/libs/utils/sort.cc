//-----------------------------------------------------------------------------
// File: sort.cc
//
// Purpose: Algorithms for sorting lists. Currently, only a function that
//          implements MergeSort is defined.
//
// Remarks: --
//
// History: 6/22/95 - JPMS - created.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#include <math.h>

#include "defs.hh"
#include "array.hh"
#include "list.hh"
#include "sort.hh"


//-----------------------------------------------------------------------------
// Function: mergeSort
//
// Purpose: Sorts a list of objects in decreasing order using MergeSort.
//
// Side-effects: List gets sorted.
//-----------------------------------------------------------------------------

List<sortType*> &mergeSort(List<sortType*> &keys) {
    DBG1(cout<<"-> Entering ::merge_sort()"<<endl;);
    Array<ListItem<sortType*>*> array_1( keys.size() );
    Array<ListItem<sortType*>*> array_2( keys.size() );

    register int key_size = keys.size();
    register Array<ListItem<sortType*>*> *array_a, *array_b, *array_tmp;

    // Load keys into first array and sort pairwise.

    register ListItem<sortType*> *item = keys.first();
    keys.extract(item);
    array_1[0] = item;
    register int index = 0;

    while(item = keys.first()) {
	keys.extract(item);
	if(EVEN(index) &&
	   item->data()->value() < array_1[index]->data()->value()) {
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
	 cout<<(*array_a)[k]->data()->value()<<" ";cout<<endl;);
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
		    double val_a1 = (*array_a)[ref1]->data()->value();
		    double val_a2 = (*array_a)[ref2]->data()->value();

		    if(val_a1 <= val_a2)
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
	     cout<<(*array_a)[k]->data()->value()<<" ";cout<<endl;);
	CHECK(if(refb < key_size) Warn("NOT all keys moved!?"););
    }

    // Final merge, that loads directly to the keys array. If required
    // duplicates are removed.
    // NOTE: Above loop could have one less iteration.

    DBG1(cout<<"\t-> Filling sorted key list"<<endl;);
    keys.append((*array_a)[0]);
    for(index = 1; index < key_size; index++) {
	keys.append((*array_a)[index]);
    }
    array_1.resize(0);
    array_2.resize(0);
    return keys;
}

/*****************************************************************************/

