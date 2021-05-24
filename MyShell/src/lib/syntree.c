#include "syntree.h"

struct syntax_tree* syntax_tree_new( 
        char* data,  
        enum syntax_tree_type type) {
    struct syntax_tree* syntax_tree = 
        sigsafe_malloc(sizeof(struct syntax_tree));
    syntax_tree->type = type;
    if (data != NULL) {
        syntax_tree->data = sigsafe_malloc(sizeof(char) * (strlen(data) + 1));
        strcpy(syntax_tree->data, data);
    }
    else {
        syntax_tree->data = NULL;
    }
    syntax_tree->left = NULL;
    syntax_tree->right = NULL;
    return syntax_tree;
}


void syntax_tree_merge(
        struct syntax_tree* root,
        struct syntax_tree* left,
        struct syntax_tree* right) {
    assert (root != NULL);
    root->left = left;
    root->right = right;
}


void syntax_tree_delete(struct syntax_tree* root) {
    if (root == NULL) {
        return;
    }
    if (root->data != NULL) {
        free(root->data);
    }
    syntax_tree_delete(root->left);
    syntax_tree_delete(root->right);
    free(root);
}

