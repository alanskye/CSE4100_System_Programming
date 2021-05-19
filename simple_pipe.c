#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#define MAX_LINE 128

int main() {
    int n, fd[2]; // fd[0]: input, fd[1]: output
    pid_t pid;
    char line[100];
    if (pipe(fd) < 0 || (pid = fork()) < 0) {
        exit(-1);
    }
    else if (pid > 0) { // parent
        close(fd[0]);
        write(fd[1], "Hello World\n", 12);
        wait(NULL);
    }
    else { // child
        close(fd[1]);
        n = read(fd[0], line, MAX_LINE);
        write(STDOUT_FILENO, line, n);
    }
}

