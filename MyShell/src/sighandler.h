#ifndef __MYSH_SIGHANDLER_H__
#define __MYSH_SIGHANDLER_H__

#include "lib/list.h"
#include "lib/commonlibs.h"
#include "lib/sigsafelibs.h"

typedef void handler_t(int);

void shell_init_and_ignore();
void shell_activate_reaper();
void shell_restore_sigint();
void shell_restore_sigtstp();
void shell_restore_sigchld();

handler_t* Signal(int signum, handler_t* handler);

handler_t shell_sigchld_handler;
handler_t shell_child_stopped;

extern handler_t process_sigchld_handler;
extern handler_t process_sigcont_handler;
extern handler_t process_sigtstp_handler;


#endif
