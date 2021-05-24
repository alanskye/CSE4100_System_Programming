#ifndef __MYSH_SYNTREE_H__
#define __MYSH_SYNTREE_H__

#include <commonlibs.h>
#include <sigsafelibs.h>


enum syntree_type {
    SYNTREE_PATH    = (1 << 0);
    SYNTREE_ARG     = (1 << 1);
    SYNTREE_PIPE    = (1 << 2);
    SYNTREE_BCKGRD  = (1 << 3);
};

struct syntree {
    enum syntree_type;
    char *data;
    struct syntree* left;
    struct syntree* right;
};


#endif
