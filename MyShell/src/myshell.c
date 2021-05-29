#include "myshell.h"

// prototypes
static int read_command(char**);
static void free_resources(char*, struct list*, struct syntax_tree*);

int main() {
    // TODO: ignore CTRL+C
    
    shell_init_and_ignore();
    shell_activate_reaper();

    while (true) {
        char* cmd_buffer = NULL;
        struct list* token_list = NULL;
        struct syntax_tree* syntax_tree = NULL;
        
        // TODO: read a line
        rewind(stdin);
        int cmd_len = read_command(&cmd_buffer);
        if (cmd_len == 0) {
            continue;
        }

        // TODO: tokenzie the given line
        token_list = malloc(sizeof(struct list));
        tokenize(cmd_buffer, cmd_len, token_list);
        if (list_size(token_list) == 0) {
            free_resources(cmd_buffer, token_list, syntax_tree);
            continue;
        }
        else {
            // show_tokens(token_list);
        }

        // TODO: build a parse tree
        if (parse(token_list, &syntax_tree) != 0) {
            free_resources(cmd_buffer, token_list, syntax_tree);
            continue;
        }

        // syntax_tree_traverse(syntax_tree);
        
        // TODO: execute over the tree
        execute(syntax_tree);
        // TODO: clear the resources

    }
    return -1;
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
        exit(0);
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

