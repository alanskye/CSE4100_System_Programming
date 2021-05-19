#include "csapp.h"

void sigint_handler(int sig) {
    printf("signal %d\n", sig);
    sleep(2);
    exit(0);
}

int main() {
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        unix_error("signal_error");
    }
    // wait for the recepit of a signal
    pause();
    return 0;
}

