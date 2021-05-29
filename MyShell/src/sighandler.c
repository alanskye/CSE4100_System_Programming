#include "sighandler.h"


void shell_sigchld_handler(int signum) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0);
}

void shell_init_and_ignore() {
    Signal(SIGINT, SIG_IGN);
    Signal(SIGTSTP, SIG_IGN);
}

void shell_activate_reaper() {
    Signal(SIGCHLD, shell_sigchld_handler);
}

void shell_restore_sigint() {
    Signal(SIGINT, SIG_DFL);
}

void shell_restore_sigtstp() {
    Signal(SIGTSTP, SIG_DFL);
}

void shell_restore_sigchld() {
    Signal(SIGCHLD, SIG_DFL);
}

handler_t* Signal(int signum, handler_t* handler) {
    struct sigaction action, old_action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    sigaction(signum, &action, &old_action);
    return old_action.sa_handler;
}

