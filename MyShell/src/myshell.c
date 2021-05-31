#include "myshell.h"

// prototypes
static int read_command(char**);
static void free_resources(char*, struct list*, struct syntax_tree*);
static void refresh_shell();

int main() {

    shell_init_sighandlers();

    while (true) {
        char* cmd_buffer = NULL;
        struct list* token_list = NULL;
        struct syntax_tree* syntax_tree = NULL;
        
        refresh_shell();

        printf("CSE4100-SP-P4 > ");

        int cmd_len = read_command(&cmd_buffer);
        if (cmd_len == 0) {
            free_resources(cmd_buffer, token_list, syntax_tree);
            continue;
        }

        token_list = sigsafe_malloc(sizeof(struct list));
        tokenize(cmd_buffer, cmd_len, token_list);
        if (list_size(token_list) == 0) {
            free_resources(cmd_buffer, token_list, syntax_tree);
            continue;
        } 

        if (parse(token_list, &syntax_tree) != 0) {
            free_resources(cmd_buffer, token_list, syntax_tree);
            continue;
        }

        execute(syntax_tree);
        free_resources(cmd_buffer, token_list, syntax_tree);
    }
    return 0;
}

static int read_command(char** cmd_buffer) {
    ssize_t read_len;
    size_t zero = 0;
    while (true) {
        *cmd_buffer = NULL;
        read_len = getline(cmd_buffer, &zero, stdin);
        if (read_len < 0 && errno == EINTR) {
            clearerr(stdin);
        }
        else {
            (*cmd_buffer)[--read_len] = CHAR_NULL;
            break;
        }
    }
    if (feof(stdin)) {
        putchar('\n');
        _exit(0);
    }
    return (int)read_len;
}



static void free_resources(
        char* cmd_buffer, 
        struct list* token_list, 
        struct syntax_tree* syntax_tree) 
{
        if (cmd_buffer != NULL) {
            free(cmd_buffer);
        }
        if (token_list != NULL) {
            destroy_token_list(token_list);
        }
        if (syntax_tree != NULL) {
            syntax_tree_delete(syntax_tree);
        }
}

static void refresh_shell() {
    rewind(stdin);
    char** argv = sigsafe_malloc(sizeof(char*) * 3);
    argv[0] = malloc(sizeof(char) * sizeof("stty"));
    strcpy(argv[0], "stty");
    argv[1] = malloc(sizeof(char) * sizeof("sane"));
    strcpy(argv[1], "sane");
    argv[2] = NULL;
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(argv[0], argv) == -1) {
            printf("\nfailed to refersh shell\n");
        }
    }
    else if (pid > 0) {
        waitpid(pid, NULL, 0);
    }
    else {
        puts("[myshell has failed to refresh the shell]");
        return;
    }
    free(argv[0]);
    free(argv[1]);
    free(argv);
}

