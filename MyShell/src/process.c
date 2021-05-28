#include "process.h"

struct job_info* job_info_new(pid_t pid, int state) {
    struct job_info* job_info = malloc(sizeof(job_info));
    job_info->pid = pid;
    job_info->state = state;
    return job_info;
}

bool job_list_find(pid_t pid) {
    struct list_elem* it;
    for (   it = list_begin(&job_list);
            it != list_end(&job_list);
            it = list_next(it)
        )
    {
        struct job_info* job_info = list_entry(it, struct job_info, elem);
        if (job_info->pid == pid) {
            return true;
        }
    }
    return false;
}

void job_list_update(pid_t pid, int state) {
    struct list_elem* it;
    for (   it = list_begin(&job_list);
            it != list_end(&job_list);
            it = list_next(it)
        )
    {
        struct job_info* job_info = list_entry(it, struct job_info, elem);
        if (job_info->pid == pid) {
            if (state == job_killed) {
                list_remove(it);
                puts("killed");
                return;
            }
            else {
                job_info->state = state;
                return;
            }
        }
    }
}

void job_list_show() {
    struct list_elem* it;
    for (   it = list_begin(&job_list);
            it != list_end(&job_list);
            it = list_next(it)
        )
    {
        struct job_info* job_info = list_entry(it, struct job_info, elem);
        printf("Process [%8d]: ");
        if (job_info->state == job_running) {
            printf("running");
        }
        else {
            printf("stopped");
        }
        printf("\n");
    }
}

void process_sigtstp_handler(int signum) {
    job_list_update(getpid(), job_stopped);
    SIGTSTP_DEFAULT_HANDLER(signum);
}

void process_sigcont_handler(int signum) {
    job_list_update(getpid(), job_running);
    SIGCONT_DEFAULT_HANDLER(signum);
}

struct process_info {
    int argc;
    char** argv;
    bool sync_mode;
    bool pipe_in;
    bool pipe_out;
    int fd_in;
    int fd_out;
};

void process_info_destory(struct process_info* p_info) {
    if (p_info->argv != NULL) {
        for (int i = 0; i < p_info->argc; i++) {
            free(p_info->argv[i]);
        }
        free(p_info->argv);
    }
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

int process_exit() {
    _exit(0);
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
        p_info->argv[i] = sigsafe_malloc(sizeof(char) * (strlen(node->data) + 1));
        strcpy(p_info->argv[i], node->data);
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

    if (strcmp(p_info->argv[0], "cd") == 0) {
        process_cd(p_info->argc, p_info->argv);
        return;
    }
    else if (strcmp(p_info->argv[0], "exit") == 0) {
        process_exit();
    }

    pid_t pid;

    int fd_stdout = dup(STDIN_FILENO);

    if ((pid = fork() == 0)) {
        // restore sigint for child 
        shell_restore_sigint();
        shell_restore_sigtstp();
        shell_restore_sigcont();
        printf("exec child pid = %d\n", getpid());
        if (p_info->sync_mode == ASYNC) {
            //...
        }

        if (p_info->pipe_in) {
            dup2(p_info->fd_in, STDIN_FILENO);
        }
        if (p_info->pipe_out) {
            dup2(p_info->fd_out, STDOUT_FILENO);
        }

        list_push_back(&job_list, &job_info_new(pid, job_running)->elem);
        if (execvp(p_info->argv[0], p_info->argv) == -1) {
            job_list_update(pid, job_killed);
            dup2(fd_stdout, STDOUT_FILENO);
            printf("Command '%s' not found.\n", p_info->argv[0]);
            _exit(1);
        }
        else {
            
        }
    }
    else if (pid < 0) {
        perror("fork error");
        return;
    }

    if (p_info->sync_mode == SYNC) {
        while (job_list_find(pid));
        puts("sync job finisihed");
    }
    else if (p_info->sync_mode == ASYNC) { // ASYNC
        return;
    }
    else {
        assert(false);
    }
}

int process(
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
