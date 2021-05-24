#include "myshell.h"


// prototypes
static int read_command(char**);

int main() {
    // TODO: ignore CTRL+C
    
    while (true) {
        char* cmd_buffer = NULL;
        struct list* token_list;
        // TODO: read a line
        int cmd_len = read_command(&cmd_buffer);

        // TODO: tokenzie the given line
        tokenize(cmd_buffer, cmd_len, &token_list);
        show_tokens(token_list);

        // TODO: build a parse tree
        if (list_size(token_list) == 0) {
            continue;
        }
        // TODO: execute over the tree
        parse(token_list);
        // TODO: clear the resources
        if (cmd_buffer != NULL) {
            free(cmd_buffer);
        }
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

