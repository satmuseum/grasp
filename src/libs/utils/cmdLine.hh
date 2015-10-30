//
// file:      CmdLine.h
// purpose:   declarations for command line class
//
#ifndef _CmdLine
#define _CmdLine

#include <iostream.h>

#include "array.hh"
#include "cmdArg.hh"

//
// command line class -- a primitive command parser
//
class CmdLine { 
    enum {MAXARGS = 20};	      // maximum number of cmd arguments
    int _size;			      // actual number of arguments
    Array<CmdArg> args;               // vector of arguments
    static const commandBufferSize;   // size of command text buffer
    static CmdArg dummy;              // used for out-of-range accesses
public:
    //
    // parse a command
    //
    void parse (char *str);
    // 
    // constructors
    //
    CmdLine (int argc, char *argv[]); 
    CmdLine (char *a) : args (MAXARGS), _size (0) {parse (a);}
    CmdLine (void) : args (MAXARGS) , _size (0) {}
    CmdLine (const CmdLine& cmd) : args (cmd.args), _size (cmd._size) {}
    CmdLine& operator= (const CmdLine& cmd) {
	args = cmd.args; _size = cmd._size; return *this;}
    //
    // destructor
    //
    virtual ~CmdLine (void) {}
    //
    // access command arguments
    //
    CmdArg& operator[] (int index);
    int size (void) const {return _size;}
    //
    // stream i/o methods
    //
    friend ostream& operator<< (ostream& stream, CmdLine &command);
    friend istream& operator>> (istream& stream, CmdLine &command);
};

#endif

