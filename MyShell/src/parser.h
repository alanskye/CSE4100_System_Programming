#ifndef __MYSH_PARSER_H__
#define __MYSH_PARSER_H__

int parse(
        struct list* token_list, 
        struct syntax_tree** syntax_tree);

#endif
