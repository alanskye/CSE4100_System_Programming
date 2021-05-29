#ifndef __MYSH_SIGHANDLER_H__
#define __MYSH_SIGHANDLER_H__

#include "lib/list.h"
#include "lib/commonlibs.h"
#include "lib/sigsafelibs.h"

typedef void handler_t(int);

void shell_init_sighandlers();
void shell_restore_sigint();
void shell_restore_sigtstp();
void shell_restore_sigchld();

handler_t* Signal(int signum, handler_t* handler);

#endif
