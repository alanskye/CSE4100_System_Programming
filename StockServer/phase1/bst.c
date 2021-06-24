#include "bst.h"

static inline int max(int a, int b) {
    return a > b ? a : b;
}

static inline int get_h_diff(bst_node_t* node) {
    return node != NULL ? node->h_diff : 0;
}

static inline int get_balance(bst_node_t* node) {
    return node != NULL ? (get_h_diff(node->left) - get_h_diff(node->right)) : 0;
}

bst_t* bst_new(comparator_t* comparator) {
    bst_t* bst = malloc(sizeof(bst_t));
    bst->root = NULL;
    bst->size = 0;
    bst->comparator = comparator;
    return bst;
}

bst_node_t* rotate_left(bst_node_t *x) {
    bst_node_t* y = x->right;
    bst_node_t* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->h_diff = max(get_h_diff(x->left), get_h_diff(x->right)) + 1;
    y->h_diff = max(get_h_diff(y->left), get_h_diff(y->right)) + 1;

    return y;
}

bst_node_t* rotate_right(bst_node_t* y) {
    bst_node_t* x = y->left;
    bst_node_t* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->h_diff = max(get_h_diff(y->left), get_h_diff(y->right)) + 1;
    x->h_diff = max(get_h_diff(x->left), get_h_diff(x->right)) + 1;

    return x;
}

bst_node_t* insert(bst_node_t* self, bst_node_t* node, comparator_t* comparator) {

    if (self == NULL) {
        return node;
    }

    int result = comparator(self, node);

    if (result < 0) { // self < node => node < key
        self->right = insert(self->right, node, comparator);
    }
    else if (result > 0) { // self > node => node > key
        self->left = insert(self->left, node, comparator);
    }
    else {
        return self;
    }

    self->h_diff = max(get_h_diff(self->left), get_h_diff(self->right)) + 1;

    int balance = get_balance(self);

    // key < node->left->key == node < self->left = self->left > node
    if (balance > 1 && comparator(self->left, node) > 0) {
        return rotate_right(self);
    }

    // key > node->right->key == node > self->right == self->right < node
    if (balance < -1 && comparator(self->right, node) < 0) {
        return rotate_left(self);
    }

    // key > node->left->key == node > self->left == self->left < node
    if (balance > 1 && comparator(self->left, node) < 0) {
        self->left = rotate_left(self->left);
        return rotate_right(self);
    }

    // key < node->right->key == node < self->right == self->right > node
    if (balance < -1 && comparator(self->right, node) > 0) {
        self->right = rotate_right(self->right);
        return rotate_left(self);
    }

    return self;
}

void bst_insert(bst_t* bst, bst_node_t* bst_node) {
    if ((bst->root = insert(bst->root, bst_node, bst->comparator)) == bst_node) {
        bst->size++;
    }
}

#ifdef BST_TEST
typedef struct int_node {
    bst_node_t bst_node;
    int key;
} int_node;

int compare_int(const bst_node_t* a, const bst_node_t* b) {
    return ((int_node*)a)->key - ((int_node*)b)->key;
}

void test(bst_node_t* node) {
    if (node == NULL) {
        puts("NULL");
        return;
    }
    test(node->left);
    printf("key = %d\n", bst_node_entry(node, int_node, bst_node)->key);
    test(node->right);
}

int main() {
    bst_t* bst = bst_new(compare_int);
    for (int i = 100; i > 0; i--) {
        int_node* item = malloc(sizeof(int_node));
        item->key = i;
        bst_insert(bst, &item->bst_node);
    }
    test(bst->root);
    return 0;
}
#endif

