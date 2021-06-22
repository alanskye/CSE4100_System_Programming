#ifndef __STOCKDB_H__
#define __STOCKDB_H__

#include "csapp.h"
#include "bst.h"

#define STOCKDB_ERROR -1
#define STOCKDB_SUCCESS 0
#define STOCKDB_FAILED 1

extern bst_t* stockdb;

void stockdb_load();
void stockdb_show(char** buf_next);
void stockdb_save();
int stockdb_buy(int id, int cnt);
int stockdb_sell(int id, int cnt);

#endif
