#include "parser.h"

/*
 * Shell Grammer for Recursive Descent Parser
 *
 * <command_list>   ::= <job>
 *                  |   <job> '&'
 *                  |   <job> '&' <command_list>
 *
 * <job>            ::= <command>
 *                  |   <job> '|' <command>
 *
 * <command>        ::= <path> <arg_list>
 *
 * <arg_list>       ::= <arg> <arg_list>
 *                  |   NULL
 *
 */

#define selector_macro(tree, func)  glob_token_elem = saved_token_elem; \
                                    if ((tree = func) != NULL) {        \
                                        return tree;                    \
                                    }   

struct list* glob_token_list;
struct list_elem* glob_token_elem;

bool term(enum token_type required_type, char** data);
struct syntax_tree* cmd_list_selector();
struct syntax_tree* cmd_list_1();           // <job> 
struct syntax_tree* cmd_list_2();           // <job> '&'
struct syntax_tree* cmd_list_3();           // <job> '&' <command_list>

struct syntax_tree* job_selector();
struct syntax_tree* job_1();        // <command>
struct syntax_tree* job_2();        // <job> '|' <command>

struct syntax_tree* cmd_selector();          
struct syntax_tree* cmd_1();        // <path> <arg_list>

struct syntax_tree* arg_list_selector();          
struct syntax_tree* arg_list_1();        // <arg> <arg_list>
struct syntax_tree* arg_list_2();        // NULL

   
bool term(enum token_type required_type, char** data) {
    if (glob_token_elem == list_end(glob_token_list)) {
        return false;
    }
    struct token* token = list_entry(glob_token_elem, struct token, elem);

    // printf("\t\treq type = %d\n", required_type);
    // print_token(token);
    
    if (token->type == required_type) {
        if (data != NULL) {
            *data = sigsafe_malloc(sizeof(char) * (strlen(token->word) + 1));
            strcpy(*data, token->word);
        }
        glob_token_elem = list_next(glob_token_elem);
        return true;
    }

    glob_token_elem = list_next(glob_token_elem);
    return false;
}

int parse(struct list* token_list, struct syntax_tree** syntax_tree) {
    assert (list_size(token_list) > 0);
    glob_token_list = token_list;
    glob_token_elem = list_begin(token_list);
    *syntax_tree = cmd_list_selector();
    // error checking
    if (glob_token_elem != list_end(token_list) || *syntax_tree == NULL) {
        printf("Syntax Error: ");
        if (glob_token_elem == list_end(glob_token_list)) {
            printf("at end of line\n");
        }
        else {
            struct token* token = list_entry(glob_token_elem, struct token, elem);
            printf("at %s\n", token->word);
        }
        return -1;
    }
    return 0;
} 

struct syntax_tree* cmd_list_selector() {
    struct syntax_tree* cmd_list;
    struct list_elem* saved_token_elem = glob_token_elem;

    selector_macro(cmd_list, cmd_list_3());
    selector_macro(cmd_list, cmd_list_2());
    selector_macro(cmd_list, cmd_list_1());

    return NULL;
}

// <job>
struct syntax_tree* cmd_list_1() { 
    return job_selector();
}

// <job> '&'
struct syntax_tree* cmd_list_2() {
    struct syntax_tree* job;
    struct syntax_tree* result;

    if ((job = job_selector()) == NULL) {
        return NULL;
    }

    if (!term(CHAR_AMP, NULL)) {
        syntax_tree_delete(job);
        return NULL;
    }

    result = syntax_tree_new(NULL, SYNTAX_TREE_BCKGRD);
    syntax_tree_merge(result, job, NULL);

    return result;
}

// <job> '&' <command_list>
struct syntax_tree* cmd_list_3() {
    struct syntax_tree* job;
    struct syntax_tree* cmd_list;
    struct syntax_tree* result;
    
    if ((job = job_selector()) == NULL) {
        return NULL;
    }
    
    if (!term(CHAR_AMP, NULL)) {
        syntax_tree_delete(job);
        return NULL;
    }

   
    if ((cmd_list = cmd_list_selector()) == NULL) {
        syntax_tree_delete(job);
        return NULL;
    }

    result = syntax_tree_new(NULL, SYNTAX_TREE_BCKGRD);
    syntax_tree_merge(result, job, cmd_list);
    puts("cmd_list_3!");
    return result;
}

struct syntax_tree* job_selector() {
    struct syntax_tree* job;
    struct list_elem* saved_token_elem = glob_token_elem;

    selector_macro(job, job_2());
    selector_macro(job, job_1());

    return NULL;
}

struct syntax_tree* job_1() {       // <command>
    return cmd_selector();
}

struct syntax_tree* job_2() {       // <command> '|' <job>
    struct syntax_tree* cmd;
    struct syntax_tree* job;
    struct syntax_tree* result;
    
    if ((cmd = cmd_selector()) == NULL) {
        return NULL;
    }

    if (!term(CHAR_PIPE, NULL)) {
        syntax_tree_delete(cmd);
        return NULL;
    }
    puts("PIPE DETECTED");
    // print_token(list_entry(glob_token_elem, struct token, elem));

    if ((job = job_selector()) == NULL) {
        syntax_tree_delete(cmd);
        return NULL;
    }

    puts("pipe and job detected");
    result = syntax_tree_new(NULL, SYNTAX_TREE_PIPE);
    syntax_tree_merge(result, cmd, job);

    return result;
}

// <job> '|' <command>

struct syntax_tree* cmd_selector() {
    struct syntax_tree* cmd_tree;
    struct list_elem* saved_token_elem = glob_token_elem;

    selector_macro(cmd_tree, cmd_1());

    return NULL;
}

struct syntax_tree* cmd_1() {       // <path> <arg_list>
    struct syntax_tree* arg_list;
    struct syntax_tree* result;
    char* path;
    if (!term(TOKEN_WORD, &path)) {
        return NULL;
    }
    arg_list = arg_list_selector();

    result = syntax_tree_new(path, SYNTAX_TREE_PATH);
    syntax_tree_merge(result, NULL, arg_list);

    return result;
}

// <pathname> <arg_list>
struct syntax_tree* arg_list_selector() {
    struct syntax_tree* arg_list;
    struct list_elem* saved_token_elem = glob_token_elem;

    selector_macro(arg_list, arg_list_1());
    selector_macro(arg_list, arg_list_2());

    return NULL;
}

// <arg> <arg_list>
struct syntax_tree* arg_list_1() {
    struct syntax_tree* arg_list;
    struct syntax_tree* result;
    char* arg;
    
    if (!term(TOKEN_WORD, &arg)) {
        return NULL;
    }

    arg_list = arg_list_selector();

    result = syntax_tree_new(arg, SYNTAX_TREE_ARG);
    syntax_tree_merge(result, NULL, arg_list);

    return result;
}

struct syntax_tree* arg_list_2() {
    return NULL;
}



