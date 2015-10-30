//-----------------------------------------------------------------------------
// File: list.hh
//
// Purpose: List class definition for the GRASP software toolset.
//
// Remarks: --
//
// History: 5/18/94 - JPMS - first version, but adapated from the list
//                           class in MooCAF (developed by Tim Burks).
//          1/18/96 - JPMS - modified: proper inlining of inlined functions
//                           requires joint declaration and definition
//                           (in gcc 2.7.2)
//-----------------------------------------------------------------------------

#ifndef _LIST_DEF_
#define _LIST_DEF_

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "defs.hh"
#include "memory.hh"


template<class T>
class List;


//-----------------------------------------------------------------------------
// Class definitions: ListItem and List.
//-----------------------------------------------------------------------------

//
// class: 	ListItem
// purpose:	defines entries in a linked list of objects
//
template<class T>
class ListItem {
    friend class List<T>;
public:

    //-------------------------------------------------------------------------
    // constructors and destructors
    //-------------------------------------------------------------------------

    inline ListItem (const T& data) : _data (data), _prev(0), _next(0) {}
    inline ~ListItem (void) {
	CHECK(if(_next || _prev)
	      Warn("Invalid pointer management while deleting item"););
    }
    //-------------------------------------------------------------------------
    // accessors and modifiers
    //-------------------------------------------------------------------------

    inline ListItem *prev(void) const { return _prev; }
    inline ListItem *next(void) const { return _next; }
    inline T& data(void) { return _data; }
    inline T& data(const T& n_data) { _data = n_data; return _data; }

    //-------------------------------------------------------------------------
    // Memory management of objects of class ListItem<T>. External variables
    // have to be used since gcc 2.7.2 does not yet support static variables
    // in templates.
    //-------------------------------------------------------------------------

    DMEM(
    inline void *operator new(size_t _size) {
	// *** if(!mem_defined) init_memory(); DONE BEFORE ALLOCATING ITEMs ***
	if( !::newList[_size] ) {
	    STAT(::memblock_allocs[_size]++;)
	    ::newList[_size] = (void*) new char[ALLOC_BLOCK_SIZE * _size];
	    register int k;
	    for( k = 0; k < ALLOC_BLOCK_SIZE-1; k++ ) {
		((ListItem*)::newList[_size])[k]._next =
		    &(((ListItem*)::newList[_size])[k+1]);
	    }
	    ((ListItem*)::newList[_size])[k]._next = NULL;
	}
	STAT(::new_count[_size]++;)
	ListItem *ret_ptr = (ListItem*) ::newList[_size];
	::newList[_size] = (void*) ((ListItem*)::newList[_size])->_next;
	return ret_ptr;
    }
    inline void operator delete(void *ptr) {
	STAT(::del_count[sizeof(ListItem)]++;)
	((ListItem*)ptr)->_next = (ListItem*) ::newList[sizeof(ListItem)];
	::newList[sizeof(ListItem)] = ptr;
    })

protected:
    T _data;		 // list data
    ListItem *_prev;     // pointer to previous list entry
    ListItem *_next;     // pointer to next list entry

    //-------------------------------------------------------------------------
    // Restricted methods for contents manipulation.
    //-------------------------------------------------------------------------

    inline void setPrev(ListItem *n_prev) { _prev = n_prev; }
    inline void setNext(ListItem *n_next) { _next = n_next; }
};


//-----------------------------------------------------------------------------
// Class: List
//
// Purpose: Defines a list of objects of another class T.
//-----------------------------------------------------------------------------

template <class T>
class List {
public:

    //-------------------------------------------------------------------------
    // Constructors, destructors and operators.
    //-------------------------------------------------------------------------

    List(void) : _first(0), _last(0), _size(0) {}
    ~List(void) { erase(); }
    List& operator=(List& _from) {
	erase();
	for(ListItem<T> *item = _from.first(); item; item = item->next())
	    append (item->data());
	return *this;
    }
    //-------------------------------------------------------------------------
    // Basic interface: access & modification; O(1) operations.
    // List fields access and manipulation methods.
    //-------------------------------------------------------------------------

    inline int size (void) const { return _size; }
    inline ListItem<T> *first (void) const { return _first; }
    inline ListItem<T> *last (void) const { return _last; }
    inline T& operator[] (ListItem<T> *item) { return item->data(); }

    //-------------------------------------------------------------------------
    // Append an item (or a list of items) to a list.
    //-------------------------------------------------------------------------

    inline ListItem<T> *append(ListItem<T> *item) {
	if(_last) _last->setNext(item);
	else      _first = item;
	item->setPrev(_last);
	_last = item;
	_size++;
	return item;
    }
    inline ListItem<T> *append(const T& data) {
	ListItem<T> *ptr = new ListItem<T> (data);
	append(ptr);
	return ptr;
    }
    inline ListItem<T> *append(List& tailList) {
	if(tailList.size()) {
	    if(_last) _last->setNext(tailList.first());
	    else      _first = tailList.first();
	    tailList.first()->setPrev(_last);
	    _last = tailList.last();
	    _size += tailList.size();
	}
	return _first;
    }
    //-------------------------------------------------------------------------
    // Prepend an item (or a list of items) to a list.
    //-------------------------------------------------------------------------

    inline ListItem<T> *prepend(ListItem<T> *item) {
	if(_first) _first->setPrev(item);
	else       _last = item;
	item->setNext(_first);
	_first = item;
	_size++;
	return item;
    }
    inline ListItem<T> *prepend(const T& data) {
	ListItem<T> *ptr = new ListItem<T> (data);
	prepend(ptr);
	return ptr;
    }
    inline ListItem<T> *prepend(List& headList) {
	if(headList.size()) {
	    if(_first) _first->setPrev(headList.last());
	    else       _last = headList.last();
	    headList.last()->setNext(_first);
	    _first = headList.first();
	    _size += headList.size();
	}
	return _first;
    }
    //-------------------------------------------------------------------------
    // Take out an item (or a list of items) from a list. Extract does not
    // delete item, whereas remove does.
    //   Warnings: 1. Extracting a complete list is only consistent if
    //                the specified list is contained in the current list.
    //             2. Iterator should not be active
    //-------------------------------------------------------------------------

    inline void extract(ListItem<T> *item) {
	CHECK(if(!item || !_size || !_first)
	      {if(!item) Warn("Extracting NO item!?");
	       if(!_size) Warn("Extracting from list with NO size!?");
	       if(!_first) Warn("Extracting from list with NULL first!?");
	       Abort("Invalid list management: cannot extract entry");});
	if (_first == item)  {
	    if(_last == item)
		_first = _last = NULL;
	    else {
		_first = item->next();
		_first->_prev = NULL;
	    }
	}
	else if (_last == item)  {
	    _last = item->prev();
	    _last->_next = NULL;
	} else {
	    (item->prev())->_next = item->next();
	    (item->next())->_prev = item->prev();
	}
	item->_next = NULL;
	item->_prev = NULL;
	_size--;
    }
    inline void extract(ListItem<T> *Lfirst, ListItem<T> *Llast, int Lsize) {
	CHECK(if(_size < Lsize)
	      Abort("Invalid list management: cannot extract sub list"););
	if(_first == Lfirst) {
	    if(_last == Llast)
		_first = _last = NULL;
	    else {
		_first = Llast->next();
		_first->_prev = NULL;
	    }
	}
	else if(_last == Llast) {
	    _last = Lfirst->prev();
	    _last->_next = NULL;
	} else {
	    Lfirst->prev()->setNext(Llast->next());
	    Llast->next()->setPrev(Lfirst->prev());
	}
	Llast->setNext(NULL);
	Lfirst->setPrev(NULL);
	_size -= Lsize;
    }
    inline void remove(ListItem<T> *item) {
	extract(item);
	delete item;
    }
    //-------------------------------------------------------------------------
    // Member functions to model operation of a list as a stack.
    //-------------------------------------------------------------------------

    inline List& push (const T& data) { prepend(data); }
    inline List& push (ListItem<T> *item) { prepend(item); }
    inline T pop (void) {
	T rdata = _first->data();
	remove(_first);
	return rdata;
    }
    //-------------------------------------------------------------------------
    // List management.
    //-------------------------------------------------------------------------

    inline void loadList(ListItem<T> *nfirst, ListItem<T> *nlast, int nsize) {
	_first = nfirst; _last = nlast; _size = nsize;
    }
    inline void loadList(List& nlist) {
	_first = nlist.first(); _last = nlist.last(); _size = nlist.size();
    }
    inline void reset() { loadList(NULL, NULL, 0); }

    inline ListItem<T> *insertAfter(ListItem<T> *ref_ptr, ListItem<T> *n_ptr) {
	if(ref_ptr->next()) {
	    ref_ptr->next()->_prev = n_ptr;
	    n_ptr->_next = ref_ptr->next();
	} else {
	    _last = n_ptr;
	    n_ptr->_next = NULL;
	}
	n_ptr->_prev = ref_ptr;
	ref_ptr->_next = n_ptr;
	_size++;
	return n_ptr;
    }
    inline ListItem<T> *insertBefore(ListItem<T> *n_ptr,ListItem<T> *ref_ptr) {
	if(ref_ptr->prev()) {
	    ref_ptr->prev()->_next = n_ptr;
	    n_ptr->_prev = ref_ptr->prev();
	} else {
	    _first = n_ptr;
	    n_ptr->_prev = NULL;
	}
	n_ptr->_next = ref_ptr;
	ref_ptr->_prev = n_ptr;
	_size++;
	return n_ptr;
    }
    //-------------------------------------------------------------------------
    // Additional interface: global list access; O(n) operations.
    //-------------------------------------------------------------------------

    inline void clear(void) { while(_first) extract(_first); }
    inline void erase(void) { while(_first) remove(_first); }

    //-------------------------------------------------------------------------
    // Dump method
    //-------------------------------------------------------------------------

    friend ostream &operator<<(ostream &os, const List& list);
    void dump(void) { cout << *this; }

protected:
    ListItem<T> *_first;	// points to first entry in list
    ListItem<T> *_last;	        // points to last entry in list
    int _size;		        // contains length of list
};


//-----------------------------------------------------------------------------
// Definition of the inlined functions of clasess ListItem and List.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Class methods for List.
//-----------------------------------------------------------------------------

// Basic interface: access & modification; O(1) operations. List fields
// access and manipulation methods.

//
// Operators.
//
//template<class T>
//List<T>& List<T>::operator=(List<T>& _from) {
//    erase();
//    for(ListItem<T> *item = _from.first(); item; item = item->next())
//	append (item->data());
//    return *this;
//}


//
// function:   operator<<
// purpose:    provides stream output for lists
// parameters: none
// returns:    void
// 
template<class T>
ostream &operator<<(ostream &os, const List<T> &list) {
    for(ListItem<T> *item = list.first(); item; item = item->next()) {
	os << item->data() << " ";
    }
    os << endl;
    return os;
}


//-----------------------------------------------------------------------------
// Class definitions for list iterators. The first iterator iterates over the
// elements of a list, whereas the second iterator iterares over the pointers
// of the list.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Class: ListIter
//
// Purpose: Generic definition of a list iterator.
//-----------------------------------------------------------------------------

template <class T>
class ListIter {
public:
    ListIter( List<T> &alist, int direction = FORWARD ) {
	list_ptr = &alist;
	resetIter( direction );
    }
    virtual ~ListIter() {}

    inline void resetIter(int direction = FORWARD ) {
	_direction = direction;
	ptr = (_direction == FORWARD) ? list_ptr->first() : list_ptr->last();
    }

    inline void resetIter( List<T> &alist, int direction = FORWARD ) {
	list_ptr = &alist;
	resetIter( direction );
    }
protected:
    List<T> *list_ptr;   // Pointer to list
    ListItem<T> *ptr;	  // Current entry being accessed
    int _direction;       // Direction of iteration
};

//-----------------------------------------------------------------------------
// Class: ListIterElem
//
// Purpose: Iterates over the elements of a list. NO element can be removed.
//-----------------------------------------------------------------------------

template <class T>
class ListIterElem : public ListIter<T> {
public:
    ListIterElem( List<T> &alist, int direction = FORWARD )
	: ListIter<T>( alist, direction ) { }
    virtual ~ListIterElem() {}

    inline int moreElements() { return( (int) ptr ); }

    inline T nextElement() {
	T retElem = ptr->data();
	ptr = (_direction == FORWARD) ? ptr->next() : ptr->prev();
	return retElem;
    }
};

//-----------------------------------------------------------------------------
// Class: ListIterItem
//
// Purpose: Iterates over the list elements of a list. Current items can be
//           deleted.
//-----------------------------------------------------------------------------

template <class T>
class ListIterItem : public ListIter<T> {
public:
    ListIterItem( List<T> &alist, int direction = FORWARD )
	: ListIter<T>( alist, direction ) { }
    virtual ~ListIterItem() {}

    inline int moreItems() { return( (int) ptr ); }

    inline ListItem<T> *nextItem() {
	ListItem<T> *ret_ptr = ptr;
	ptr = (_direction == FORWARD) ? ptr->next() : ptr->prev();
	return ret_ptr;
    }
};

//-----------------------------------------------------------------------------
// Class: ListIterElemForward and ListIterElemBackward
//
// Purpose: Iterate elements forward and backward.
//-----------------------------------------------------------------------------

template <class T>
class ListIterElemForward : public ListIterElem<T> {
public:
    ListIterElemForward( List<T> &alist )
	: ListIterElem<T>( alist, FORWARD ) { }
    virtual ~ListIterElemForward() {}

    inline T nextElement() {
	register T retElem = ptr->data();
	ptr = ptr->next();
	return retElem;
    }
};

template <class T>
class ListIterElemBackward : public ListIterElem<T> {
public:
    ListIterElemBackward( List<T> &alist )
	: ListIterElem<T>( alist, BACKWARD ) { }
    virtual ~ListIterElemBackward() {}

    inline T nextElement() {
	register T retElem = ptr->data();
	ptr = ptr->prev();
	return retElem;
    }
};

//-----------------------------------------------------------------------------
// Class: ListIterItemForward and ListIterItemBackward
//
// Purpose: Iterate list items forward and backward.
//-----------------------------------------------------------------------------

template <class T>
class ListIterItemForward : public ListIterItem<T> {
public:
    ListIterItemForward( List<T> &alist )
	: ListIterItem<T>( alist, FORWARD ) { }
    virtual ~ListIterItemForward() {}

    inline ListItem<T> *nextItem() {
	register ListItem<T> *ret_ptr = ptr;
	ptr = ptr->next();
	return ret_ptr;
    }
};

template <class T>
class ListIterItemBackward : public ListIterItem<T> {
public:
    ListIterItemBackward( List<T> &alist )
	: ListIterItem<T>( alist, BACKWARD ) { }
    virtual ~ListIterItemBackward() {}

    inline ListItem<T> *nextItem() {
	register ListItem<T> *ret_ptr = ptr;
	ptr = ptr->prev();
	return ret_ptr;
    }
};


//-----------------------------------------------------------------------------
// Macros to iterate over lists. These macros are supposed to lead to more
// efficient code than the iterators above.
//-----------------------------------------------------------------------------

#define for_each(iterV,ListN,typeT) \
for( register ListItem<typeT> *iterV = ListN.first(); iterV; \
    iterV=iterV->next() )

#define while_first(iterV,ListN,typeT) \
register ListItem<typeT> *iterV; \
while( iterV = ListN.first() )

#endif    // _LIST_DEF_

/*****************************************************************************/

