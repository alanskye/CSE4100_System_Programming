#include <stdio.h>
int main() {
    char** argv = malloc(sizeof(char*) * 10);
    argv[0] = malloc(sizeof("bg"));
    strcpy(argv[0], "bg");

    argv[1] = malloc(sizeof("%1"));
    strcpy(argv[1], "%1");

    argv[2] = NULL;
    if (execvp(argv[0], argv) == -1) {
        puts("failed");
    }
}
