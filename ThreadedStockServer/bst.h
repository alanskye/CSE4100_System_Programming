#ifndef __BST_H__
#define __BST_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define bst_node_entry(BST_NODE, STRUCT, MEMBER)    \
        ((STRUCT*) ((uint8_t*) &(BST_NODE)->h_diff  \
        - offsetof(STRUCT, MEMBER.h_diff)))

typedef struct node {
    struct node* left;
    struct node* right;
    int h_diff;
} bst_node_t;

typedef int comparator_t(const bst_node_t* a, const bst_node_t* b);

typedef struct bst {
    bst_node_t *root;
    size_t size;
    comparator_t* comparator;
} bst_t;

bst_t* bst_new(comparator_t* comparator);
void bst_insert(bst_t* bst, bst_node_t* bst_node);

#endif
