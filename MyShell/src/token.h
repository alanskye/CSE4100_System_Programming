#ifndef __MYSH_TOKEN_H__
#define __MYSH_TOKEN_H__

#include "lib/commonlibs.h"
#include "lib/sigsafelibs.h"
#include "lib/list.h"

enum char_type {
    CHAR_NULL       = '\0',
    CHAR_NEWLINE    = '\n',
    CHAR_WSPACE     = ' ',
    CHAR_TAB        = '\t',
    CHAR_PIPE       = '|',
    CHAR_AMP        = '&',
    CHAR_QUOTE      = '\'',
    CHAR_DBQUOTE    = '\"',
    CHAR_ESCSEQ     = '\\'
};

enum tokenizer_state {
    TOKENIZER_NORMAL,
    TOKENIZER_QUOTED,
    TOKENIZER_DBQUOTED,
};

enum token_type {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_AMP
};

struct token {
    struct list_elem elem;
    char* word;
    enum token_type type;
};

int tokenize(char line_buffer[], int len, struct list** token_list);
void destroy_tokens();

#endif
