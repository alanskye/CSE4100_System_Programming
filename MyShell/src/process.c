#include "process.h"

struct process_info {
    int argc;
    char** argv;
    bool sync_mode;
    bool pipe_in;
    bool pipe_out;
    int fd_in;
    int fd_out;
};

void allocate_string(char** arr, char* s) {
    size_t len = strlen(s);
    *arr = sigsafe_malloc(sizeof(char) * len);
    strcpy(*arr, s);
}


void process_info_destory(struct process_info* p_info) {
    if (p_info->argv != NULL) {
        for (int i = 0; i < p_info->argc; i++) {
            free(p_info->argv[i]);
        }
        free(p_info->argv);
    }
}

void process_jobs(int mode, int target) {
    bool bgfg_done = false;
    bool failed = false;
    char** ps_argv = sigsafe_malloc(sizeof(char*) * 6);

    allocate_string(&ps_argv[0], "ps");
    allocate_string(&ps_argv[1], "-o");
    allocate_string(&ps_argv[2], "s,pid,cmd");
    allocate_string(&ps_argv[3], "--ppid");
    ps_argv[4] = malloc(sizeof(char) * 16);
    sprintf(ps_argv[4], "%d", getpid());
    ps_argv[5] = NULL;

    pid_t ps_pid;
    int fd[2];
    int fd_stdout = dup(STDOUT_FILENO);
    pipe(fd);
    if ((ps_pid = fork()) == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        if (execvp(ps_argv[0], ps_argv) == -1) {
            dup2(fd_stdout, STDOUT_FILENO);
            perror("error occured while listing jobs");
            _exit(-1);
        }
    }
    else if (ps_pid > 0) {
        size_t zero = 0;
        char* buffer = NULL;
        char status;
        int ret;
        int pid;
        char cmd[128];
        int cnt = 1;
        waitpid(ps_pid, &ret, 0);
        if (ret == 0) {
            close(fd[1]);
            FILE *ps_fp = fdopen(fd[0], "r");
            bool skip = true;
            while (getline(&buffer, &zero, ps_fp) > 0) {
                if (skip) {
                    skip = false;
                }
                else {
                    sscanf(buffer, "%c %d %s", &status, &pid, cmd);
                    if (pid == ps_pid) {
                        continue;
                    }

                    if (mode == 0) { // jobs
                        printf("[%d] ", cnt);
                        switch (status) {
                            case 'S':
                            case 'D':
                                printf("sleeping");
                                break;
                            case 'T':
                                printf("stopped");
                                break;
                            case 't':
                                printf("stopped while debugging");
                            case 'R':
                                printf("running");
                                break;
                            case 'Z':
                                printf("<defunct>");
                            default:
                                putchar(status);
                        }
                        printf("\t%s\n", cmd);
                    }
                    else if (mode == 1) { // bg
                        if (cnt == target) {
                            kill(pid, SIGCONT);
                            bgfg_done = true;
                        }
                        break;
                    }
                    else if (mode == 2) { // fg
                        if (cnt == target) {
                            kill(pid, SIGCONT);
                            bgfg_done = true;
                            waitpid(pid, NULL, WUNTRACED);
                        } 
                        break;
                    }
                    else if (mode == 3) { // kill
                        if (cnt == target) {
                            kill(pid, SIGKILL);
                            bgfg_done = true;
                        }
                        break;
                    }
                    cnt++;
                }
            }
            free(buffer);
            close(fd[0]);
        }
        else {
            failed = true;
        }
    }
    else {
        failed = true;
    }

    if (mode != 0 && bgfg_done == false) {
        printf("[job %%%d not exists]\n", target);
    }

    if (failed) {
        perror("error occured while listing jobs");
    }
    for (int i = 0; i < 5; i++) {
        free(ps_argv[i]);
    }
    free(ps_argv);
}

int process_cont(int argc, char* argv[], int mode) {
    int target = 1;
    if (argc == 1) {
        process_jobs(mode, target);
    }
    else if ((argc == 2 || argc == 3) && sscanf(argv[1], "%%%d\n", &target) == 1) {
        process_jobs(mode, target);
    }
    else {
        if (mode == 1) {
            printf("bg: ");
        }
        else if (mode == 2) {
            printf("fg: ");
        }
        else {
            printf("kill (MyShell implemented): ");
        }
        printf("invalid arguments!\n");
        return -1;
    }
    return 0;
}


int process_cd(int argc, char** argv) {
    if (argc == 1) {
        struct passwd* pw = getpwuid(getuid());
        const char* home = pw->pw_dir;
        chdir(home);
    }
    else if (argc == 2) {
        if (chdir(argv[1]) != 0) {
            perror(argv[1]);
        }
    }
    else {
        printf("cd: too many arguments\n");
        return -1;
    }
    return 0;
}

int process_init(
        struct process_info* p_info,
        struct syntax_tree* syntax_tree,
        bool sync_mode,
        bool pipe_in,
        bool pipe_out,
        int fd_in,
        int fd_out) {

    assert (syntax_tree != NULL && syntax_tree->type == SYNTAX_TREE_PATH);

    struct syntax_tree* node;
    node = syntax_tree;
    p_info->argc = 0;
    while (node != NULL && 
            (node->type == SYNTAX_TREE_PATH || 
             node->type == SYNTAX_TREE_ARG)) {
        node = node->right;
        p_info->argc++;
    }

    p_info->argv = sigsafe_malloc(sizeof(char*) * (p_info->argc + 1));

    node = syntax_tree;
    for (int i = 0; i < p_info->argc; i++) {
        allocate_string(&p_info->argv[i], node->data);
        node = node->right;
    }

    p_info->argv[p_info->argc] = NULL;
    p_info->sync_mode   = sync_mode;
    p_info->pipe_in     = pipe_in;
    p_info->pipe_out    = pipe_out;
    p_info->fd_in       = fd_in;
    p_info->fd_out      = fd_out;

    return 0;
}

void process_exec(struct process_info* p_info) {

#define MYSH_BUILT_IN(x) (strcmp(p_info->argv[0], x) == 0)
    if (MYSH_BUILT_IN("cd")) {
        process_cd(p_info->argc, p_info->argv);
        return;
    }
    if (MYSH_BUILT_IN("exit")) {
        _exit(0);
    }
    if (MYSH_BUILT_IN("jobs")) {
        process_jobs(0, 0);
        return;
    }
    if (MYSH_BUILT_IN("bg")) {
        process_cont(p_info->argc, p_info->argv, 1);
        return;
    }
    if (MYSH_BUILT_IN("fg")) {
        process_cont(p_info->argc, p_info->argv, 2);
        return;
    }
    if (MYSH_BUILT_IN("kill")) {
        process_cont(p_info->argc, p_info->argv, 3);
        return;
    }
    pid_t pid;

    int fd_stdout = dup(STDIN_FILENO);

    if ((pid = fork()) == 0) {
        // restore sigint for child 
        shell_restore_sigint();
        shell_restore_sigtstp();

        if (p_info->sync_mode == ASYNC) {
            //...
        }

        if (p_info->pipe_in) {
            dup2(p_info->fd_in, STDIN_FILENO);
        }
        if (p_info->pipe_out) {
            dup2(p_info->fd_out, STDOUT_FILENO);
        }

        if (execvp(p_info->argv[0], p_info->argv) == -1) {
            dup2(fd_stdout, STDOUT_FILENO);
            printf("Command '%s' not found.\n", p_info->argv[0]);
            _exit(127);
        }

    }
    else if (pid < 0) {
        perror("fork error");
        return;
    } 
    else {
        // wait until child process be queued
        if (p_info->sync_mode == SYNC) {
            waitpid(pid, NULL, WUNTRACED);
        }
        else if (p_info->sync_mode == ASYNC) { // ASYNC
            return;
        }
        else {
            assert(false);
        }
    }
}

void process(
        struct syntax_tree* syntax_tree,
        bool sync_mode,
        bool pipe_in,
        bool pipe_out,
        int fd_in,
        int fd_out) {

    struct process_info p_info = { 0 };

    process_init(
            &p_info,
            syntax_tree,
            sync_mode,
            pipe_in,
            pipe_out,
            fd_in,
            fd_out);

    process_exec(&p_info);
    process_info_destory(&p_info);
}
