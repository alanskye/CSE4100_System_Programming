#include "stockdb.h"

typedef struct stock {
    bst_node_t bst_node;
    int id;
    int cnt;
    int price;
    sem_t mutex;
} stock_t;

bst_t* stockdb = NULL;

/* headers */
static stock_t* stock_new(
        int id, 
        int cnt,
        int price);

int compare_stock(const bst_node_t* a, const bst_node_t* b);

static void stockdb_show2(bst_node_t* self);
static void stock_print(bst_node_t* self);

/* implementations */

void stockdb_load() {
    char buffer[MAXLINE];
    FILE *fp;
    int id, cnt, price;
    fp = Fopen("stock.txt", "r");
    stockdb = bst_new(compare_stock);
    while (Fgets(buffer, MAXLINE, fp) != NULL) {
        sscanf(buffer, "%d %d %d", &id, &cnt, &price);
        stock_t* stock = stock_new(id, cnt, price);
        bst_insert(stockdb, &stock->bst_node);
    }
    Fclose(fp);
}

void stockdb_show() {
    stockdb_show2(stockdb->root);
}

static stock_t* stock_new(
        int id, 
        int cnt,
        int price) {
    stock_t* stock = Malloc(sizeof(stock_t));
    stock->id = id;
    stock->cnt = cnt;
    stock->price = price;
    Sem_init(&stock->mutex, 0, 1);
    return stock;
}

int compare_stock(const bst_node_t* a, const bst_node_t* b) {
    return 
        bst_node_entry((bst_node_t*)a, stock_t, bst_node)->id -
        bst_node_entry((bst_node_t*)b, stock_t, bst_node)->id;
}

static void stockdb_show2(bst_node_t* self) {
    if (self == NULL) {
        return;
    }
    stockdb_show2(self->left);
    stock_print(self);
    stockdb_show2(self->right);
}

static void stock_print(bst_node_t* self) {
    stock_t* stock = bst_node_entry(self, stock_t, bst_node);
    printf("-- stock info\n");
    printf("[   id] = %06d\n", stock->id);
    printf("[ left] = %06d\n", stock->cnt);
    printf("[price] = %06d\n", stock->price);
}

