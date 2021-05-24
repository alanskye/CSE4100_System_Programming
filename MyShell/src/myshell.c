#include "myshell.h"


// prototypes
static void read_command(char**, size_t*);

int main() {
    // TODO: ignore CTRL+C
    
    while (true) {
        char* cmd_buffer;
        size_t cmd_len;
        struct list* token_list;
        // TODO: read a line
        read_command(&cmd_buffer, &cmd_len);

        // TODO: tokenzie the given line
        tokenize(cmd_buffer, cmd_len, &token_list);
        printf("size = %lu", list_size(token_list));
        // TODO: build a parse tree
        
        // TODO: execute over the tree
        
        // TODO: clear the resources
    }
    return -1;
}

static void read_command(char** cmd_buffer, size_t* cmd_len) {
    while (true) {
        *cmd_buffer = NULL;
        ssize_t read_len = getline(cmd_buffer, cmd_len, stdin);
        if (read_len < 0 && errno == EINTR) {
            clearerr(stdin);
        }
        else {
            break;
        }
    }
    if (feof(stdin)) {
        exit(0);
    }
}

