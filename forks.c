#include "csapp.h"

void fork1() {
    int x = 1; // identical private space for each process
    pid_t pid = fork();
    if (pid == 0) {
        printf("Child has x = %d\n", ++x);
    }
    else {
        printf("Parent has x = %d\n", --x);
    }
    printf("Bye from process with pid = %d, x = %d\n", getpid(), x);
}

void fork2() {
    puts("L0");
    fork();
    puts("L1");
    fork();
    puts("Bye");
}

/* nested forks in parent */
void fork4() {
    puts("L0");
    if (fork() != 0) {
        puts("L1");
        if (fork() != 0) {
            puts("L2");
        }
    }
    puts("Bye");
}

/* nested forks in child */
void fork5() {
    puts("L0");
    if (fork() == 0) {
        puts("L1");
        if (fork() == 0) {
            puts("L2");
        }
    }
    puts("Bye");
}

void cleanup() {
    puts("Cleaning up");
}

void fork6() {
    // register a function to be called at normal process termination
    atexit(cleanup);
    fork();
    exit(0);
}

/* zombie example */
void fork7() {  // ./forks 7 & <- go to shell immediately
    if (fork() == 0) {
        printf("Terminating Child, PID = %d\n", getpid());
        exit(0);
    }
    else {
        printf("Runninng parent, PID = %d\n", getpid());
        while (1); // kill pid -> init reap the process
    }
}

/* non-terminating child example */
void fork8() {
    if (fork() == 0) {
        printf("Running child, PID = %d\n", getpid());
        while (1);
    }
    else {
        printf("Terminating Parent, PID = %d\n", getpid());
        exit(0);
    }
}


#define N 5
void fork10() {
    pid_t  pid[N];
    int i, child_status;
    for (i = 0; i < N; i++) {
        if ((pid[i] = fork()) == 0) {
            exit(100 + i);
        }
    }
    for (i = 0; i < N; i++) {
        pid_t wpid = wait(&child_status);
        if (WIFEXITED(child_status)) {
            printf("Child %d terminated with exit status %d\n",
                    wpid, WEXITSTATUS(child_status));
        }
        else {
            printf("Child %d terminate abormally\n", wpid);
        }
    }
}

void fork12() {
    pid_t pid[N];
    int i, child_status;
    for (i = 0; i < N; i++) {
        if ((pid[i] = fork()) == 0) {
            while (1);  // Child: infinite loop
        }
    }

    for (i = 0; i < N; i++) {
        printf("Killing process %d\n", pid[i]);
        // sending signals with the kill() function
        kill(pid[i], SIGINT);
    }

    for (i = 0; i < N; i++) {
        pid_t wpid = wait(&child_status);
        if (WIFEXITED(child_status)) {
            printf("Child %d terminated with exit status %d\n",
                    wpid, WEXITSTATUS(child_status));
        }
        else {
            printf("Child %d terminated abnormally with exit status %d\n", 
                    wpid, child_status);
        }
    }
}

// SIGCHLD handler that reaps one terminated child
int ccount = 0;
void child_handler(int sig) {
    int olderrno = errno;
    pid_t pid;
    if ((pid = wait(NULL)) < 0) {
        Sio_error("wait error");
    }
    ccount--;
    Sio_puts("Handler reaped child ");
    Sio_putl((long)pid);
    Sio_puts(" \n");
    sleep(1); // make sure that the children invokes SIGCHLD
    // -> pending signals are not queued but discarded
    // thus, don't use signals to count such events
    errno = olderrno;
}

// alternative of child_handler
void child_handler2(int sig) {
    int olderrno = errno;
    pid_t pid;
    while ((pid = wait(NULL)) > 0) {
        // Put wait in a loop to reap all terminated children
        ccount--;
        Sio_puts("Handler reaped child ");
        Sio_putl((long)pid);
        Sio_puts(" \n");
    }
    if (errno != ECHILD) {
        Sio_error("wait error");
    }
    errno = olderrno;
}

void fork14() {
    pid_t pid[N];
    int i;
    ccount = N;
    Signal(SIGCHLD, child_handler);
    for (i = 0; i < N; i++) {
        if ((pid[i] = Fork()) == 0) {
            Sleep(1);
            exit(0); // SIGCHLD to parent
        }
    }
    while (ccount > 0); // parent spins
}

void fork15() {
    pid_t pid[N];
    int i;
    ccount = N;
    Signal(SIGCHLD, child_handler2);
    for (i = 0; i < N; i++) {
        if ((pid[i] = Fork()) == 0) {
            Sleep(1);
            exit(0);
        }
    }
    while (ccount > 0);
}


int main(int argc, char** argv) {
    assert (argc == 2);
    int code;
    sscanf(argv[1], "%d", &code);
    switch (code) {
        case 1:     fork1();    break;
        case 2:     fork2();    break;
        case 4:     fork4();    break;
        case 5:     fork5();    break;
        case 6:     fork6();    break;
        case 7:     fork7();    break;
        case 8:     fork8();    break;
        case 10:    fork10();   break;
        case 12:    fork12();   break;
        case 14:    fork14();   break;
        case 15:    fork15();   break;
    }
    return 0;
}

