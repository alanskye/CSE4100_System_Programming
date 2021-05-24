#include "token.h"

static struct token* new_token(char* word, int type) {
    struct token* token = malloc(sizeof(struct token));
    token->word = sigsafe_malloc(sizeof(char) * strlen(word) + 1);
    strcpy(token->word, word);
    token->type = type;
    return token;
}

static void clear_buffer(char* word, int* idx) {
    memset(word, 0, sizeof(char) * (*idx));
    *idx = 0;
}

int tokenize(char cmd_buffer[], int cmd_len, struct list** token_list_ptr) {
    assert (cmd_buffer != NULL);

    if (cmd_len == 0) {
        return 0;
    }

    struct list* token_list = *token_list_ptr;
    char ch;
    char* word_buffer = sigsafe_calloc(cmd_len, sizeof(char));
    enum char_type ch_type;
    int cmd_idx = 0, word_idx = 0;
    int tok_cnt = 0;
    enum tokenizer_state state = TOKENIZER_NORMAL;

    list_init(token_list);
    
    do {
        ch = cmd_buffer[cmd_idx];
        ch_type = ch;

        switch (state) {
            case TOKENIZER_NORMAL:
                switch (ch_type) {
                    case CHAR_AMP:
                    case CHAR_PIPE:
                        if (word_idx > 0) {
                            // end the token that was reading before
                            list_push_back(token_list, &new_token(word_buffer, ch_type)->elem);
                            clear_buffer(word, &word_idx);
                        }

                        word_buffer[word_idx] = ch_type;
                        list_push_back(token_list, &new_token(word_buffer, ch_type)->elem);
                        clear_buffer(word, &word_idx);
                        break;
                              
                    case CHAR_QUOTE:
                        state = TOKENIZER_QUOTED;
                        word_buffer[word_idx++] = CHAR_QUOTE;
                        break;

                    case CHAR_DBQUOTE:
                        state = TOKENIZER_DBQUOTED;
                        word_buffer[word_idx++] = CHAR_QUOTE;
                        break;
                    
                    case CHAR_WSPACE:
                        if (word_idx > 0) {
                            list_push_back(token_list, &new_token(word_buffer, ch_type)->elem);
                            clear_buffer(word, &word_idx);
                        }
                        break;
           
                    default:
                        word_buffer[word_idx++] = ch;
                        break;
                }
                break;

            case TOKENIZER_QUOTED:
                word_buffer[word_idx++] = ch;
                if (ch_type == CHAR_QUOTE) {
                    state = TOKENIZER_NORMAL;
                }
                break;

            case TOKENIZER_DBQUOTED:
                word_buffer[word_idx++] = ch;
                if (ch_type == CHAR_DBQUOTE) {
                    state = TOKENIZER_DBQUOTED;
                }
                break;
        }

        if (ch_type == CHAR_NULL) {
            if (word_idx > 0) {
                list_push_back(token_list, &new_token(word_buffer, ch_type)->elem);
                clear_buffer(word, &word_idx);
            }
        }

        cmd_idx++;

    } while (ch != '\0');

    for (   struct elem* it = list_begin(token_list); 
            it != list_end(token_list); 
            it = list_next(it)) {

        struct token* token = list_entry(it, struct token, elem);
        if (token->type == TOKEN_WORD) {
            glob_t glob_buf;
            glob(token->word, GLOB_TILDE, NULL, &glob_buf);
            if (glob_buf.gl_pathc > 0) {
                // replace the first one
                it->word = 0;

                // the current token with the first one
            }
        }
    }

    free(word_buffer);
}

void destroy_tokens() {

}

