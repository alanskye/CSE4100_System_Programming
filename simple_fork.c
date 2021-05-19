#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Error-handling wrapper
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

// fork Example
int main() {
    pid_t pid;
    int x = 1;
    pid = Fork();
    if (pid == 0) {
        printf("child: x = %d\n", ++x);
        exit(0);
    }
    
    printf("parent: x = %d\n", --x);
    exit(0);
}

