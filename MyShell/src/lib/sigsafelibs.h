#ifndef __MYSH_SIGSAFFELIBS_H__
#define __MYSH_SIGSAFFELIBS_H__

#include <commonlibs.h>

void unix_error(char msg[]);

#define MAX_TRIAL 32
void* sigsafe_malloc(size_t size);
void* sigsafe_calloc(size_t num, size_t size);
#endif
