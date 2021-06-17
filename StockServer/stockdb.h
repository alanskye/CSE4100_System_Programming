#ifndef __STOCKDB_H__
#define __STOCKDB_H__

#include "csapp.h"
#include "bst.h"

extern bst_t* stockdb;

void stockdb_load();
void stockdb_show();

#endif
