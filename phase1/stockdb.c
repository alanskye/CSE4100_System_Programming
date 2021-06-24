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

static void stockdb_show2(bst_node_t* self, char** buf_next);
static void stockdb_save2(bst_node_t* self, FILE* fp);
static void stock_print(bst_node_t* self, char** buf_next);
static stock_t* stock_search(bst_node_t* self, int id);

/* implementations */

void stockdb_load() {
    char buffer[MAXLINE];
    int id, cnt, price;
    FILE* fp = Fopen("stock.txt", "r");
    stockdb = bst_new(compare_stock);
    while (Fgets(buffer, MAXLINE, fp) != NULL) {
        sscanf(buffer, "%d %d %d", &id, &cnt, &price);
        stock_t* stock = stock_new(id, cnt, price);
        bst_insert(stockdb, &stock->bst_node);
    }
    Fclose(fp);
}

void stockdb_show(char** buf_next) {
    stockdb_show2(stockdb->root, buf_next);
}

void stockdb_save() {
    FILE *fp = Fopen("stock.txt", "w");
    stockdb_save2(stockdb->root, fp);
    Fclose(fp);
    exit(0);
}

int stockdb_buy(int id, int cnt) {
    stock_t* stock = stock_search(stockdb->root, id);
    if (stock == NULL) {
        return STOCKDB_ERROR;
    }
    int success;
    P(&stock->mutex);
    if (stock->cnt >= cnt) {
        stock->cnt -= cnt;
        success = STOCKDB_SUCCESS;
    }
    else {
        success = STOCKDB_FAILED;
    }
    V(&stock->mutex);
    return success;
}

int stockdb_sell(int id, int cnt) {
    stock_t* stock = stock_search(stockdb->root, id);
    if (stock == NULL) {
        return STOCKDB_ERROR;
    }
    P(&stock->mutex);
    stock->cnt += cnt;
    V(&stock->mutex);
    return STOCKDB_SUCCESS;
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

static void stockdb_show2(bst_node_t* self, char** buf_next) {
    if (self == NULL) {
        return;
    }
    stockdb_show2(self->left, buf_next);
    stock_print(self, buf_next);
    stockdb_show2(self->right, buf_next);
}

static void stock_print(bst_node_t* self, char** buf_next) {
    stock_t* stock = bst_node_entry(self, stock_t, bst_node);
    int len;
    P(&stock->mutex);
    sprintf(*buf_next, "%d %d %d\n", stock->id, stock->cnt, stock->price);
    len = strlen(*buf_next);
    *buf_next += len;
    V(&stock->mutex);
}

static stock_t* stock_search(bst_node_t* self, int id) {
    stock_t* stock;
    if (self == NULL) {
        return NULL;
    }
    stock = bst_node_entry(self, stock_t, bst_node);
    if (id < stock->id) {
        return stock_search(self->left, id);
    }
    else if (id == stock->id) {
        return stock;
    }
    else {
        return stock_search(self->right, id);
    }
}

static void stockdb_save2(bst_node_t* self, FILE* fp) {
    stock_t* stock;
    if (self == NULL) {
        return;
    }
    stockdb_save2(self->left, fp);
    stock = bst_node_entry(self, stock_t, bst_node);
    fprintf(fp, "%d %d %d\n", stock->id, stock->cnt, stock->price); 
    stockdb_save2(self->right, fp);
}

