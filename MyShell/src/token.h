#ifndef __MYSH_TOKEN_H__
#define __MYSH_TOKEN_H__

#include "lib/commonlibs.h"
#include "lib/sigsafelibs.h"
#include "lib/list.h"

#define CHAR_NULL       '\0'
#define CHAR_NEWLINE    '\n'
#define CHAR_WSPACE     ' '
#define CHAR_TAB        '\t'
#define CHAR_PIPE       '|'
#define CHAR_AMP        '&'
#define CHAR_QUOTE      '\''
#define CHAR_DBQUOTE    '\"'
#define CHAR_ESCSEQ     '\\'

enum tokenizer_state {
    TOKENIZER_NORMAL,
    TOKENIZER_QUOTED,
    TOKENIZER_DBQUOTED,
};

enum token_type {
    TOKEN_WORD = 0,
    TOKEN_PIPE,
    TOKEN_AMP
};

struct token {
    struct list_elem elem;
    char* word;
    enum token_type type;
};

int tokenize(const char cmd_buffer[], const int cmd_len, struct list* token_list);
void print_token(struct token* token);
void show_tokens(struct list* token_list);
void destroy_token_list(struct list* token_list);

#endif
