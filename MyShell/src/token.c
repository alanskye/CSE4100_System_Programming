#include "token.h"

static struct token* token_new(char* word, int type) {
    struct token* token = malloc(sizeof(struct token));
    token->word = sigsafe_malloc(sizeof(char) * (strlen(word) + 1));
    strcpy(token->word, word);
    token->type = type;
    return token;
}

static void token_free(struct token* token) {
    assert (token != NULL);
    free(token->word);
    free(token);
}

static void token_replace(struct token* token, char *word, int type) {
    free(token->word);
    token->word = sigsafe_malloc(sizeof(char) * (strlen(word) + 1));
    strcpy(token->word, word);
    token->type = type;
}

static void token_remove_quotes(struct token* token) {
    printf("before trim = %s\n", token->word);
    int word_len = strlen(token->word);
    char qtype = CHAR_NULL;
    int temp_idx = 0;
    for (int word_idx = 0; word_idx < word_len; word_idx++) {
        char ch = token->word[word_idx];
        if (qtype == CHAR_NULL && (ch == CHAR_QUOTE || ch == CHAR_DBQUOTE)) {
            qtype = ch;
        }
        else if (ch == qtype) {
            qtype = 0;
        }
        else {
            token->word[temp_idx++] = ch;
        }
    }
    token->word[temp_idx++] = CHAR_NULL;
    token->word = sigsafe_realloc(token->word, sizeof(char) * temp_idx);
    printf("after trim = %s\n", token->word);
}

static void clear_buffer(char* word, int* idx) {
    memset(word, 0, sizeof(char) * (*idx));
    *idx = 0;
}

int tokenize(const char cmd_buffer[], const int cmd_len, struct list* token_list) {
    assert (cmd_buffer != NULL);

    if (cmd_len == 0) {
        return 0;
    }

    char ch;
    char* word_buffer = sigsafe_calloc(cmd_len, sizeof(char));
    int cmd_idx = 0, word_idx = 0;
    enum tokenizer_state state = TOKENIZER_NORMAL;

    list_init(token_list);

    do {
        ch = cmd_buffer[cmd_idx];

        switch (state) {
            case TOKENIZER_NORMAL:
                if (ch == CHAR_AMP || ch == CHAR_PIPE) {
                    if (word_idx > 0) {
                        // end the token that was reading before
                        list_push_back(token_list, &token_new(word_buffer, TOKEN_WORD)->elem);
                        clear_buffer(word_buffer, &word_idx);
                    }
                    word_buffer[word_idx++] = ch;
                    list_push_back(token_list, &token_new(word_buffer, ch)->elem);
                    clear_buffer(word_buffer, &word_idx);
                }

                else if (ch == CHAR_QUOTE) {
                    state = TOKENIZER_QUOTED;
                    word_buffer[word_idx++] = CHAR_QUOTE;
                }

                else if (ch == CHAR_DBQUOTE) {
                    state = TOKENIZER_DBQUOTED;
                    word_buffer[word_idx++] = CHAR_DBQUOTE;
                }

                else if (ch == CHAR_ESCSEQ) {
                    word_buffer[word_idx++] = ch;
                }

                else if (ch == CHAR_WSPACE) {
                    if (word_idx > 0) {
                        list_push_back(token_list, &token_new(word_buffer, TOKEN_WORD)->elem);
                        clear_buffer(word_buffer, &word_idx);
                    }
                }

                else if (ch != CHAR_NULL) {
                    // general case
                    word_buffer[word_idx++] = ch;
                }

                break;

            case TOKENIZER_QUOTED:
                word_buffer[word_idx++] = ch;
                if (ch == CHAR_QUOTE) {
                    state = TOKENIZER_NORMAL;
                }
                break;

            case TOKENIZER_DBQUOTED:
                word_buffer[word_idx++] = ch;
                if (ch == CHAR_DBQUOTE) {
                    state = TOKENIZER_NORMAL;
                }
                break;
        }

        if (ch == CHAR_NULL) {
            printf("cmd len = %d, cmd_idx = %d, word idx = %d, %d\n", cmd_len, cmd_idx, word_idx, word_buffer[1]);
            printf("before the end, size = %zu\n", list_size(token_list));
            if (word_idx > 0) {
                list_push_back(token_list, &token_new(word_buffer, TOKEN_WORD)->elem);
                clear_buffer(word_buffer, &word_idx);
            }
            printf("after the end, size = %zu\n", list_size(token_list));
        }

        cmd_idx++;

    } while (ch != CHAR_NULL);

    for (   struct list_elem* it = list_begin(token_list); 
            it != list_end(token_list); 
            it = list_next(it)) {

        struct token* token = list_entry(it, struct token, elem);
        // printf("glob tokword = %s %d\n", token->word, token->type);
        if (token->type == TOKEN_WORD) {
            glob_t glob_buf;
            glob(token->word, GLOB_TILDE, NULL, &glob_buf);
            if (glob_buf.gl_pathc > 0) {
                // replace the first one
                token_replace(token, glob_buf.gl_pathv[0], TOKEN_WORD);
                // the current token with the first one
                for (size_t i = 1; i < glob_buf.gl_pathc; i++) {
                    list_insert(&token->elem, &token_new(glob_buf.gl_pathv[i], TOKEN_WORD)->elem);
                }
            }
            else {
                token_remove_quotes(token);
            }
        }
    }
    free(word_buffer);
    return 0;
}

void print_token(struct token* token) {
    printf("token->word's len = %d\n", strlen(token->word));
    printf("token->word: %s\n", token->word);
    printf("token->type: %d\n", token->type);
}


void show_tokens(struct list* token_list) {
    int i = 0;
    for (   struct list_elem* it = list_begin(token_list); 
            it != list_end(token_list); 
            it = list_next(it)) {
        struct token* token = list_entry(it, struct token, elem);
        printf("token [%d]\n", i++);
        print_token(token);
    }
}

void destroy_token_list(struct list* token_list) {
    assert (token_list != NULL);
    while (!list_empty(token_list)) {
        struct list_elem* it = list_pop_front(token_list);
        struct token* token = list_entry(it, struct token, elem);
        if (token->word != NULL) {
            free(token->word);
        }
        free(token);
    }
    free(token_list);
}

