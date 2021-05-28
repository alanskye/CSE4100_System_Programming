#ifndef __MYSH_EXECUTE_H__
#define __MYSH_EXECUTE_H__

#include "lib/commonlibs.h"
#include "lib/sigsafelibs.h"
#include "syntax.h"
#include "process.h"

void execute(struct syntax_tree* syntax_tree);

#endif
