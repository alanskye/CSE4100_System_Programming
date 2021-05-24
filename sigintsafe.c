#include "csapp.h"

// sigintsafe.c
// async-signal-safe handler
void sigint_handler(int sig) {
    Sio_puts("sigint safe");
    _exit(0);
}

int main() {
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        unix_error("signal error");
    }
    pause();
    return 0;
}
