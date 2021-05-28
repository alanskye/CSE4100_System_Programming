#ifndef __MYSH_PARSER_H__
#define __MYSH_PARSER_H__

#include "lib/commonlibs.h"
#include "lib/sigsafelibs.h"
#include "syntax.h"
#include "token.h"

int parse(
        struct list* token_list, 
        struct syntax_tree** syntax_tree);

#endif
