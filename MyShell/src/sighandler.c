#include "sighandler.h"


#define job_killed 2

void (*SIGINT_DEFAULT_HANDLER)(int);
void (*SIGTSTP_DEFAULT_HANDLER)(int);
void (*SIGCHLD_DEFAULT_HANDLER)(int);
void (*SIGCONT_DEFAULT_HANDLER)(int);

void shell_init_and_ignore() {
    list_init(&job_list);
    SIGINT_DEFAULT_HANDLER = Signal(SIGINT, SIG_IGN);
    SIGTSTP_DEFAULT_HANDLER = Signal(SIGTSTP, SIG_IGN);
    SIGCONT_DEFAULT_HANDLER = Signal(SIGCONT, SIG_IGN);

    Signal(SIGCONT, SIGCONT_DEFAULT_HANDLER);
}

void shell_activate_reaper() {
    Signal(SIGCHLD, shell_sigchld_handler);
}

void shell_restore_sigint() {
    printf("-- test --\n");
    Signal(SIGINT, SIGINT_DEFAULT_HANDLER);
}

void shell_restore_sigtstp() {
    assert(Signal(SIGTSTP, process_sigtstp_handler) == SIG_IGN);
}

void shell_restore_sigcont() {
    Signal(SIGCONT, process_sigcont_handler);
}

void shell_sigchld_handler(int signum) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        job_list_update(pid, job_killed);
        if (status != COMMAND_NOT_FOUND) {
            printf("Process [%06d] terminated with exit code %d\n", pid, status);
        }
    }
}


handler_t* Signal(int signum, handler_t* handler) {
    struct sigaction action, old_action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    sigaction(signum, &action, &old_action);
    printf("old handler is %u\n", old_action);
    return old_action.sa_handler;
}

