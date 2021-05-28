#include "execute.h"



void execute_cmd_line(struct syntax_tree* cmd_line);
void execute_job(struct syntax_tree* job, bool sync_mode);
void execute_pipe(struct syntax_tree *job, bool sync_mode);
void execute_cmd(
        struct syntax_tree* job,
        bool sync_mode,
        bool pipe_in,
        bool pipe_out,
        int fd_in,
        int fd_out);

void execute_cmd_line(struct syntax_tree* cmd_line) {
    if (cmd_line == NULL) {
        return;
    }
    
    switch (cmd_line->type) {
        case SYNTAX_TREE_BCKGRD:
            execute_job(cmd_line->left, ASYNC);
            execute_cmd_line(cmd_line->right);
            break;
        default:
            execute_job(cmd_line, SYNC);
    }
}


void execute_job(struct syntax_tree* job, bool sync_mode) {
    if (job == NULL) {
        return;
    }

    switch (job->type) {
        case SYNTAX_TREE_PIPE:
            execute_pipe(job, sync_mode);
            break;
        case SYNTAX_TREE_PATH:
            execute_cmd(job, sync_mode, false, false, FD_NULL, FD_NULL);
            break;
        default:
            assert (false);
    }
}

void execute_pipe(struct syntax_tree* job_piped, bool sync_mode) {
    int fd[2];
    int fd_in, fd_out;

    pipe(fd);
    fd_in = fd[0]; fd_out = fd[1];

    execute_cmd(job_piped->left, sync_mode, false, true, FD_NULL, fd_out);

    struct syntax_tree *job = job_piped->right;

    while (job != NULL && job->type == SYNTAX_TREE_PIPE) {
        close(fd_out);
        pipe(fd);
        fd_out = fd[1];
        execute_cmd(job->left, sync_mode, true, true, fd_in, fd_out);
        close(fd_in);
        fd_in = fd[0];
        job = job->right;
    }

    fd_in = fd[0];
    close(fd_out);

    execute_cmd(job, sync_mode, true, false, fd_in, FD_NULL);
    close(fd_in);
}



void execute_cmd(
        struct syntax_tree* job,
        bool sync_mode,
        bool pipe_in,
        bool pipe_out,
        int fd_in,
        int fd_out) {
    if (job == NULL) {
        return;
    }
    process(job, sync_mode, pipe_in, pipe_out, fd_in, fd_out);
}


void execute(struct syntax_tree* syntax_tree) {

    execute_cmd_line(syntax_tree);
}

