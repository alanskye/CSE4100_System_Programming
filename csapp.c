#include "csapp.h"


static size_t sio_strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') {
        ++i;
    }
    return i;
}

static void sio_reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

static void sio_ltoa(long v, char s[], int b) {
    int c, i = 0;
    do {
        s[i++] = ((c = (v % b)) < 10) ? c + '0' : c - 10 + 'a';
    } while ((v /= b) > 0);
    s[i] = '\0';
    sio_reverse(s);
}

ssize_t sio_puts(char s[]) {
    return write(STDOUT_FILENO, s, sio_strlen(s));
}

ssize_t sio_putl(long v) {
    char s[128];
    sio_ltoa(v, s, 10);
    return sio_puts(s);
}

void sio_error(char s[]) {
    sio_puts(s);
    _exit(1);
}


/* wrappers for the SIO routines */
ssize_t Sio_puts(char s[]) {
    ssize_t n;
    if ((n = sio_puts(s)) < 0) {
        sio_error("Sio_puts error");
    }
    return n;
}

ssize_t Sio_putl(long v) {
    ssize_t n;
    if ((n = sio_putl(v)) < 0) {
        sio_error("Sio_putl error");
    }
    return n;
}

void Sio_error(char s[]) {
    sio_error(s);
}


void unix_error(char* msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

pid_t Fork() {
    pid_t pid;
    if ((pid = fork()) < 0) {
        unix_error("Fork error");
    }
    return pid;
}

handler_t* Signal(int signum, handler_t* handler) {
    struct sigaction action, old_action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);   // block sigs of type being handled
    action.sa_flags = SA_RESTART;   // restart syscalls if possible
    if (sigaction(signum, &action, &old_action) < 0) {
        unix_error("Signal error");
    }
    return (old_action.sa_handler);
}

unsigned int Sleep(unsigned int secs) {
    return sleep(secs);
}

