#include "sighandler.h"

handler_t* SIGINT_DEFAULT_HANDLER;
handler_t* SIGTSTP_DEFAULT_HANDLER;
handler_t* SIGCHLD_DEFAULT_HANDLER;

void shell_signal_handler(int signum) {
    pid_t pid;
    int status;
    switch (signum) {
        case SIGCHLD:
            while ((pid = waitpid(-1, &status, WNOHANG)) > 0);
            break;
        case SIGINT: // do nothing
        case SIGTSTP:
            break;
    }
}

void shell_init_sighandlers() {
    SIGINT_DEFAULT_HANDLER = Signal(SIGINT, shell_signal_handler);
    SIGTSTP_DEFAULT_HANDLER = Signal(SIGTSTP, shell_signal_handler);
    SIGCHLD_DEFAULT_HANDLER = Signal(SIGCHLD, shell_signal_handler);
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

