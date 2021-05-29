#ifndef __MYSH_PROCESS_H__
#define __MYSH_PROCESS_H__

#include "lib/list.h"
#include "lib/commonlibs.h"
#include "lib/sigsafelibs.h"
#include "myshell.h"
#include "sighandler.h"
#include "syntax.h"

#define FD_NULL 0
#define ASYNC   true
#define SYNC    false

void process(
        struct syntax_tree* node,
        bool sync_mode,
        bool pipe_in,
        bool pipe_out,
        int fd_in,
        int fd_out);


#endif
