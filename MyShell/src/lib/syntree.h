#ifndef __MYSH_SYNTREE_H__
#define __MYSH_SYNTREE_H__

#include <commonlibs.h>
#include <sigsafelibs.h>

enum syntax_tree_type {
    SYNTAX_TREE_PATH    = (1 << 0);
    SYNTAX_TREE_ARG     = (1 << 1);
    SYNTAX_TREE_PIPE    = (1 << 2);
    SYNTAX_TREE_BCKGRD  = (1 << 3);
};

struct syntax_tree {
    char *data;
    enum syntax_tree_type type;
    struct syntax_tree* left;
    struct syntax_tree* right;
};

struct syntax_tree* syntax_tree_new(
        char* data,
        enum syntax_tree_type type);

void syntax_tree_merge(
        struct syntax_tree* root,
        struct syntax_tree* left,
        struct syntax_tree* right);

void syntax_tree_delete(struct syntax_tree* root);

#endif
