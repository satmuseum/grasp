//-----------------------------------------------------------------------------
// File: grp_CNF.hh
//
// Purpose: Class descriptions representing classic CNF formulas (clause
//          databases).
//
// Remarks: --
//
// History: 06/15/95 - JPMS - created.
//          03/09/96 - JPMS - cleaned up interface and readability.
//
// Copyright (c) 1996 Joao P. Marques Silva.
//-----------------------------------------------------------------------------

#ifndef __CNF__
#define __CNF__

#include <iostream.h>

#include "defs.hh"
#include "memory.hh"
#include "list.hh"


//-----------------------------------------------------------------------------
// Class names and typedefs used for simplying interface with CNF formulas.
//-----------------------------------------------------------------------------

class Variable;
class Literal;
class Clause;

typedef Variable *VariablePtr;
typedef List<Variable*> VariableList;
typedef ListItem<Variable*> *VariableListPtr;

typedef Literal *LiteralPtr;
typedef List<Literal*> LiteralList;
typedef ListItem<Literal*> *LiteralListPtr;

typedef Clause *ClausePtr;
typedef List<Clause*> ClauseList;
typedef ListItem<Clause*> *ClauseListPtr;



//-----------------------------------------------------------------------------
// Class: Variable
//
// Purpose: Class declaration of variables of a CNF formula.
//-----------------------------------------------------------------------------

class Variable {
  public:

    //-------------------------------------------------------------------------
    // Constructor / desctructor.
    //-------------------------------------------------------------------------

    Variable (char *Nname, int nID) : _literals() {
	_name = Nname; _ID = nID;
	_pos_literals = _neg_literals = 0;
	_cnf_ref = new ListItem<Variable*> (this);
    }
    virtual ~Variable () {
	CHECK(if (_literals.size() )Warn("Deleting variable with literals?"););
	delete _cnf_ref;
    }

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    inline char *name() { return _name; }
    inline LiteralList &literals() { return _literals; }

    inline void add_literal (LiteralListPtr plit);
    inline void del_literal (LiteralListPtr plit);

    inline int ID() { return _ID; }

    inline VariableListPtr cnf_ref() { return _cnf_ref; }

    inline int positive_literals() { return _pos_literals; }
    inline int negative_literals() { return _neg_literals; }

    //-------------------------------------------------------------------------
    // Memory management of objects of class Variable.
    //-------------------------------------------------------------------------

    MEM_MNG_DECL(Variable, "Variable")

  protected:
    VariableListPtr _cnf_ref;     // Var ref for its existence in CNF formula

    char *_name;                  // Name of variable
    int _ID;                      // ID reference of variable

    LiteralList _literals;        // List of literals associated with variable

    int _pos_literals;
    int _neg_literals;

  private:

};


//-----------------------------------------------------------------------------
// Class: Literal
//
// Purpose: A literal is associated with a variable and the specifies a sign
//          (either positive or negative.
//-----------------------------------------------------------------------------

class Literal {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    Literal (ClausePtr Ncl, VariablePtr Nvar, int Nsign ) {
	_clause = Ncl; _variable = Nvar; _sign = Nsign;

	_var_ref = new ListItem<Literal*> (this);
	_cl_ref = new ListItem<Literal*> (this);
    }
    virtual ~Literal() {
	delete _var_ref;     // Refs must have been taken from vars and clauses
	delete _cl_ref;
    }
    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    inline LiteralListPtr &variable_ref() { return _var_ref; }
    inline LiteralListPtr &clause_ref() { return _cl_ref; }
    inline VariablePtr &variable() { return _variable; }
    inline ClausePtr &clause() { return _clause; }
    inline int &sign() { return _sign; }

    //-------------------------------------------------------------------------
    // Memory management of objects of class Literal.
    //-------------------------------------------------------------------------

    MEM_MNG_DECL(Literal, "Literal")

  protected:
    LiteralListPtr _var_ref;       // Reference for this literal in variable
    LiteralListPtr _cl_ref;        // Reference for this literal in clause

    VariablePtr _variable;         // Variable associated with literal
    ClausePtr _clause;             // Clause containing literal

    int      _sign;     // Indicates whether literal is positive or negative

  private:

};


//-----------------------------------------------------------------------------
// Class: Clause
//
// Purpose: A clause is defined as a set of literals.
//-----------------------------------------------------------------------------

class Clause {
  public:

    //-------------------------------------------------------------------------
    // Constructor/destructor.
    //-------------------------------------------------------------------------

    Clause () : _literals() { _cnf_ref = new ListItem<Clause*> (this); }
    virtual ~Clause() {
	CHECK(if (_literals.size())Warn("Deleting clause with literals?"););
	delete _cnf_ref;
    }

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    LiteralList &literals() { return _literals; }
    void add_literal( LiteralListPtr plit ) { _literals.append( plit ); }
    void del_literal( LiteralListPtr plit ) { _literals.extract( plit ); }

    inline ClauseListPtr cnf_ref() { return _cnf_ref; }

    //-------------------------------------------------------------------------
    // Dumping clause contents
    //-------------------------------------------------------------------------
    
    virtual void dump (ostream &outs) {
	outs<<"clause = ( ";
	for_each(plit,_literals,LiteralPtr) {
	    LiteralPtr lit = plit->data();
	    if (lit->sign()) outs<<"-";
	    outs<<lit->variable()->name()<<" ";
	}
	outs<<")";
    }

    //-------------------------------------------------------------------------
    // Memory management of objects of class Clause.
    //-------------------------------------------------------------------------

    MEM_MNG_DECL(Clause, "Clause")

  protected:
    LiteralList _literals;
    ClauseListPtr _cnf_ref;          // Ref indicating existence in CNF formula

  private:

};


//-----------------------------------------------------------------------------
// Class: ClauseDatabase
//
// Purpose: A clause database is defined as a set of clauses and respective
//          variables.
//-----------------------------------------------------------------------------

class ClauseDatabase {
  public:

    //-------------------------------------------------------------------------
    // Constructor/desctructor.
    //-------------------------------------------------------------------------

    ClauseDatabase() : _variables(), _clauses() { _varIDs = 0; }
    virtual ~ClauseDatabase() { _variables.erase(); _clauses.erase(); }

    virtual void dump (ostream &outs = cout);

    //-------------------------------------------------------------------------
    // Interface contract.
    //-------------------------------------------------------------------------

    inline VariableList &variables() { return _variables; }
    inline ClauseList &clauses() { return _clauses; }

    inline VariablePtr add_variable (char *vname = NULL) {
	VariablePtr var = new Variable (vname, _varIDs++);
	_variables.append (var->cnf_ref());
	return var;
    }
    inline void del_variable (VariablePtr var) {
	for_each(plit,var->literals(),LiteralPtr) {
	    del_literal (plit->data());
	}
	_variables.extract (var->cnf_ref());
	delete var;
    }
    inline ClausePtr add_clause() {
	ClausePtr cl = new Clause();
	_clauses.append (cl->cnf_ref());
	return cl;
    }
    inline void del_clause (ClausePtr cl) {
	for_each(plit,cl->literals(),LiteralPtr) {
	    del_literal (plit->data());
	}
	_clauses.extract (cl->cnf_ref());
	delete cl;
    }
    inline LiteralPtr add_literal (ClausePtr cl, VariablePtr var, int sign) {
	LiteralPtr lit = new Literal (cl, var, sign);
	var->add_literal (lit->variable_ref());
	cl->add_literal (lit->clause_ref());
	return lit;
    }
    inline void del_literal (LiteralPtr lit) {
	VariablePtr var = lit->variable();
	ClausePtr cl = lit->clause();
	var->del_literal (lit->variable_ref());
	cl->del_literal (lit->clause_ref());
	delete lit;
    }

  protected:
    VariableList _variables;    // List of variables used in the CNF formula
    ClauseList _clauses;        // List of clauses

    int _varIDs;
};


//-----------------------------------------------------------------------------
//
// Member function definition for functions requiring a posteriori definition.
//
//-----------------------------------------------------------------------------

inline void Variable::add_literal (LiteralListPtr plit)
{
    _literals.append (plit);
    if (plit->data()->sign()) { _pos_literals++; }
    else                      { _neg_literals++; }
}

inline void Variable::del_literal (LiteralListPtr plit)
{
    if (plit->data()->sign()) { _pos_literals--; }
    else                      { _neg_literals--; }
    _literals.extract (plit);
}

#endif // __CNF__

/*****************************************************************************/
