#ifndef __MYSH_SIGHANDLER_H__
#define __MYSH_SIGHANDLER_H__

#include "lib/list.h"
#include "lib/commonlibs.h"
#include "lib/sigsafelibs.h"

#define job_running 0
#define job_stopped 1
#define job_killed  2

#define COMMAND_NOT_FOUND 127

typedef void handler_t(int);

void shell_init_and_ignore();
void shell_activate_reaper();
void shell_restore_sigint();
void shell_restore_sigtstp();

handler_t* Signal(int signum, handler_t* handler);

handler_t shell_sigchld_handler;
handler_t shell_child_stopped;

extern handler_t process_sigchld_handler;
extern handler_t process_sigcont_handler;
extern handler_t process_sigtstp_handler;

extern void (*SIGINT_DEFAULT_HANDLER)(int);
extern void (*SIGTSTP_DEFAULT_HANDLER)(int);
extern void (*SIGCHLD_DEFAULT_HANDLER)(int);
extern void (*SIGCONT_DEFAULT_HANDLER)(int);

#endif
