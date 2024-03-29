#ifndef __MYSH_SIGSAFFELIBS_H__
#define __MYSH_SIGSAFFELIBS_H__

#include <commonlibs.h>


typedef void handler_t(int);

void unix_error(char msg[]);

#define MAX_TRIAL 32
void* sigsafe_malloc(size_t size);
void* sigsafe_calloc(size_t num, size_t size);
void* sigsafe_realloc(void* ptr, size_t new_size);

#endif
