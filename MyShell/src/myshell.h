#ifndef __MYSH_SHELL_H__
#define __MYSH_SHELL_H__

#include "lib/commonlibs.h"
#include "lib/list.h"

#include "token.h"
#include "parser.h"
#include "syntax.h"
#include "execute.h"
#include "sighandler.h"

extern struct list job_list;

struct job_info {
    struct list_elem elem;
    pid_t pid;
    int state;
};

#define job_running 0
#define job_stopped 1
#define job_killed  2

#endif
