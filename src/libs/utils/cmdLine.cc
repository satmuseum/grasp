//
// file:      CmdLine.C
// purpose:   code declarations for command line class
//
#include <iostream.h>
#include <string.h>

#include "defs.hh"
#include "cmdLine.hh"

const int CmdLine::commandBufferSize = 1024;
CmdArg CmdLine::dummy;

//
// parse a character string into a vector of commands
//
void CmdLine::parse (char *str) {
    //
    // parse tokens into separate arguments
    //
    DBGn(cout<<"\t-> Processing string: "<<str<<endl;);
    char buf[256];
    strncpy (buf,str,256);
    char *c = strtok (buf, " ");
    if(!c) c = strtok(0,"\n");
    if (c) {
	_size = 1;
	args [0] = c;
	DBGn(cout<<"\t-> String read: "<<args[0].string()<<endl;);
	while ((c = strtok (0, " ")) || (c = strtok(0,"\n"))) {
	    if (_size == MAXARGS) {
		cout << "Too many command arguments.  "
		    << "Increase MAXARGS in CmdLine.h" << endl;
		return;
	    }
	    args[_size++] = c;
	    DBGn(cout<<"\t-> String read: "<<args[_size-1].string()<<endl;);
	}
    } else {
	_size = 0;
    }
    //
    // zero out remaining arguments (for safety)
    //
    for (int i = _size; i < MAXARGS; i++)
        args[i] = "";
    DBGn(cout<<"\t-> String processed!"<<endl;);
}

//
// constructor (from program command line)
//
CmdLine::CmdLine (int argc, char *argv[]) : args (argc) {
    for (int i = 0; i < argc; i++)
	args[i] = argv[i];
}

//
// access an individual command argument
//
CmdArg& CmdLine::operator[] (int index) {
    if ((index >= args.size()) || (index < 0))
	return dummy;
    else
        return args[index];
}

//
// stream output method
//
ostream& operator<< (ostream& stream, CmdLine &command) {
    if (command.size()) {
    	for (int i = 0; i < command.size(); i++) {
	    if (i != 0) stream << " ";
	    stream << command[i].string();
    	}
    } else {
	stream << "NULL";
    }
    return stream;
}

//
// stream input method
//
istream& operator>> (istream& stream, CmdLine &c) {
    static char buf [CmdLine::commandBufferSize];
    stream.getline (buf, CmdLine::commandBufferSize);
    if (stream) {
	c.parse (buf);
    }
    return stream;
}

