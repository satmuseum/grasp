//-----------------------------------------------------------------------------
// File: symbTable.hh
//
// Purpose: Implementation of a symbol table. 
//
// Remarks: This is a very crude implementation. Optimizations should
//          be developed.
//
// History: 7/6/95 - JPMS - created (borrows extensively from Tim Burks'
//                          SymbolTable, but it is much more crude).
//-----------------------------------------------------------------------------

#ifndef _symbTable
#define _symbTable

#include <iostream.h>
#include <string.h>

#include "defs.hh"
#include "array.hh"
#include "list.hh"



//-----------------------------------------------------------------------------
// class: 	SymbolTable
// purpose:	collection of named objects.  Data is stored in a hash table.
//-----------------------------------------------------------------------------

template <class T>
class SymbolTable {
public:

    //-------------------------------------------------------------------------
    // constructors and destructors
    //-------------------------------------------------------------------------

    SymbolTable (int bins) : _names( bins ), _entries( bins ), _size (0) {}
    virtual ~SymbolTable();

    //-------------------------------------------------------------------------
    // table access methods
    //-------------------------------------------------------------------------

    inline T lookup( char *name );
    inline void insert( T entry, char *name );
    inline void remove( T entry, char *name );
    inline void clear();
    inline int size() { return _size; }
    inline void dump();

protected:

    Array<List<char*> > _names;                  // Stores names added to table
    Array<List<T> > _entries;       // Stores entries associated with each name
    int _size;                             // Number of objects stored in table

private:

    //-------------------------------------------------------------------------
    // hashing function
    //-------------------------------------------------------------------------

    int computeHash( char *name ) {
	register int value = 0;
	for( register char *p = name; *p; p++ ) {
	    value += *p;
	}
	return value;
    }
};

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------

template<class T>
SymbolTable<T>::~SymbolTable (void)
{
    CHECK(
        // report statistics on symbol table usage

        int min = 32767;
        int max = 0;
        for (int i = 0; i < _names.size(); i++) {
	    if (_names[i].size() < min)
	        min = _names[i].size();
	    if (_names[i].size() > max)
	        max = _names[i].size();
        }
        cout << "symbol table statistics\n";
        cout << "  average bucket size: " << 1.0*_size/_names.size() << "\n";
        cout << "  minimum bucket size: " << min << "\n";
        cout << "  maximum bucket size: " << max << "\n";
    )
}

//-----------------------------------------------------------------------------
// Function: lookup
//
// Purpose: Checks whether a given name/entry is in the symbol table.
//-----------------------------------------------------------------------------

template<class T>
T SymbolTable<T>::lookup( char *lk_name )
{
    register int hash_val = computeHash(lk_name);
    register ListItem<char*> *p_name = _names[hash_val].first();
    register ListItem<T> *p_entry = _entries[hash_val].first();
    while( p_name ) {
	if( !strcmp(p_name->data(), lk_name) )
	    return p_entry->data();
	else  {
	    p_entry = p_entry->next();
	    p_name = p_name->next();
	}
    }
    return NULL;
}

//-----------------------------------------------------------------------------
// Function: insert
//
// Purpose: Adds a new entry/name to symbol table. Assumes that space for the
//          name has been allocated elsewhere.
//-----------------------------------------------------------------------------

template<class T>
void SymbolTable<T>::insert( T entry, char *name )
{
    register int index = computeHash( name );
    _names[index].append( name );
    _entries[index].append( entry );
    _size++;
}

//-----------------------------------------------------------------------------
// Function: remove
//
// Purpose: Deletes an entry from the symbol table.
//-----------------------------------------------------------------------------

template<class T>
void SymbolTable<T>::remove( T entry, char *name )
{
    register int index = computeHash( name );
    register ListItem<char*> *p_name = _names[index].first();
    register ListItem<T> *p_entry = _entries[index].first();
    while( p_entry && p_entry->data() != entry ) {
	p_entry = p_entry->next();
	p_name = p_name->next();
    }
    CHECK(if(!p_name)Abort("UNABLE to delete entry from symbol table??"););
    _names[index].remove( p_name );
    _entries[index].remove( p_entry );
    _size--;
}

//-----------------------------------------------------------------------------
// Function: clear
//
// Purpose: Deletes all entries from symbol table.
//-----------------------------------------------------------------------------

template<class T>
void SymbolTable<T>::clear (void)
{
    for( register int k = 0; k < _names.size(); k++ ) {    // empty each bucket
	_names[k].erase();
	_entries[k].erase();
    }
    _size = 0;
}

//-----------------------------------------------------------------------------
// Function: dump
//
// Purpose: Outputs contents of symbol table.
//-----------------------------------------------------------------------------

template<class T>
void SymbolTable<T>::dump()
{
    cout << "    Table size:" << size() << "\n";
    for( register int k = 0; k < _names.size(); k++) {
	ListItem<char*> *p_name = _names[k].first();
	for( ; p_name; p_name = p_name->next() ) {
	    cout << k << ": " << p_name->data() << endl;
	}
    }
}

#endif

/*****************************************************************************/

