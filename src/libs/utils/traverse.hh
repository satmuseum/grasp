//-----------------------------------------------------------------------------
// File: traverse.h
//
// Purpose: Class definitions for procedures implementing different forms
//          of implicit graph traversals: levelized breadth-first and
//          depth-first. Traversals are performed on an abstract graph
//          structure implicitly defined by ordering or scheduling
//          information. No notion of structure is assumed, and scheduling
//          operations are coordinated by the accessing methods to the class.
//
// Remarks: Methods defined inside class declaration because of multiple
//          definitions. By using self references to IDs, we avoid the
//          overhead of creating/deleting list items to manipulate traversals.
//          Exceptions are the children of nodes in DF traversals, because
//          each such node may be the child of several other nodes.
//
// History: 5/18/94 - JPMS - first version.
//          7/10/95 - JPMS - simplified and unified usage.
//          1/18/96 - JPMS - modified: proper inlining of inlined functions
//                           requires joint declaration and definition
//                           (in gcc 2.7.2)
//-----------------------------------------------------------------------------

#ifndef _TRAVERSE_ALGS
#define _TRAVERSE_ALGS

#include <iostream.h>

#include "defs.hh"
#include "array.hh"
#include "list.hh"


class LBF_NodeTraversalInfo;
class DF_NodeTraversalInfo;


//-----------------------------------------------------------------------------
//
// Class definitions for several traversal engines. The definition of the
// (inlined) member functions is given below. Each engine also associates
// dedicated information with each node that can be visited.
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Class: LBF_NodeTraversalInfo
//
// Purpose: Information associated with each node being traversed in an LBF
//          traversal.
//-----------------------------------------------------------------------------

class LBF_NodeTraversalInfo {
public:

    //-------------------------------------------------------------------------
    // Constructor/destructor
    //-------------------------------------------------------------------------

    LBF_NodeTraversalInfo( int nID ) {
	_ID = nID;
	_extRef = NULL;
	setRef( this );
    }
    ~LBF_NodeTraversalInfo() {}

    //-------------------------------------------------------------------------
    // Status testing/modifying methods.
    //-------------------------------------------------------------------------
    
    inline void unmark() { _extRef = NULL; }
    inline void schedule( void *nref ) { _extRef = nref; }

    inline int unmarked() { return( _extRef == NULL  ); }
    inline int scheduled() { return( _extRef != NULL ); }

    //-------------------------------------------------------------------------
    // Access/modifying methods.
    //-------------------------------------------------------------------------
    
    inline int ID() { return _ID; }

    inline void *extRef() { return _extRef; }

    inline ListItem<LBF_NodeTraversalInfo*> *pnode() { return _pnode; }

protected:
    int _ID;
    void *_extRef;
    ListItem<LBF_NodeTraversalInfo*> *_pnode;

private:
    void setRef( LBF_NodeTraversalInfo *selfRef ) {
	_pnode = new ListItem<LBF_NodeTraversalInfo*> (selfRef);
    }
};


//-----------------------------------------------------------------------------
// Class: DF_NodeTraversalInfo
//
// Purpose: Information associated with each node being traversed.
//-----------------------------------------------------------------------------

enum { White = 100, Gray = 101, Black = 102 };


class DF_NodeTraversalInfo {
public:

    //-------------------------------------------------------------------------
    // Constructor/destructor
    //-------------------------------------------------------------------------

    DF_NodeTraversalInfo( int nID ) {
	_ID = nID;
	_status = White;
	_extRef = NULL;
	setRef( this );
    }
    ~DF_NodeTraversalInfo() {}

    //-------------------------------------------------------------------------
    // Status testing/modifying methods.
    //-------------------------------------------------------------------------
    
    inline void unmark() { _status = White; }
    inline void schedule() { _status = Gray; }
    inline void visit() { _status = Black; }
    
    inline int unmarked() { return( _status == White ); }
    inline int scheduled() { return( _status == Gray ); }
    inline int visited() { return( _status == Black ); }

    //-------------------------------------------------------------------------
    // Access/modifying methods.
    //-------------------------------------------------------------------------
    
    inline int ID() { return _ID; }

    inline int status() { return _status; }
    inline int status( int nstat ) { _status = nstat; return _status; }

    inline void *extRef() { return _extRef; }
    inline void *extRef( void *nref ) { _extRef = nref; return _extRef; }

    inline ListItem<DF_NodeTraversalInfo*> *pnode() { return _pnode; }

protected:
    int _ID;
    int _status;
    void *_extRef;
    ListItem<DF_NodeTraversalInfo*> *_pnode;

private:
    void setRef( DF_NodeTraversalInfo *selfRef ) {
	_pnode = new ListItem<DF_NodeTraversalInfo*> (selfRef);
    }
};


//-----------------------------------------------------------------------------
// Class: LBF_TraverseForward
//
// Purpose: Engine for forward levelized breadth-first traversals (mimics
//          the previous traversal engine, but not implemented yet).
//-----------------------------------------------------------------------------

class LBF_TraverseForward {
public:

    //-------------------------------------------------------------------------
    // Constructors/destructors.
    //-------------------------------------------------------------------------

    LBF_TraverseForward() : LBF(0), _nodeArray(0) {
	totID = totLevel = 0;
	traverseActive = FALSE;

	currNode = frstNode = NULL;
	currRef = NULL;

	currWidth = 0;
	currLevel = minLevel = NONE;
    }
    LBF_TraverseForward( int _totID, int _totLevel ) : LBF(0), _nodeArray(0) {
	initialize( _totID, _totLevel );
	traverseActive = FALSE;

	currNode = frstNode = NULL;
	currRef = NULL;

	currWidth = 0;
	currLevel = minLevel = NONE;
    }
    virtual ~LBF_TraverseForward() { erase(); }

    //-------------------------------------------------------------------------
    // Erase structures.
    //-------------------------------------------------------------------------

    inline void erase() {
	CHECK(if(traverseActive)
	      Warn("Invalid erase invocation: traversal being performed"););
	_nodeArray.resize(0);
	LBF.resize(0);
	totID = totLevel = 0;
    }

    //-------------------------------------------------------------------------
    // Initialization/resizing.
    //-------------------------------------------------------------------------

    inline void initialize(int _totID, int _totLevel) {
	CHECK(if(_totLevel <= 0 || _totID <= 0)
	      {cout<<"\t-> _totID is: "<<_totID;
	       cout<<" and _totLevel is: "<<_totLevel<<endl;
	       Warn("Improper definition of LBF parameters");});

	LBF.resize( _totLevel );
	_nodeArray.resize( _totID );
	for( register int k = 0; k < _totID; k++ ) {
	    _nodeArray[k] = new LBF_NodeTraversalInfo(k);
	}
	totID = _totID;
	totLevel = _totLevel;
    }
    inline void resize(int _totID, int _totLevel) {
	if( _totLevel > totLevel ) {
	    LBF.resize( _totLevel );
	    totLevel = _totLevel;
	}
	if( _totID > totID ) {
	    _nodeArray.resize( _totID );
	    for( register int k = totID; k < _totID; k++ ) {
		_nodeArray[k] = new LBF_NodeTraversalInfo(k);
	    }
	    totID = _totID;
	}
    }
    //-------------------------------------------------------------------------
    // Traversal methods: setup traversal conditions; start traversal;
    // schedule nodes to be traversed; current width of traversal; current
    // reference to node being visited; iterating to next node to be visited;
    // stopping the traversal.
    //-------------------------------------------------------------------------

    inline void setup() {                             // Prepare next traversal
	CHECK(if(traverseActive) Warn("Setting mode in active traversal!?");
	      if(currLevel != NONE)
	      Warn("Invalid initial traversal level for setup!?"););
	traverseActive = FALSE;
	currLevel = minLevel = totLevel + 1;
	currNode = frstNode = NULL;
	currWidth = 0;
    }
    inline void start() {                                 // Initiate traversal
	CHECK(if(totLevel <= 0 || totID <= 0)
	      {cout<<"\t-> totID is: "<<totID<<" and totLevel is: ";
	       cout<<totLevel<<endl;
	       Warn("Improper definition of LBF parameters");}
	      if(traverseActive) Warn("Starting active traversal!?"););
	traverseActive = TRUE;
	currLevel = minLevel;
	if( currNode = frstNode ) {
	    currRef = currNode->extRef();
	}
    }
    inline void stop() {                         // Terminate current traversal
	CHECK(if(!traverseActive) Abort("Stopping inactive LBF!?"););
	while( currWidth ) {
	    register ListItem<LBF_NodeTraversalInfo*> *pnode;
	    while( pnode = LBF[currLevel].first() ) {
		pnode->data()->unmark();
		currWidth--;
		LBF[currLevel].extract( pnode );
	    }
	    currLevel++;
	}
	traverseActive = FALSE;
	currLevel = NONE;
	CHECK(if(currWidth != 0)Warn("Invalid width management in LBF!?"););
    }

    inline int width() { return currWidth; }
    inline void *currentRef() { return currRef; }

    inline void schedule(void *nref, int nID, int nlevel ) {
	CHECK(if(nlevel < 0 || nlevel > totLevel)
	      {cout<<"\t-> level is: "<<nlevel<<" w/ totLevel: ";
	       cout<<totLevel<<endl;Abort("Invalid range for level");}
	      if(nID < 0 || nID >= totID)
	      {cout<<"\t-> ID is: "<<nID<<" w/ totID: "<<totID<<endl;
	       Abort("Invalid range for ID");}
	      if(traverseActive && nlevel < currLevel)
	      {cout<<"CurrLevel: "<<currLevel<<" w/SchedLevel: "<<nlevel<<endl;
	       Warn("Level below current level in forward LBF");});

	register LBF_NodeTraversalInfo *node = _nodeArray[nID];
	if( !node->scheduled() ) {
	    node->schedule( nref );
	    LBF[nlevel].append( node->pnode() );

	    currWidth++;
	    if( nlevel < minLevel ) {
		minLevel = nlevel;
		frstNode = node;
	    }
	}
    }
    inline void *iterate() {                   // Visit next node in traversal
	CHECK(if(!traverseActive || currLevel<0 || currLevel>totLevel)
	      Abort("Iterating inactive or empty LBF!?"););

	currNode->unmark();
	LBF[currLevel].extract( currNode->pnode() );

	if( --currWidth ) {
	    CHECK(if(currWidth < 0)Abort("Invalid width for LBF"););

	    register ListItem<LBF_NodeTraversalInfo*> *pnode =
		LBF[currLevel].first();
	    if( !pnode ) {
		while( !(pnode = LBF[++currLevel].first()) ) ;
	    }
	    currNode = pnode->data();
	    currRef = currNode->extRef();
	} else {
	    currNode = NULL;
	    currRef = NULL;
	}
    }
protected:
    Array<List<LBF_NodeTraversalInfo*> > LBF;             // Level BF structure
    int currWidth;                                  // Current traversal width
    int currLevel;

    //-------------------------------------------------------------------------
    // Information regarding current node being visited.
    //-------------------------------------------------------------------------

    LBF_NodeTraversalInfo *currNode;
    void *currRef;

    //-------------------------------------------------------------------------
    // Sets of nodes associated with the graph to be traversed. Each node is
    // uniquely identifies by an ID, which is used to address the node in the
    // array.
    //-------------------------------------------------------------------------

    Array<LBF_NodeTraversalInfo*> _nodeArray;

    //-------------------------------------------------------------------------
    // Control of traversal and startup conditions.
    //-------------------------------------------------------------------------

    int traverseActive;

    int minLevel;
    LBF_NodeTraversalInfo *frstNode;

    //-------------------------------------------------------------------------
    // Definition of the size of possible LBF traversals.
    //-------------------------------------------------------------------------

    int totID;
    int totLevel;
};


//-----------------------------------------------------------------------------
// Class: LBF_TraverseBackward
//
// Purpose: Engine for levelized breadth-first traversals.
//-----------------------------------------------------------------------------

class LBF_TraverseBackward {
public:

    //-------------------------------------------------------------------------
    // Constructors/destructors.
    //-------------------------------------------------------------------------

    LBF_TraverseBackward() : LBF(0), _nodeArray(0) {
	totID = totLevel = 0;
	traverseActive = FALSE;

	currNode = frstNode = NULL;
	currRef = NULL;

	currWidth = 0;
	currLevel = maxLevel = NONE;
    }
    LBF_TraverseBackward( int _totID, int _totLevel ) : LBF(0), _nodeArray(0) {
	initialize( _totID, _totLevel );
	traverseActive = FALSE;

	currNode = frstNode = NULL;
	currRef = NULL;

	currWidth = 0;
	currLevel = maxLevel = NONE;
    }
    virtual ~LBF_TraverseBackward() {
	erase();
    }
    //-------------------------------------------------------------------------
    // Erase structures.
    //-------------------------------------------------------------------------

    inline void erase() {
	CHECK(if(traverseActive)
	      Warn("Invalid erase invocation: traversal being performed"););
	_nodeArray.resize(0);
	LBF.resize(0);
	totID = totLevel = 0;
    }
    //-------------------------------------------------------------------------
    // Initialization/resizing.
    //-------------------------------------------------------------------------

    inline void initialize(int _totID, int _totLevel) {
	CHECK(if(_totLevel <= 0 || _totID <= 0)
	      {cout<<"\t-> _totID is: "<<_totID;
	       cout<<" and _totLevel is: "<<_totLevel<<endl;
	       Warn("Improper definition of LBF parameters");});

	LBF.resize( _totLevel );
	_nodeArray.resize( _totID );
	for( register int k = 0; k < _totID; k++ ) {
	    _nodeArray[k] = new LBF_NodeTraversalInfo(k);
	}
	totID = _totID;
	totLevel = _totLevel;
    }
    inline void resize(int _totID, int _totLevel) {
	if( _totLevel > totLevel ) {
	    LBF.resize( _totLevel );
	    totLevel = _totLevel;
	}
	if( _totID > totID ) {
	    _nodeArray.resize( _totID );
	    totID = _totID;
	}
    }
    //-------------------------------------------------------------------------
    // Traversal methods: setup traversal conditions; start traversal;
    // schedule nodes to be traversed; current width of traversal; current
    // reference to node being visited; iterating to next node to be visited;
    // stopping the traversal.
    //-------------------------------------------------------------------------

    inline void setup() {                             // Prepare next traversal
	CHECK(if(traverseActive) Warn("Setting mode in active traversal!?");
	      if(currLevel != NONE)
	      Warn("Invalid initial traversal level for setup!?"););
	traverseActive = FALSE;
	currLevel = maxLevel = -1;
	currNode = frstNode = NULL;
	currWidth = 0;
    }
    inline void start() {                                 // Initiate traversal
	CHECK(if(totLevel <= 0 || totID <= 0)
	      {cout<<"\t-> totID is: "<<totID<<" and totLevel is: ";
	       cout<<totLevel<<endl;
	       Warn("Improper definition of LBF parameters");}
	      if(traverseActive) Warn("Starting active traversal!?"););
	traverseActive = TRUE;
	currLevel = maxLevel;
	if( currNode = frstNode ) {
	    currRef = currNode->extRef();
	}
    }
    inline void stop() {                         // Terminate current traversal
	CHECK(if(!traverseActive) Abort("Stopping inactive LBF!?"););

	while( currWidth ) {
	    register ListItem<LBF_NodeTraversalInfo*> *pnode;
	    while( pnode = LBF[currLevel].first() ) {
		pnode->data()->unmark();
		currWidth--;
		LBF[currLevel].extract( pnode );
	    }
	    currLevel--;
	}
	traverseActive = FALSE;
	currLevel = NONE;
	CHECK(if(currWidth != 0)Warn("Invalid width management in LBF!?"););
    }
    inline int width() { return currWidth; }
    inline void *currentRef() { return currRef; }

    inline void schedule(void *nref, int nID, int nlevel ) {
	CHECK(if(nlevel < 0 || nlevel > totLevel)
	      {cout<<"\t-> level is: "<<nlevel<<" w/ totLevel: ";
	       cout<<totLevel<<endl;Abort("Invalid range for level");}
	      if(nID < 0 || nID >= totID)
	      {cout<<"\t-> ID is: "<<nID<<" w/ totID: "<<totID<<endl;
	       Abort("Invalid range for ID");}
	      if(traverseActive && nlevel > currLevel)
	      {cout<<"CurrLevel: "<<currLevel<<" w/SchedLevel: "<<nlevel<<endl;
	       Warn("Level above current level in backward LBF");});

	register LBF_NodeTraversalInfo *node = _nodeArray[nID];
	if( !node->scheduled() ) {
	    node->schedule( nref );
	    LBF[nlevel].append( node->pnode() );
	    currWidth++;
	    if( nlevel > maxLevel ) {
		maxLevel = nlevel;
		frstNode = node;
	    }
	}
    }
    inline void *iterate() {               // Access next element to be visited
	CHECK(if(!traverseActive || currLevel<0 || currLevel>totLevel)
	      Abort("Iterating inactive or empty LBF!?"););

	currNode->unmark();
	LBF[currLevel].extract( currNode->pnode() );

	if( --currWidth ) {
	    CHECK(if(currWidth < 0)Abort("Invalid width for LBF"););

	    register ListItem<LBF_NodeTraversalInfo*> *pnode =
		LBF[currLevel].first();
	    if( !pnode ) {
		while( !(pnode = LBF[--currLevel].first()) ) ;
	    }
	    currNode = pnode->data();
	    currRef = currNode->extRef();
	} else {
	    currNode = NULL;
	    currRef = NULL;
	}
    }
protected:
    Array<List<LBF_NodeTraversalInfo*> > LBF;             // Level BF structure
    int currWidth;                                  // Current traversal width
    int currLevel;

    //-------------------------------------------------------------------------
    // Information regarding current node being visited.
    //-------------------------------------------------------------------------

    LBF_NodeTraversalInfo *currNode;
    void *currRef;

    //-------------------------------------------------------------------------
    // Sets of nodes associated with the graph to be traversed. Each node is
    // uniquely identifies by an ID, which is used to address the node in the
    // array.
    //-------------------------------------------------------------------------

    Array<LBF_NodeTraversalInfo*> _nodeArray;

    //-------------------------------------------------------------------------
    // Control of traversal and startup conditions.
    //-------------------------------------------------------------------------

    int traverseActive;

    int maxLevel;
    LBF_NodeTraversalInfo *frstNode;

    //-------------------------------------------------------------------------
    // Definition of the size of possible LBF traversals.
    //-------------------------------------------------------------------------

    int totID;
    int totLevel;
};


//-----------------------------------------------------------------------------
// Class: DF_TraverseEngine
//
// Purpose: Engine for depth first traversals.
//-----------------------------------------------------------------------------

class DF_TraverseEngine {
public:

    //-------------------------------------------------------------------------
    // Constructors/destructors.
    //-------------------------------------------------------------------------
    
    DF_TraverseEngine() : DF(), _visitedNodes(), _nodeArray(0) {
	traverseActive = FALSE;

	currNode = NULL;
	currRef = NULL;

	totID = 0;
    }
    DF_TraverseEngine(int _totID) : DF(), _visitedNodes(), _nodeArray(0) {
	initialize( _totID );
	traverseActive = FALSE;

	currNode = NULL;
	currRef = NULL;

	totID = _totID;
    }
    virtual ~DF_TraverseEngine() { erase(); }

    //-------------------------------------------------------------------------
    // Clears structures.
    //-------------------------------------------------------------------------

    inline void erase() {
	CHECK(if(traverseActive)
	      Warn("Invalid erase invocation. DF traversal being performed"););
	_nodeArray.resize(0);
    }

    //-------------------------------------------------------------------------
    // Initialization/resizing.
    //-------------------------------------------------------------------------

    inline void initialize(int _totID) {
	_nodeArray.resize( _totID );
	for( register int k = 0; k < _totID; k++ ) {
	    _nodeArray[k] = new DF_NodeTraversalInfo(k);
	}
	totID = _totID;
    }
    inline void resize(int _totID) { initialize( _totID ); }

    //-------------------------------------------------------------------------
    // Traversal methods.
    //-------------------------------------------------------------------------

    inline void start( void *nref, int nID ) {
	CHECK(if(traverseActive || nID < 0 || nID > totID)
	      {cout<<"traverseActive: "<<traverseActive<<endl;
	       cout<<"totID: "<<totID<<"and nID: "<<nID<<endl;
	       Abort("Invalid conditions to start DF traversal");});
	traverseActive = TRUE;

	DF_NodeTraversalInfo *node = _nodeArray[nID];
	node->schedule();
	node->extRef( nref );
	DF.prepend( node->pnode() );

	currNode = node;
	currRef = nref;
    }
    inline void start() {
	CHECK(if(traverseActive)
	      {cout<<"traverseActive: "<<traverseActive<<endl;
	       Abort("Invalid conditions to start DF traversal");}
	      if(!DF.first()->data())Abort("NO scheduled elements??"););

	traverseActive = TRUE;
	currNode = DF.first()->data();
	currRef = currNode->extRef();
    }
    inline void stop() {
	register ListItem<DF_NodeTraversalInfo*> *pnode;
	register DF_NodeTraversalInfo *node;

	while( pnode = DF.first() ) {
	    node = pnode->data();
	    DF.extract( pnode );
	    node->unmark();
	}
	while( pnode = _visitedNodes.first() ) {
	    node = pnode->data();
	    _visitedNodes.extract( pnode );
	    node->unmark();
	}
	traverseActive = FALSE;
    }
    inline void schedule( void *nref, int nID ) {
	CHECK(if(nref == currRef) Abort("Cycle condition in DF scheduling?"););

	register DF_NodeTraversalInfo *node = _nodeArray[nID];
	if( node->unmarked() ) {
	    node->schedule();
	    node->extRef( nref );
	    DF.prepend( node->pnode() );
	} else if( node->scheduled() ) {
	    node->schedule();
	    node->extRef( nref );
	    DF.extract( node->pnode() );
	    DF.prepend( node->pnode() );
	}
    }

    inline int height() { return DF.size(); }
    inline void *currentRef() { return currRef; }

    inline int iterate() {
	currNode->visit();
	DF.extract( currNode->pnode() );
	_visitedNodes.append( currNode->pnode() );
	if( DF.first() ) {
	    currNode = DF.first()->data();
	    currRef = currNode->extRef();
	} else {
	    currNode = NULL;
	    currRef = NULL;
	}
    }
protected:

    //-------------------------------------------------------------------------
    // Stack of nodes to be visited.
    //-------------------------------------------------------------------------

    List<DF_NodeTraversalInfo*> DF;

    //-------------------------------------------------------------------------
    // Sets of nodes associated with the graph to be traversed. Each node is
    // uniquely identifies by an ID, which is used to address the node in the
    // array.
    //-------------------------------------------------------------------------

    Array<DF_NodeTraversalInfo*> _nodeArray;

    //-------------------------------------------------------------------------
    // Nodes visited, whose marks must be cleared in the end.
    //-------------------------------------------------------------------------

    List<DF_NodeTraversalInfo*> _visitedNodes;

    //-------------------------------------------------------------------------
    // Information regarding the node currently being visited.
    //-------------------------------------------------------------------------

    DF_NodeTraversalInfo *currNode;
    void *currRef;

    //-------------------------------------------------------------------------
    // Control of traversal / size of graph.
    //-------------------------------------------------------------------------

    int traverseActive;
    int totID;
};

#endif

/*****************************************************************************/

