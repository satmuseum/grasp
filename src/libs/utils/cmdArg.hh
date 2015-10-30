//
// file:      CmdArg.h
// purpose:   header for command argument class
// history:   7/12/94 - JPMS - Modified from the MooCAF code (by Tim Burks).
//
#ifndef _CmdArg
#define _CmdArg


//
// argument class
//
class CmdArg {
public:

    // constructors

    CmdArg (void) {}
    CmdArg (char *str) { define_arg(str); }

    // destructor

    virtual ~CmdArg (void) { delete _ref_ptr; }

    // assignment operators

    CmdArg &operator= (CmdArg &arg);
    CmdArg &operator= (char *arg);

    // data access operators

    operator char*();
    operator int();
    operator long();
    operator float();
    operator double();

    // character shifting operator

    CmdArg &operator>> (int pos);

    int isEmpty() { return(strlen(_string) == 0); }

    //-------------------------------------------------------------------------
    // used to test string values
    //-------------------------------------------------------------------------

    int matches (char *str);

    //-------------------------------------------------------------------------
    // returns original string.
    //-------------------------------------------------------------------------

    char *string();

protected:
    char *_string;
    char *_ref_ptr;

    //-------------------------------------------------------------------------
    // allocates space for string.
    //-------------------------------------------------------------------------

    void define_arg(char *str);
};

#endif









